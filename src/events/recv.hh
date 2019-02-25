/**
 * \file events/listener.hh
 * \brief ListenerEW declaration.
 */

#pragma once

#include <arpa/inet.h>
#include <socket/socket.hh>

#include "events.hh"
#include "vhost/connection.hh"

namespace http
{
    /**
     * \class ListenerEW
     * \brief Workflow for listener socket.
     */
    class RecvEv : public EventWatcher
    {
    public:
        /**
         * \brief Create a ListenerEW from a listener socket.
         */
        explicit RecvEv(shared_socket socket);

        /**
         * \brief Start accepting connections on listener socket.
         */
        void operator()() final;

    private:
        /**
         * \brief Listener socket.
         */
        shared_socket sock_;
        /**
         * \brief Port on which the socket is listening.
         */
        uint16_t port_;
    };
} // namespace http
