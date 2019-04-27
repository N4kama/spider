#include <arpa/inet.h>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <events/sendRequest.hh>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <request/request.hh>
#include <request/response.hh>
#include <request/types.hh>
#include <socket/socket.hh>
#include <socket/ssl-socket.hh>
#include <sstream>
#include <vector>

#include "../main.hh"

namespace http
{
    static bool error_405(const std::string& s)
    {
        std::vector<std::string> method(
            {"PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE"});
        return std::find(method.begin(), method.end(), s) != method.end();
    }

    Response::Response(const STATUS_CODE& s)
        : status_code(s)
    {}

    Response::Response(const struct Request& r, const STATUS_CODE& s)
        : status_code(s)
        , is_file(false)
        , keep_alive(false)
    {
        // Handling reverse-proxy request
        if (r.headers.find(std::string("Proxy-Connection")) != r.headers.end())
        {
            reverse_proxy_handler(r);
            return;
        }

        // Not documented here:
        auto p = r.headers.find(std::string("Connection"));
        if (p != r.headers.end() && p->second == "keep-alive")
        {
            keep_alive = true;
        }
        if (r.config_ptr->header_max_size_
            && r.head_size > r.config_ptr->header_max_size_)
        {
            keep_alive = false;
            status_code = HEADER_FIELDS_TOO_LARGE;
            set_error_rep(status_code);
            return;
        }
        if (r.config_ptr->uri_max_size_
            && r.uri.size() > r.config_ptr->uri_max_size_)
        {
            std::cout << "URI MAX: " << r.config_ptr->uri_max_size_ << '\n';
            keep_alive = false;
            status_code = URI_TOO_LONG;
            set_error_rep(status_code);
            return;
        }
        if (r.config_ptr->payload_max_size_
            && r.message_body.size() > r.config_ptr->payload_max_size_)
        {
            keep_alive = false;
            status_code = PAYLOAD_TOO_LARGE;
            set_error_rep(status_code);
            return;
        }
        if (r.path_info.second == -400)
        {
            status_code = BAD_REQUEST;
            set_error_rep(status_code);
            return;
        }
        if (error_405(r.method))
        {
            set_error_rep(METHOD_NOT_ALLOWED);
            return;
        }
        if (r.path_info.second == -404)
        {
            status_code = NOT_FOUND;
            set_error_rep(status_code);
            return;
        }
        if (r.path_info.second == -403)
        {
            status_code = FORBIDDEN;
            set_error_rep(status_code);
            return;
        }
        if (r.http_version.size() != 0
            && strncmp(r.http_version.c_str(), "HTTP/1.1", 8) != 0)
        {
            std::string method = r.http_version;
            if (strncmp(method.c_str(), "HTTP/", 5) != 0)
            {
                status_code = BAD_REQUEST;
            } else
            {
                std::string v_str(method.substr(5));
                std::string::size_type sz;
                float v = std::stof(v_str, &sz);

                if (v < 1.1)
                    status_code = UPGRADE_REQUIRED;
                else
                {
                    status_code = HTTP_VERSION_NOT_SUPPORTED;
                    keep_alive = false;
                }
            }
            set_error_rep(status_code);
            return;
        }
        if (strncmp(r.method.c_str(), "GET", 3) == 0)
        {
            http_rget(r);
        } else if (strncmp(r.method.c_str(), "HEAD", 4) == 0)
        {
            http_rhead(r);
        } else if (strncmp(r.method.c_str(), "POST", 4) == 0)
        {
            http_rpost(r);
        } else
        {
            status_code = BAD_REQUEST;
            keep_alive = false;
            set_error_rep(status_code);
        }
    }

    void Response::set_error_rep(const STATUS_CODE& s)
    {
        std::stringstream str;
        std::stringstream msg;
        std::pair<STATUS_CODE, const char*> err = statusCode(s);
        const int err_n = err.first;
        const char* err_m = err.second;

        msg << "<html><h1>Http Error: " << err_n
            << "</h1><h2> Error Message: " << err_m << "</h2></html>";

        str << "HTTP/1.1 " << err_n << " " << err_m << "\r\n";
        str << "Date: " << get_date() << "\r\n";

        str << "Content-Length: " << msg.str().size() << "\r\n";

        if (s == METHOD_NOT_ALLOWED)
            str << "Allow: GET, HEAD, POST\r\n";

        str << "Connection: " << (keep_alive ? "keep-alive" : "close");
        str << "\r\n\r\n";
        str << msg.str();
        rep = str.str();
        rep += "\r\n";
    }

    std::string get_date(void)
    {
        time_t rawtime;
        struct tm* timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, 200, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);
        std::string time(buffer);

