#include "connection.hh"

namespace http
{

Dispatcher dispatcher;
int new_connexion(shared_socket sock)
{
    //Init Connection
    Connection cnx = Connection();
    cnx.sock_ = sock;

    //Getting request from client
    http::DefaultSocket s = http::DefaultSocket(sock->fd_get());
    cnx.req_ = fill_Request(s);
    return dispatcher.dispatch_request(cnx);
}
} // namespace http