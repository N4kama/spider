#include "connection.hh"

namespace http
{
int new_connexion(shared_socket sock)
{
    //Init Connection
    Connection cnx = Connection();
    cnx.sock_ = sock;

    //Getting request from client
    cnx.req_ = fill_Request(*cnx.sock_);

    return dispatcher.dispatch_request(cnx);
}
} // namespace http