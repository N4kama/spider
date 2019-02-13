#include <request/request.hh>
#include <misc/socket.hh>
#include <netdb.h>
#include <iostream>
#include <cstring>

namespace http
{

std::string &recvLine(DefaultSocket &sock)
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

void fill_Request(DefaultSocket sock)
{
    Request req = Request();

    //fill info line
    std::string &line = recvLine(sock);
    auto start = line.begin();
    auto end = start;
    end += line.find_first_of(' ', 0);
    req.method = std::string(line.begin(), end);
    start = end + 1;
    end = line.begin() + line.find_first_of(' ', start - line.begin());
    req.url = std::string(start, end);
    req.http_version = std::string(end + 1, line.end() - 1);

    //fill headers
    line = recvLine(sock);
    while (line.size() && line.at(0) != '\n')
    {
        //creating pairs of two string separated by a semicolon and a space
        size_t sep_idx = line.find_first_of(':', 0);
        req.headers.emplace_back(std::string(line.begin(), line.begin() + sep_idx),
        std::string(line.begin() + sep_idx + 2, line.end() - 1));

        line = recvLine(sock);
    }

    //if message exists, it will be initialized
    if (line.size())
    {
        char buf[4096];
        while (sock.recv(buf, 4096) > 0)
        {
            req.message_body.append(buf);
        }
    }

    request_server(req, sock);
}

void request_server(struct Request r, DefaultSocket socketClient)
{

    /*handling error
    */

    if ((strncmp(r.http_version.c_str(), "HTTP/1.1", 8) != 0) || (strncmp(r.url.c_str(), "http://", 7) != 0))
    {
        socketClient.send(
             "<html><h1>http error: 501</h1><h2> There is place for only Head,"
             " Post and Get method allowed in this town cowboy</h2></html>",
             125);
    }
    if ((strstr(r.message_body.c_str(), "\nHost") == NULL) || (strstr(r.message_body.c_str(), "\nContent-Length") == NULL))
    {
        socketClient.send(
             "<html><h1>http error: 400</h1><h2> There is place for only Head,"
             " Post and Get method allowed in this town cowboy</h2></html>",
             125);
    }

    if (strncmp(r.method.c_str(),"GET", 3))
    {
        http_get(r, socketClient);
    }
    else if (strncmp(r.method.c_str(),"HEAD", 4))
    {
        http_head(r, socketClient);
    }
    else if (strncmp(r.method.c_str(),"POST", 4))
    {
        http_post(r, socketClient);
    }
    else
    {
        socketClient.send(
             "<html><h1>http error: 405</h1><h2> There is place for only Head,"
             " Post and Get method allowed in this town cowboy</h2></html>",
             125);
    }
}

} // namespace http