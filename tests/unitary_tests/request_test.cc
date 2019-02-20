#include <request/request.hh>
#include <request/response.hh>
#include <misc/fd.hh>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main()
{
    try
    {
        struct sockaddr_in adr;
    //char *localip;
        hostent *localHost;

        localHost = gethostbyname("localhost");
    //localip = inet_ntoa(*(struct in_addr *)*localHost->h_addr_list);
        adr.sin_family = AF_INET;
        adr.sin_addr.s_addr =  *((unsigned long*)localHost->h_addr);
        adr.sin_port = htons(2200);

        http::DefaultSocket sockserv = http::DefaultSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockserv.bind((struct sockaddr *)&adr, sizeof(adr));
        sockserv.listen(20);


        struct sockaddr_in adr_c;
        socklen_t clientaddrsize;


        http::shared_socket tmp = sockserv.accept((struct sockaddr *)&adr_c, &clientaddrsize);
        http::DefaultSocket sock = http::DefaultSocket(tmp->fd_get());
        std::cout << "ici\n";

        //tmp->~Socket();

        auto r = http::Request();
        r.method = "get";
        r.status = 100;
        r.url = "localhost";

        http_head(r, sock);

        ssize_t nDataLength = 0;
        char buffer[10000];
        while (1)
        {
            while ((nDataLength = sock.recv(buffer,10000)) > 0)
            {
                std::cout << buffer;
            }
        }
    /*
    while ((nDataLength = sockserv.recv(buffer,10000)) > 0)
    {
        std::cout << "la\n";
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r')
        {
            std::cout << buffer;
            i += 1;
        }
    }*/
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 1;
}
