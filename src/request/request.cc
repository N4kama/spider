#include <cstring>
#include <iostream>
#include <misc/socket.hh>
#include <netdb.h>
#include <request/request.hh>
#include <request/response.hh>

namespace http
{
    std::string recvLine(Socket& sock)
    {
        std::string data = std::string("");
        char c = ' ';
        while (sock.recv(&c, 1) > 0)
        {
            data.append(1, c);
            if (c == '\n')
                break;
        }
        return data;
    }

    Request fill_Request(Socket& sock)
    {
        Request req = Request();

        // fill info line
        std::string line = recvLine(sock);
        auto start = line.begin();
        auto end = start;
        end += line.find_first_of(' ', 0);
        req.method = std::string(line.begin(), end);
        start = end + 1;
        end = line.begin() + line.find_first_of(' ', start - line.begin());
        req.uri = std::string(start, end);
        req.http_version = std::string(end + 1, line.end() - 2);

        // fill headers
        line = recvLine(sock);
        while (line.size() && line.at(0) != '\r')
        {
            // creating pairs of two string separated by a semicolon and a space
            size_t sep_idx = line.find_first_of(':', 0);
            req.headers.emplace_back(
                std::string(line.begin(), line.begin() + sep_idx),
                std::string(line.begin() + sep_idx + 2, line.end() - 2));

            line = recvLine(sock);
        }

        // if message exists, it will be initialized
        // well not really because others might want to get it themselves
        return req;
    }

} // namespace http