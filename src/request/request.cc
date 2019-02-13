#include <request/request.hh>
#include <misc/socket.hh>
#include <netdb.h>
#include <iostream>
#include <cstring>

namespace http
{

std::string& recvLine(DefaultSocket &sock)
{
    std::string data("");
    char c = ' ';
    while (sock.recv(&c, 1) > 0)
    {
        data.append(c);
        if (c == '\n')
            break;
    }
    return data;
}


void fill_Request(DefaultSocket sock)
{
    Request req = Request();

    //fill info line
    std::string& line = recvLine(sock);
    auto start = line.begin();
    auto end = start;
    end += line.find_first_of(' ', 0);
    req.method = std::string(line.begin(), end);
    start = end + 1;
    end = line.begin() + line.find_first_of(' ', start - line.begin());
    req.url = std::string(start, end);
    req.http_version = std::string(end + 1, line.end());

    //fill headers
    line = recvLine(sock);
    while (line.size() && line.at(0) != '\n')
    {
        //creating pairs
        //TODO

        line = recvLine(sock);
    }

    //fill message (don't need to call recvLine her)
    //TODO

    request_server(req, sock);
}

void request_server(struct Request r, DefaultSocket sock)
{

    /*handling error
    */

    if ((strncmp(r.http_version.c_str(), "HTTP/1.1", 8) != 0)
    || (strncmp(r.url.c_str(), "http://", 7) != 0))
    {
        send(socketClient,
             "<html><h1>http error: 501</h1><h2> There is place for only Head,"
             " Post and Get method allowed in this town cowboy</h2></html>",
             125, 0);
    }
    if ((strstr(r.message_body.c_str(), "\nHost") == NULL)
        || (strstr(r.message_body.c_str(), "\nContent-Length") == NULL))
    {
        send(socketClient,
             "<html><h1>http error: 400</h1><h2> There is place for only Head,"
             " Post and Get method allowed in this town cowboy</h2></html>",
             125, 0);
    }

    if (r.method == GET)
    {
        http_get(r);
    }
    else if (r.method == HEAD)
    {
        http_head(r);
    }
    else if (r.method == POST)
    {
        http_post(r);
    }
    else
    {
        sys::send(socketClient,
                  "<html><h1>http error: 405</h1><h2> There is place for only Head,"
                  " Post and Get method allowed in this town cowboy</h2></html>",
                  125 /*len*/, 0);
    }
}

} // namespace http