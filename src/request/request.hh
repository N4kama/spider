/**
 * \file request/request.hh
 * \brief Request declaration.
 */

#pragma once

#include <vector>
#include <socket/default-socket.hh>
#include "request/types.hh"

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
        Request(const Request&) = default;
        Request& operator=(const Request&) = default;
        Request(Request&&) = default;
        Request& operator=(Request&&) = default;
        ~Request() = default;
        // FIXME: Add members to store the information relative to a request.

        std::string method;
        int status;
        std::string url;
        std::string http_version;
        std::vector<header> headers;
        std::string message_body;
    };

    void fill_Request(DefaultSocket sock);

    void http_get(struct Request r, DefaultSocket socketClient);
    void http_head(struct Request r, DefaultSocket socketClient);
    void http_post(struct Request r, DefaultSocket socketClient);

} // namespace http
