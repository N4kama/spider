/**
 * \file request/response.hh
 * \brief Response declaration.
 */

#pragma once

#include <request/request.hh>
#include <request/types.hh>

namespace http
{
    /**
     * \struct Response
     * \brief Value object representing a response.
     */
    struct Response
    {
        explicit Response(const STATUS_CODE&);
        Response(const Request&, const STATUS_CODE& = STATUS_CODE::OK);

        Response() = default;
        Response(const Response&) = default;
        Response& operator=(const Response&) = default;
        Response(Response&&) = default;
        Response& operator=(Response&&) = default;
        ~Response() = default;
        // FIXME: Add members to store the information relative to a response.
    };

    void http_get(struct Request r, DefaultSocket socketClient);
    void http_head(struct Request r, DefaultSocket socketClient);
    void http_post(struct Request r, DefaultSocket socketClient);
} // namespace http
