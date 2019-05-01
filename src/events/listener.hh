/**
 * \file events/listener.hh
 * \brief ListenerEW declaration.
 */

#pragma once

#include <arpa/inet.h>

#include "events/events.hh"
#include "events/event-loop.hh"
#include "socket/socket.hh"
#include "vhost/connection.hh"
namespace http
{
    /**
     * \class ListenerEW
     * \brief Workflow for listener socket.
     */
    class ListenerEW : public EventWatcher
    {
    public:
        /**
         * \brief Create a ListenerEW from a listener socket.
         */
        explicit ListenerEW(shared_socket socket);

        /**
         * \brief Start accepting connections on listener socket.
         */
        void operator()() final;

        shared_socket socket()
        {
            return sock_;
        }

        std::shared_ptr<ev_child> child()
        {
            return child_;
        }

        struct ev_loop* loop()
        {
            return loop_.loop;
        }

    private:
        /**
         * \brief Listener socket.
         */
        shared_socket sock_;
        std::shared_ptr<ev_child> child_;
        EventLoop loop_;
        /**
         * \brief Port on which the socket is listening.
         */
        uint16_t port_;
    };
} // namespace http
