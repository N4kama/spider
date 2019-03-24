/**
 * \file config/config.hh
 * \brief Declaration of ServerConfig and VHostConfig.
 */

#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Winline"
#ifdef __clang__
#    pragma GCC diagnostic ignored "-Wlogical-op-parentheses"
#endif
#include <json.hpp>
#pragma GCC diagnostic pop

using json = nlohmann::json;

namespace http
{
    /**
     * \struct VHostConfig
     * \brief Value object storing a virtual host configuration.
     *
     * Since each virtual host of the server has its own configuration, a
     * dedicated structure is required to store the information related to
     * each one of them.
     */
    struct VHostConfig
    {
        VHostConfig(std::string& ip, int port, std::string& server_name,
                    std::string& root, std::string& ssl_cert,
                    std::string& ssl_key, size_t header_max_size,
                    size_t uri_max_size, size_t payload_max_size,
                    std::string& default_file);
        VHostConfig() = default;
        VHostConfig(const VHostConfig&) = default;
        VHostConfig& operator=(const VHostConfig&) = default;
        VHostConfig(VHostConfig&&) = default;
        VHostConfig& operator=(VHostConfig&&) = default;

        ~VHostConfig() = default;

        void print_VHostConfig(void);

        std::string ip_;
        int port_ = 80;
        std::string server_name_;
        std::string root_;
        std::string ssl_cert_;
        std::string ssl_key_;
        size_t header_max_size_ = 0;
        size_t uri_max_size_ = 0;
        size_t payload_max_size_ = 0;
        std::string default_file_;


        int no_ssl = 0;
    };

    /**
     * \struct ServerConfig
     * \brief Value object storing the server configuration.
     *
     * To avoid opening the configuration file each time we need to access the
     * server configuration, a dedicated structure is required to store it.
     */
    struct ServerConfig
    {
        ServerConfig() = default;
        ServerConfig(const ServerConfig&) = default;
        ServerConfig& operator=(const ServerConfig&) = default;
        ServerConfig(ServerConfig&&) = default;
        ServerConfig& operator=(ServerConfig&&) = default;

        ~ServerConfig() = default;

        std::vector<http::VHostConfig> vhosts_;
    };

    /**
     * \brief Parse the server configuration file.
     *
     * \param path string containing the path to the server configuration
     * file.
     * \return The server configuration.
     */
    json get_vhosts(const std::string& s);
    int check_vhost(json j);
    struct ServerConfig parse_configuration(const std::string& path);
} // namespace http
