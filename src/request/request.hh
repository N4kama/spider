/**
 * \file request/request.hh
 * \brief Request declaration.
 */

#pragma once

#include <vector>
#include "../socket/default-socket.hh"
#include "types.hh"
#include "../config/config.hh"

namespace http
{
using header = std::pair<std::string, std::string>;

/**
     * \struct Request
     * \brief Value object representing a request.
     */
struct Request
{
    Request() = default;
    Request(const Request &) = default;
    Request &operator=(const Request &) = default;
    Request(Request &&) = default;
    Request &operator=(Request &&) = default;
    ~Request() = default;
    // FIXME: Add members to store the information relative to a request.

    int port;
    std::string method;
    int status;
    std::string http_version;
    std::vector<header> headers;
    std::string message_body;
    std::string uri;
};

std::string recvLine(DefaultSocket &sock);
Request fill_Request(DefaultSocket &sock);
void request_server(struct Request r, DefaultSocket socketClient);

} // namespace httpy