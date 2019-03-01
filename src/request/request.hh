/**
 * \file request/request.hh
 * \brief Request declaration.
 */

#pragma once

#include <vector>
#include <map>

#include "../config/config.hh"
#include "../socket/default-socket.hh"
#include "types.hh"

namespace http
{
    /**
     * \struct Request
     * \brief Value object representing a request.
     */
    struct Request
    {
        Request(std::string &s);
        Request() = default;
        Request(const Request&) = default;
        Request& operator=(const Request&) = default;
        Request(Request&&) = default;
        Request& operator=(Request&&) = default;
        ~Request() = default;
        // FIXME: Add members to store the information relative to a request.

        void get_path();

        std::string method;
        int status;
        std::string http_version;
        std::map<std::string, std::string> headers;
        std::string message_body;
        std::string uri;

        std::shared_ptr<VHostConfig> config_ptr;
        std::pair<std::string, int> path_info;
    };
    void request_server(struct Request r, Socket& socketClient);

} // namespace http