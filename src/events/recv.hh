/**
 * \file events/listener.hh
 * \brief ListenerEW declaration.
 */

#pragma once

#include <arpa/inet.h>
#include <events/timer.hh>
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
    class RecvEv : public EventWatcher
    {
    public:
        /**
         * \brief Create a ListenerEW from a listener socket.
         */
        explicit RecvEv(shared_socket socket, shared_vhost vhost,
                        std::shared_ptr<TimerEW> timer);

        /**
         * \brief Start accepting connections on listener socket.
         */
        void operator()() final;
        void sendit();

    private:
        /**
         * \brief Listener socket.
         */
        shared_socket sock_;
        /**
         * \brief Port on which the socket is listening.
         */
        uint16_t port_;

        shared_vhost vhost_;
        std::string header;
        std::string body;
        std::shared_ptr<TimerEW> ka_timer_;
        std::shared_ptr<TimerEW> transaction_timer_;
        size_t filled;
    };
    int read_int(std::string s, size_t pos);
    int endby(std::string const& str, std::string const& end);

} // namespace http
