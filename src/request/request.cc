#include <cstring>
#include <iostream>
#include <misc/socket.hh>
#include <netdb.h>
#include <request/request.hh>
#include <request/response.hh>
#include <sys/stat.h>

namespace http
{
    static std::string getline(std::string& s)
    {
        size_t delim = s.find_first_of("\r\n") + 2;
        std::string res = s.substr(0, delim);
        s = s.substr(delim);
        return res;
    }

    Request::Request()
        : head_size(0) {
    }

    Request::Request(std::string& s)
        : head_size(0)
    {
        try
        {
            std::cout << "\n" << s << "\n";
            std::string line = getline(s);
            head_size += line.size();
            auto start = line.begin();
            auto end = start;
            end += line.find_first_of(' ', 0);
            method = std::string(line.begin(), end);
            start = end + 1;
            end = line.begin() + line.find_first_of(' ', start - line.begin());
            uri = std::string(start, end);
            http_version = std::string(end + 1, line.end() - 2);

            // fill headers
            line = getline(s);
            head_size += line.size();
            while (line.size() && line.at(0) != '\r')
            {
                // creating pairs of two string separated by a semicolon and a
                // space
                size_t sep_idx = line.find_first_of(':', 0);
                headers.emplace(
                    std::string(line.begin(), line.begin() + sep_idx),
                    std::string(line.begin() + sep_idx + 2, line.end() - 2));

                line = getline(s);
                if (line != "")
                    head_size += line.size();
            }
            if (s.length())
            {
                message_body = s;
                message_body.resize(message_body.length());
            }

            path_info = std::make_pair("", -404);
        } catch (const std::exception& e)
        {
            path_info = std::make_pair("", -400);
        }
    }

    void Request::get_path()
    {
        path_info.first = config_ptr->root_ + uri;
        struct stat buf;
        int flags = S_IROTH + S_IRGRP + S_IRUSR;

        if (!stat(path_info.first.c_str(), &buf) && buf.st_mode & S_IFDIR)
        {
            auto tmp = path_info.first + config_ptr->default_file_;
            if (!stat(tmp.c_str(), &buf))
            {
                if (buf.st_mode & flags)
                    path_info.second = buf.st_size;
                else
                    path_info.second = -403;
                path_info.first = tmp;
            }
            else
            {
                if (!stat(path_info.first.c_str(), &buf) && config_ptr->auto_index_)
               {
                   if (buf.st_mode & S_IRUSR) 
                        path_info.second = -1;
                    else
                        path_info.second = -403;
               } 
            }
        }
        else
        {
            if (!stat(path_info.first.c_str(), &buf))
            {
                if (buf.st_mode & flags)
                    path_info.second = buf.st_size;
                else
                    path_info.second = -403;
            }
        }
    }

} // namespace http
