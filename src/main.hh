#pragma once
#include <arpa/inet.h>
#include <boost/program_options.hpp>
#include <netdb.h>
#include <netinet/in.h>

#include "config/config.hh"
#include "events/callbacks.hh"
#include "events/events.hh"
#include "events/listener.hh"
#include "events/register.hh"
#include "request/request.hh"
#include "request/response.hh"
#include "socket/default-socket.hh"
#include "vhost/dispatcher.hh"

namespace http
{
    extern http::Dispatcher dispatcher;
    int start_server(std::string arg, int debug);
} // namespace http