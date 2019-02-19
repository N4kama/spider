#include <request/request.hh>
#include <request/response.hh>
#include <misc/fd.hh>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int main()
{
    struct sockaddr_in adr;
    //char *localip;
    hostent *localHost;

    localHost = gethostbyname("localhost");
    //localip = inet_ntoa(*(struct in_addr *)*localHost->h_addr_list);
    adr.sin_family = AF_INET;
    adr.sin_addr.s_addr =  *((unsigned long*)localHost->h_addr);
    adr.sin_port = htons(1234);

    auto sockserv = http::DefaultSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockserv.bind((struct sockaddr *)&adr, sizeof(struct sockaddr));
    sockserv.listen(5);


    struct sockaddr_in adr_c;
    socklen_t clientaddrsize;


    http::shared_socket tmp = sockserv.accept((struct sockaddr *)&adr_c, &clientaddrsize);
    http::DefaultSocket sock = http::DefaultSocket(tmp->fd_get());

    auto r = http::Request();
    r.method = "get";
    r.status = 100;
    r.url = "localhost";

    http_get(r, sock);

    ssize_t nDataLength = 0;
    char buffer[10000];
    while ((nDataLength = sock.recv(buffer,10000)) > 0)
    {
        std::cout << "la\n";
        /*
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r')
        {
            std::cout << buffer;
            i += 1;
        }*/
    }
    while ((nDataLength = sockserv.recv(buffer,10000)) > 0)
    {
        std::cout << "la\n";
        /*
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r')
        {
            std::cout << buffer;
            i += 1;
        }*/
    }
    return 1;
}