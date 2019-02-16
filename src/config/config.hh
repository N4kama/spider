/**
 * \file config/config.hh
 * \brief Declaration of ServerConfig and VHostConfig.
 */

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-enum"
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Winline"
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wlogical-op-parentheses"
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
     VHostConfig(const std::string &ip, const int port,
                 const std::string &server_name, const std::string &root,
                 const std::string &default_file = "index.html");
     //VHostConfig() = default;
     VHostConfig(const VHostConfig &) = default;
     VHostConfig &operator=(const VHostConfig &) = default;
     VHostConfig(VHostConfig &&) = default;
     VHostConfig &operator=(VHostConfig &&) = default;

     ~VHostConfig() = default;
     void print_VHostConfig(void);

     const std::string ip_;
     const int port_;
     const std::string server_name_;
     const std::string root_;
     const std::string default_file_;
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
     ServerConfig(const ServerConfig &) = default;
     ServerConfig &operator=(const ServerConfig &) = default;
     ServerConfig(ServerConfig &&) = default;
     ServerConfig &operator=(ServerConfig &&) = default;

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
int check_vhost(nlohmann::json j);
json get_json(const std::string &s);
nlohmann::json get_vhosts(nlohmann::json j);
struct ServerConfig parse_configuration(const std::string &path, int debug);
} // namespace http
