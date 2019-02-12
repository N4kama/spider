/**
 * \file request/request.hh
 * \brief Request declaration.
 */

#pragma once

#include <vector>
#include "request/types.hh"

namespace http
{
    using header = std::pair<std::string, std::string>;

    enum Method : uint16_t
    {
        GET,
        HEAD,
        POST
    };
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

        Method method;
        std::string url;
        std::string http_version;
        std::vector<header> headers;
        std::string message_body;
    };
} // namespace http
