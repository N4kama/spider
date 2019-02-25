#pragma once
#include <boost/program_options.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "request/response.hh"
#include "request/request.hh"

#include "config/config.hh"
#include "socket/default-socket.hh"
#include "vhost/dispatcher.hh"


#include "events/callbacks.hh"
#include "events/events.hh"
#include "events/listener.hh"
#include "events/register.hh"

int dispatch(std::string arg, int debug);