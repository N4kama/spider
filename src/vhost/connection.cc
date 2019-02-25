#include "connection.hh"
#include "dispatcher.hh"
#include "../main.hh"

namespace http
{
    int new_connexion(shared_socket sock)
    {
        // Init Connection
        Connection cnx = Connection();
        cnx.sock_ = sock;
        // Getting request from client
        cnx.req_ = fill_Request(*sock);
        return http::dispatcher.dispatch_request(cnx);
    }
} // namespace http