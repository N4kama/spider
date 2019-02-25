#pragma once

#include "events/events.hh"
#include "socket/socket.hh"

namespace http
{
    class Sendresp : public EventWatcher
    {
    public:
        explicit Sendresp(shared_socket socket);
        void operator()() final;
    private:
        shared_socket sock_;
        uint16_t port_;
        std::string recved_;
        std::string body_;
    };
} // namespace http
