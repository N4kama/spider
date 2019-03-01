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
        std::stringstream str;
        std::string msg;
        if (r.path_info.second == -400)
        {
            status_code = BAD_REQUEST;
            set_error_rep(r, status_code);
            return;
        }
        if (error_405(r.method))
        {
            set_error_rep(r, METHOD_NOT_ALLOWED);
            return;
        }
        if (r.path_info.second == -404)
        {
            // error file does not exists
            status_code = NOT_FOUND;
            set_error_rep(r, status_code);
            return;
        }
        if (r.path_info.second == -403)
        {
            status_code = FORBIDDEN;
            set_error_rep(r, status_code);
            return;
        }
        if (strncmp(r.http_version.c_str(), "HTTP/1.1", 8) != 0)
        {
            status_code = UPGRADE_REQUIRED;
            set_error_rep(r, status_code);
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
            set_error_rep(r, status_code);
        }
    }

    void Response::set_error_rep(const struct Request& r, const STATUS_CODE& s)
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
        str << "Host: " << r.config_ptr->server_name_ << ':'
            << r.config_ptr->port_ << "\r\n";
        str << "Content-Length: " << msg.str().size() << "\r\n";

        if (s == METHOD_NOT_ALLOWED)
            str << "Allow: GET, HEAD, POST\r\n";

        str << "Connection: close\r\n\r\n";
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
        str << "Host: " << r.config_ptr->server_name_ << ':'
            << r.config_ptr->port_ << "\r\n";
        str << "Content-Length: " << r.path_info.second << "\r\n";
        str << "Connection: close\r\n\r\n";
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
            set_error_rep(r, status_code);
        }
    }

    void Response::http_rpost(struct Request r)
    {
        // now handling get request
        file_p = r.path_info.first;
        set_rep_heads(r);
        std::ofstream file;
        file.open(file_p);
        if (file.is_open())
        {
            file << r.message_body;
            rep += r.message_body;
            return;
        }
        // error file does not exists
        status_code = NOT_FOUND;
        set_error_rep(r, status_code);
    }
} // namespace http