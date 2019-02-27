#pragma once

#include "events.hh"
#include <socket/socket.hh>
#include "vhost/connection.hh"
#include <arpa/inet.h>
#include <request/response.hh>

namespace http
{
    /**
     * \class ListenerEW
     * \brief Workflow for listener socket.
     */
    class SendEv : public EventWatcher
    {
    public:
        /**
         * \brief Create a ListenerEW from a listener socket.
         */
        explicit SendEv(shared_socket socket, std::shared_ptr<Response> resp);

        void clean_send();

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
        ssize_t count_;
        std::string msg_;
        std::string path_;
        bool is_file_;
    };
} // namespace http
