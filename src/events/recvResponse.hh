/**
 * \file events/listener.hh
 * \brief ListenerEW declaration.
 */

#pragma once

#include <arpa/inet.h>
#include <socket/socket.hh>

#include "events.hh"
#include "vhost/connection.hh"
#include "vhost/vhost-static-file.hh"

namespace http
{
    /**
     * \class ListenerEW
     * \brief Workflow for listener socket.
     */
    class RecvResponse : public EventWatcher
    {
    public:
        /**
         * \brief Create a ListenerEW from a listener socket.
         */
        explicit RecvResponse(shared_socket socket, Request r);

        /**
         * \brief Start accepting connections on listener socket.
         */
        void operator()() final;
        static int read_int(std::string s, size_t pos);
        static int endby(std::string const& str, std::string const& end);

    private:
        /**
         * \brief Listener socket.
         */
        shared_socket sock_;
        /**
         * \brief Port on which the socket is listening.
         */
        uint16_t port_;

        Request r_;

        shared_vhost vhost_;
        std::string header;
        std::string body;

        size_t filled;
    };

} // namespace http
