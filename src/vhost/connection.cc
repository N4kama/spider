#include "connection.hh"

#include "../main.hh"
#include "dispatcher.hh"

namespace http
{
    Connection::Connection(shared_socket sock)
        : sock_{sock}
    {}

    int new_connexion(shared_socket sock)
    {
        // Init Connection
        Connection cnx = Connection(sock);
        // Getting request from client
        cnx.req_ = fill_Request(*sock);
        return 1;
        //return http::dispatcher.dispatch_request(cnx);
    }
} // namespace http