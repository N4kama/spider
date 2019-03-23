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

        void http_rhead(struct Request r);
        void http_rget(struct Request r);
        void http_rpost(struct Request r);
        void set_error_rep(const struct Request& r, const STATUS_CODE& s);
        void set_rep_heads(const struct Request& r);

        STATUS_CODE status_code;
        bool is_file;
        std::string rep;
        std::string file_p;
        bool keep_alive;
    };
    std::string get_date(void);
} // namespace http
