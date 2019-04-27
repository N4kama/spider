#pragma once

#include <arpa/inet.h>
#include <request/response.hh>
#include <socket/socket.hh>

#include "events.hh"
#include "vhost/connection.hh"
#include "vhost/vhost.hh"

namespace http
{
    /**
     * \class ListenerEW
     * \brief Workflow for listener socket.
     */
    class SendRequest : public EventWatcher
    {
    public:
        /**
         * \brief Create a ListenerEW from a listener socket.
         */
        SendRequest(const Request &r, shared_socket sock);

        void clean_send();

        /**
         * \brief Start accepting connections on listener socket.
         */
        void operator()() final;

        void init_msg();

    private:

        shared_socket sock_;
        Request r_;
        std::string msg_;
        int count_;
    };
} // namespace http
