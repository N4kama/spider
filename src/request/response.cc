#include "../socket/default-socket.hh"


namespace http
{
    void http_get(struct Request r)
    {
        std::cout << "GET " << r.url << '\n';

        struct hostent *host;

        host = gethostbyname(r.url);
        if (!host)
        {
            socketClient.send("<html><h1>http error: 404</h1><h2> Ton url pue la merde</h2></html>", 67);
            return;
        }

        auto sock = DefaultSocket::DefaultSocket();

        int port = 80;
        sock.sin_port = hoston(port);
        sock.sin_family = AF_INET;
        sock.sin_addr.s_addr = host->h_addr;

        if (socketClient.connect((SOCKADDR*) &sock, sizeof(SockAddr)) != 0)
        {
            /*error code ?*/
		    std::cerr << "Could not connect";
		    return;
	    }

        std::cout << "connexion done!\n";
        std::string str;
        str << "GET / HTTP/1.1\r\nHost: " << r.url << " \r\nConnection: close\r\n\r\n";
        socketClient.send(str, strlen(str));
        std::cout << "GET done\n";
    }
}