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
        Response(const struct Request&, const STATUS_CODE& = STATUS_CODE::OK);

        Response() = default;
        Response(const Response&) = default;
        Response& operator=(const Response&) = default;
        Response(Response&&) = default;
        Response& operator=(Response&&) = default;
        ~Response() = default;

        STATUS_CODE status_code;
        int variable_juste_pour_test;
    };

    void parse_uri(std::string uri, std::string& servN, std::string& fileP);
    void http_get(struct Request r, Socket& socketClient);
    void http_head(struct Request r, Socket& socketClient);
    void http_post(struct Request r, Socket& socketClient);
} // namespace http
