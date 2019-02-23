#include <request/request.hh>
#include <request/response.hh>
#include <socket/socket.hh>
#include <request/types.hh>
#include <iostream>
#include <netdb.h>
#include <cstring>
#include <vector>
#include <sstream>

namespace http
{

Response::Response(const STATUS_CODE& s)
    : status_code(s), variable_juste_pour_test(0)
{
}

Response::Response(const struct Request& r, const STATUS_CODE& s)
    : status_code(s), variable_juste_pour_test(r.status)
{
}



void http_get(struct Request r, DefaultSocket socketClient)
{
    std::cout << "GET " << r.uri.c_str() << '\n';

    struct sockaddr_in sock;
    struct hostent *host;
    std::stringstream str;
    std::pair<STATUS_CODE, const char *> err;

    host = gethostbyname(r.uri.c_str());
    if (!host)
    {
        //err = statusCode(NOT_FOUND);
            std::cout << "erreur!\n";
        str << "<html><h1>http error: " << /*err.first*/"ERR" << "</h1><h2> " << err.second << "</h2></html>";
        socketClient.send(&str, str.str().size());
        return;
    }

    sock.sin_port = htons(1234);
    sock.sin_family = AF_INET;
    sock.sin_addr.s_addr = *(reinterpret_cast<unsigned long *>(host->h_addr));//*((unsigned long*)host->h_addr);

    DefaultSocket socketServer = DefaultSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    socketServer.connect((sockaddr *) &sock, sizeof(sockaddr));
    std::cout << "connexion done!\n";
    socketClient.send("yes2\n", 5);
    socketServer.send("yes1\n", 5);

    str.str("");
    str << "GET / HTTP/1.1\r\nHost: " << r.uri.c_str() << " \r\nConnection: close\r\n\r\n";
    if (socketClient.send(&str, str.str().size()) < 0)
    {
        std::cout << "Client disconnected !\n";
        //socketClient.send("<html><h1>http error: 520</h1><h2> Le serv arrive pas a me rep</h2></html>", 67);
    }
    std::cout << "GET done\n";
}

void http_head(struct Request r, DefaultSocket socketClient)
{
    std::cout << "HEAD " << r.uri.c_str() << '\n';

    //struct sockaddr_in sock;
    struct hostent *host;
    std::stringstream str;
    std::pair<STATUS_CODE, const char *> err;

    host = gethostbyname(r.uri.c_str());
    if (!host)
    {
         //err = statusCode(NOT_FOUND);
            std::cout << "erreur!\n";
        str << "<html><h1>http error: " << /*err.first*/"ERR" << "</h1><h2> " << err.second << "</h2></html>";
       socketClient.send(&str, str.str().size());
        return;
    }

    //sock.sin_port = htons(port);
    //sock.sin_family = AF_INET;
    //sock.sin_addr.s_addr = *(reinterpret_cast<unsigned long *>(host->h_addr));//*((unsigned long*)host->h_addr);

    //DefaultSocket socketServer = DefaultSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //socketServer.connect((sockaddr *) &sock, sizeof(sockaddr));
    //std::cout << "connexion done!\n";

    str.str("");
    str << "HEAD / HTTP/1.1\r\nStatus: " << r.status << " Headers: ";
    for (auto it = r.headers.begin(); it < r.headers.end(); it++)
    {
        str << it->second << " ";
    }
    str << r.headers.end()->second;
    str << " \r\nConnection: close\r\n\r\n";

    if (socketClient.send(&str, str.str().size()) < 0)
        std::cout << "Client disconnected !\n";
        //socketClient.send("<html><h1>http error: 520</h1><h2> Le serv arrive pas à m'envoyer</h2></html>", 67);
    std::cout << "HEAD done\n";
}

void http_post(struct Request r, DefaultSocket socketClient)
{
    std::cout << "POST " << r.uri.c_str() << '\n';

    //struct sockaddr_in sock;
    struct hostent *host;
    std::stringstream str;
    std::pair<STATUS_CODE, const char *> err;

    host = gethostbyname(r.uri.c_str());
    if (!host)
    {
         //err = statusCode(NOT_FOUND);
            std::cout << "erreur!\n";
        str << "<html><h1>http error: " << /*err.first*/"ERR" << "</h1><h2> " << err.second << "</h2></html>";
       socketClient.send(&str, str.str().size());
        return;
    }

    //sock.sin_port = htons(port);
    //sock.sin_family = AF_INET;
    //sock.sin_addr.s_addr = *(reinterpret_cast<unsigned long *>(host->h_addr));//*((unsigned long*)host->h_addr);

    //DefaultSocket socketServer = DefaultSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //socketServer.connect((sockaddr *) &sock, sizeof(sockaddr));
    //std::cout << "connexion done!\n";

    str.str("");
    while (socketClient.recv(&str, 500) >= 0)
    {
        std::cout << str.str();
    }
    std::cout << "\nHEAD done\n";
}

} // namespace http