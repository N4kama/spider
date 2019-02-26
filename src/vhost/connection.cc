#include "connection.hh"

#include <events/sendv.hh>

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
        cnx.rep_ = Response(cnx.req_);
        std::shared_ptr<http::SendEv> s =
            event_register
                .register_ew<http::SendEv, http::shared_socket, std::shared_ptr<Response>>(
                    std::make_shared<http::DefaultSocket>(sock->fd_get()),
                    std::make_shared<http::Response>(cnx.rep_));
        return 1;
        // return http::dispatcher.dispatch_request(cnx);
    }
} // namespace http