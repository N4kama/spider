#include "../../src/request/request.hh"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

 int main()
 {
     char *localip;
     hostent* localHost;
     struct sockaddr_in adr;
     auto sock = http::DefaultSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
 
     localHost = gethostbyname("hello.html");
     localip = inet_ntoa(*(struct in_addr *) *localHost->h_addr_list);
 
     adr.sin_family = AF_INET;
     adr.sin_addr.s_addr = inet_addr(localip);
     adr.sin_port = htons(1234);
 
     sock.bind((sockaddr *) &adr, sizeof(adr));
 
 
     auto r = http::Request();
     r.method = "get";
     r.status = 100;
     r.url = "hello.html";
 
     http_get(r, sock, 1234);
 }
