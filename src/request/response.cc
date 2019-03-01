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
    Response::Response(const STATUS_CODE& s)
        : status_code(s)
    {}

    Response::Response(const struct Request& r, const STATUS_CODE& s)
        : status_code(s)
        , is_file(false)
    {
        std::stringstream str;
        std::string msg;
        if (strncmp(r.http_version.c_str(), "HTTP/1.1", 8) != 0)
        {
            msg = "<html><h1>http error: 501</h1><h2> There is place for only "
                  "Head, Post and Get method allowed in this town "
                  "cowboy</h2></html>";
            str << "HTTP/1.1 " << 501 << " "
                << "Not Implemented\r\n";
            str << "Date: " << get_date() << "\r\n";
            str << "Host: " << r.config_ptr->server_name_ << ':'
                << r.config_ptr->port_ << "\r\n";
            str << "Content-Length: " << msg.size() << "\r\n";
            str << "Connection: close\r\n\r\n";
            str << msg;
            rep = str.str();
            status_code = NOT_IMPLEMENTED;
        }
        /*
        else if ((strstr(r.message_body.c_str(), "\nHost") == NULL)
            || (strstr(r.message_body.c_str(), "\nContent-Length") == NULL))
        {
            rep = "<html><h1>http error: 400</h1><h2> There is place for only
        Head, Post and Get method allowed in this town cowboy</h2></html>",
            status_code = BAD_REQUEST;
        }*/
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
            msg = "<html><h1>http error: 405</h1><h2> There is place for only "
                  "Head, Post and Get method allowed in this town "
                  "cowboy</h2></html>";
            str << "HTTP/1.1 " << 405 << " "
                << "Method Not Allowed\r\n";
            str << "Date: " << get_date() << "\r\n";
            str << "Host: " << r.config_ptr->server_name_ << ':'
                << r.config_ptr->port_ << "\r\n";
            str << "Content-Length: " << msg.size() << "\r\n";
            str << "Connection: close\r\n\r\n";
            str << msg;
            rep = str.str();
            status_code = METHOD_NOT_ALLOWED;
        }
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

    void Response::http_rhead(struct Request r)
    {
        r = r;
        std::stringstream str;
        std::pair<STATUS_CODE, const char*> st = statusCode(status_code);

        str << "HTTP/1.1 " << st.first << " " << st.second << "\r\n";
        str << "Date: " << get_date() << "\r\n";
        str << "Host: " << r.config_ptr->server_name_ << ':'
            << r.config_ptr->port_ << "\r\n";
        str << "Content-Length: " << r.path_info.second << "\r\n";
        str << "Connection: close\r\n\r\n";
        rep = str.str();
    }

    void Response::http_rget(struct Request r)
    {
        // now handling get request
        file_p = r.path_info.first;
        if (r.path_info.second)
        {
            is_file = true;
            // fills header first hand
            http_rhead(r);
        } else
        {
            // error file does not exists
            std::pair<STATUS_CODE, const char*> err = statusCode(NOT_FOUND);
            std::stringstream ss;
            std::stringstream msg;

            msg << "<html><h1>http error: " << err.first << "</h1><h2> "
                << err.second << "</h2></html>";

            ss << "HTTP/1.1 " << err.first << " " << err.second << "\r\n";
            ss << "Date: " << get_date() << "\r\n";
            ss << "Host: " << r.config_ptr->server_name_ << ':'
               << r.config_ptr->port_ << "\r\n";
            ss << "Content-Length: " << msg.str().size() << "\r\n";
            ss << "Connection: close\r\n\r\n";
            ss << msg.str();
            rep = ss.str();
        }
    }

    void Response::http_rpost(struct Request r)
    {
        // now handling get request
        file_p = r.path_info.first;
        http_rhead(r);
        std::ofstream file;
        file.open(file_p);
        if (file.is_open())
        {
            file << r.message_body;
            return;
        }
        // error file does not exists
        std::pair<STATUS_CODE, const char*> err = statusCode(NOT_FOUND);
        std::stringstream ss;
        std::stringstream msg;

        msg << "<html><h1>http error: " << err.first << "</h1><h2> "
            << err.second << "</h2></html>";

        ss << "HTTP/1.1 " << err.first << " " << err.second << "\r\n";
        ss << "Date: " << get_date() << "\r\n";
        ss << "Host: " << r.config_ptr->server_name_ << ':'
           << r.config_ptr->port_ << "\r\n";
        ss << "Content-Length: " << msg.str().size() << "\r\n";
        ss << "Connection: close\r\n\r\n";
        ss << msg.str();
        rep = ss.str();
    }

} // namespace http