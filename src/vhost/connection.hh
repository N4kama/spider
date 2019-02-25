/**
 * \file vhost/connection.hh
 * \brief Connection declaration.
 */

#pragma once

#include <memory>
#include "../request/request.hh"
#include "../request/response.hh"

namespace http
{
    /**
     * \struct Connection
     * \brief Value object representing a connection.
     *
     * We need to keep track of the state of each request while it has not
     * been fully processed.
     */
    struct Connection
    {
        Connection() = default;
        Connection(const Connection&) = default;
        Connection& operator=(const Connection&) = default;
        Connection(Connection&&) = default;
        Connection& operator=(Connection&&) = default;
        ~Connection() = default;

        //Client socket
        shared_socket sock_;
        //Request from client
        Request req_;
        Response rep_;
    };

    int new_connexion(shared_socket sock);

} // namespace http
