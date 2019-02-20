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
int dispatch(std::string arg, int debug);