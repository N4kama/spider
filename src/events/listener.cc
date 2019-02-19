#include "events/listener.hh"
using namespace http;

ListenerEW::ListenerEW(shared_socket socket)
:sock_(socket) // {}
{
    struct sockaddr_in my_addr;
    socklen_t len = sizeof(my_addr);
    getsockname(socket.fd_get(), (struct sockaddr *) &my_addr, &len);
    this->port_ = ntohs(my_addr.sin_port);
}

void ListenerEW::operator()()
{

}