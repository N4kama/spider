#include <cstring>
#include <ctime>
#include <iostream>
#include <netdb.h>
#include <request/request.hh>
#include <request/response.hh>
#include <request/types.hh>
#include <socket/socket.hh>
#include <sstream>
#include <vector>

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
    {
        if (r.headers.find(std::string("Connection"))->second == "keep-alive")
        {
            keep_alive = true;
        }
        if (r.config_ptr->header_max_size_ && r.head_size > r.config_ptr->header_max_size_)
        {
            keep_alive = false;
            status_code = HEADER_FIELDS_TOO_LARGE;
            set_error_rep(status_code);
            return;
        }
        if (r.config_ptr->uri_max_size_ && r.uri.size() > r.config_ptr->uri_max_size_)
        {
            keep_alive = false;
            status_code = URI_TOO_LONG;
            set_error_rep(status_code);
            return;
        }
        if (r.config_ptr->payload_max_size_ && r.message_body.size() > r.config_ptr->payload_max_size_)
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

    void Response::http_rpost(struct Request r)
    {
        http_rget(r);
    }
} // namespace http