        return time;
    }

    void Response::set_rep_heads(const struct Request& r)
    {
        std::stringstream str;
        std::pair<STATUS_CODE, const char*> st = statusCode(status_code);
        const int st_n = st.first;
        const char* st_m = st.second;

        str << "HTTP/1.1 " << st_n << " " << st_m << "\r\n";
        str << "Date: " << get_date() << "\r\n";
        str << "Content-Length: " << r.path_info.second << "\r\n";
        str << "Connection: " << (keep_alive ? "keep-alive" : "close");
        str << "\r\n\r\n";
        rep = str.str();
    }

    void Response::set_rep_list(struct Request& r)
    {
        DIR* dir = opendir(r.path_info.first.c_str());
        if (!dir)
        {
            status_code = NOT_FOUND;
            set_error_rep(status_code);
            return;
        }
        std::stringstream str;
        str << "<!DOCTYPE "
               "html><html>\n<head>\n<metacharset=utf-8>\n<title>Index of "
            << r.uri << "</title>\n</head>\n<body>\n<ul>\n";
        str << "<li><a href=\"/..\">..</a></li>" << '\n';
        struct dirent* dp;
        while ((dp = readdir(dir)) != NULL)
        {
            if (*dp->d_name == '.')
                continue;
            str << "<li><a href=\"" << r.uri;
            if (r.uri[r.uri.size() - 1] != '/')
                str << "/";
            str << dp->d_name << "\">" << dp->d_name << "</a></li>" << '\n';
        }
        closedir(dir);

        str << "</ul>\n</body>\n</html>";
        r.path_info.second = str.str().size();
        set_rep_heads(r);
        rep += str.str();
        rep += "\r\n";
    }

    void Response::http_rhead(struct Request r)
    {
        set_rep_heads(r);
        rep += "\r\n";
    }

    void Response::http_rget(struct Request r)
    {
        file_p = r.path_info.first;
        if (r.path_info.second > 0)
        {
            is_file = true;
            set_rep_heads(r);
            rep += "\r\n";
        } else
        {
            if (r.path_info.second == -1)
            {
                set_rep_list(r);
            } else
            {
                if (r.path_info.second == -404)
                {
                    status_code = NOT_FOUND;
                }
                if (r.path_info.second == -403)
                {
                    status_code = FORBIDDEN;
                }
                set_error_rep(status_code);
            }
        }
    }

    void Response::http_rpost(struct Request r)
    {
        http_rget(r);
    }

    void reverse_proxy_handler(const struct Request& r)
    {
        // Create new request to send to the real target
        Request req = Request(r);

        // Add the proxy address to the "forwarded" entry
        update_forwarded_header(req);

        // send new request to target
        if (req.config_ptr->no_ssl)
        {
            DefaultSocket mySock = DefaultSocket(AF_INET, SOCK_STREAM, 0);
            // init client socket on http (no ssl)
            event_register
                .register_ew<http::SendRequest, Request, shared_socket>(
                    std::forward<Request>(req),
                    std::make_shared<DefaultSocket>(mySock));
        } else
        {
            // fixme : same with ssl socket
        }

        // receive response from target

        // send back response to client
    }

    void update_forwarded_header(Request& req)
    {
        if (req.headers.find("Forwarded") == req.headers.end())
        {
            std::string entry = "for=";
            std::string ip = get_ip_address(req.clientSocket);
            entry.append(ip);
            entry.append(";host=" + req.config_ptr->server_name_);
            if (!req.config_ptr->no_ssl)
                entry.append(";proto=https");
            req.headers.emplace("Forwarded", entry);
        } else
        {
            std::string entry = req.headers.find("Forwarded")->second;
            entry.append(";for=");
            entry.append(get_ip_address(req.clientSocket));
            if (!req.config_ptr->no_ssl)
                entry.append(";proto=https");
        }
    }

    std::string get_ip_address(shared_socket s)
    {
        struct sockaddr_in6 addr6;
        struct sockaddr_in addr4;
        socklen_t len = s->is_ipv6() ? sizeof(addr6) : sizeof(addr4);
        if (s->is_ipv6())
            getsockname(s->fd_get()->fd_, (struct sockaddr*)&addr6, &len);
        else
            getsockname(s->fd_get()->fd_, (struct sockaddr*)&addr4, &len);
        char str[INET_ADDRSTRLEN];
        char str6[INET6_ADDRSTRLEN];
        std::string host = s->is_ipv6()
            ? inet_ntop(AF_INET6, &addr6.sin6_addr, str6, INET6_ADDRSTRLEN)
            : inet_ntop(AF_INET, &addr4.sin_addr, str, INET_ADDRSTRLEN);
        return std::string(host);
    }

} // namespace http