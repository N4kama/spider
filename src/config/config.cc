#include "config/config.hh"

#include <list>
using json = nlohmann::json;
http::TimeoutConfig toConf;

namespace http
{
    VHostConfig::VHostConfig(
        std::string& ip, int port, std::string& server_name, std::string& root,
        std::string& ssl_cert, std::string& ssl_key, size_t header_max_size,
        size_t uri_max_size, size_t payload_max_size, std::string& default_file,
        bool auto_index, std::string& proxy_ip, std::string& proxy_port,
        std::map<std::string, std::string>& proxy_set_header,
        std::vector<std::string>& proxy_remove_header,
        std::map<std::string, std::string>& set_header,
        std::vector<std::string>& remove_header, std::string& health_endpoint,
        std::string& auth_basic, std::vector<std::string>& auth_basic_users,
        bool& default_vhost, bool& is_proxy)
        : ip_(ip)
        , port_(port)
        , server_name_(server_name)
        , root_(root)
        , ssl_cert_(ssl_cert)
        , ssl_key_(ssl_key)
        , header_max_size_(header_max_size)
        , uri_max_size_(uri_max_size)
        , payload_max_size_(payload_max_size)
        , default_file_(default_file)
        , auto_index_(auto_index)
        , proxy_ip_(proxy_ip)
        , proxy_port_(proxy_port)
        , proxy_set_header_(proxy_set_header)
        , proxy_remove_header_(proxy_remove_header)
        , set_header_(set_header)
        , remove_header_(remove_header)
        , health_endpoint_(health_endpoint)
        , auth_basic_(auth_basic)
        , auth_basic_users_(auth_basic_users)
        , default_vhost_(default_vhost)
        , is_proxy_(is_proxy)
    {
        if (ssl_cert == "" || ssl_key == "")
            no_ssl = 1;
    }

    TimeoutConfig::TimeoutConfig(float to_ka, float to_tran, float to_thr_val,
                                 float to_thr_time)
        : to_keep_alive_(to_ka)
        , to_transaction_(to_tran)
        , to_throughput_val_(to_thr_val)
        , to_throughput_time_(to_thr_time)
    {}

    void VHostConfig::print_VHostConfig(void)
    {
        std::cout << "IP:\t\t" << ip_ << "\n"
                  << "PORT:\t\t" << port_ << "\n"
                  << "SERVER NAME:\t" << server_name_ << "\n"
                  << "ROOT: \t\t" << root_ << "\n"
                  << "DEFAULT FILE:\t" << default_file_ << '\n'
                  << "HEADER MAX SIZE:\t" << header_max_size_ << '\n';
    }

    json get_vhosts(const std::string& s)
    {
        json j;
        try
        {
            std::ifstream ifs(s);
            if (!ifs.is_open())
                return nullptr;
            j = json::parse(ifs);
        } catch (const std::exception& e)
        {
            std::cerr << "bad json\n";
        }
        try
        {
            auto& arr = j.at("vhosts");
            return arr;
        } catch (const std::exception& e)
        {
            std::cerr << "Invalid json structure: no vhosts" << '\n';
        }
        return nullptr;
    }

    json get_timeouts(const std::string& s)
    {
        json j;
        try
        {
            std::ifstream ifs(s);
            if (!ifs.is_open())
                return nullptr;
            j = json::parse(ifs);
        } catch (const std::exception& e)
        {
            std::cerr << "bad json\n";
        }
        try
        {
            auto& obj = j.at("timeout");
            return obj;
        } catch (const std::exception& e)
        {
            return nullptr;
        }
    }

    int check_vhost(json j)
    {
        if (j.find("ip") == j.end() || !j.find("ip")->is_string())
        {
            std::cerr << "Invalid json structure: missing ip" << '\n';
            return 0;
        }
        if (j.find("port") == j.end() || !j.find("port")->is_number())
        {
            std::cerr << "Invalid json structure: missing port" << '\n';
            return 0;
        }
        if (j.find("server_name") == j.end()
            || !j.find("server_name")->is_string())
        {
            std::cerr << "Invalid json structure: missing server_name" << '\n';
            return 0;
        }
        /*
        if (j.find("root") == j.end() || !j.find("root")->is_string())
        {
            std::cerr << "Invalid json structure: missing root" << '\n';
            return 0;
        }
        */
        return 1;
    }

    struct TimeoutConfig set_TimeoutConfig(const std::string& str)
    {
        json j = get_timeouts(str.c_str());
        if (j != nullptr)
        {
            double to_keep_alive;
            try
            {
                to_keep_alive = j.at("keep_alive").get<double>();
            } catch (const std::exception& e)
            {
                to_keep_alive = 3600;
            }

            double to_transaction;
            try
            {
                to_transaction = j.at("transaction").get<double>();
            } catch (const std::exception& e)
            {
                to_transaction = 3600;
            }

            double to_throughput_val;
            try
            {
                to_throughput_val = j.at("throughput_val").get<double>();
            } catch (const std::exception& e)
            {
                to_throughput_val = 3600;
            }

            double to_throughput_time;
            try
            {
                to_throughput_time = j.at("throughput_time").get<double>();
            } catch (const std::exception& e)
            {
                to_throughput_time = 3600;
            }

            return TimeoutConfig(to_keep_alive, to_transaction,
                                 to_throughput_val, to_throughput_time);
        }
        return TimeoutConfig(3600, 3600, 3600, 3600);
    }

    struct ServerConfig parse_configuration(const std::string& path)
    {
        ServerConfig c = ServerConfig();
        json j = get_vhosts(path.c_str());
        for (auto vhost = j.begin(); vhost != j.end(); vhost++)
        {
            json cur = *vhost;
            if (check_vhost(cur))
            {
                std::string ip_s = cur.at("ip").get<std::string>();
                if (ip_s == "")
                {
                    std::cerr << "ip is empty\n";
                    throw std::exception();
                }
                int port_i = cur.at("port").get<int>();
                std::string serv_s = cur.at("server_name").get<std::string>();
                if (serv_s == "")
                {
                    std::cerr << "server name is empty\n";
                    throw std::exception();
                }

                std::string root_s;
                try
                {
                    root_s = cur.at("root").get<std::string>();
                } catch (const std::exception& e)
                {
                    root_s = "";
                }

                size_t header_max_size_i;
                try
                {
                    header_max_size_i = cur.at("header_max_size").get<size_t>();
                } catch (const std::exception& e)
                {
                    header_max_size_i = 0;
                }
                size_t uri_max_size_i;
                try
                {
                    uri_max_size_i = cur.at("uri_max_size").get<size_t>();
                } catch (const std::exception& e)
                {
                    uri_max_size_i = 0;
                }
                std::string ssl_cert_i;
                try
                {
                    ssl_cert_i = cur.at("ssl_cert").get<std::string>();
                } catch (const std::exception& e)
                {
                    ssl_cert_i = "";
                }
                std::string ssl_key_i;
                try
                {
                    ssl_key_i = cur.at("ssl_key").get<std::string>();
                } catch (const std::exception& e)
                {
                    ssl_key_i = "";
                }

                if ((ssl_cert_i == "" && ssl_key_i != "")
                    || (ssl_cert_i != "" && ssl_key_i == ""))
                {
                    std::cerr << "Invalid json" << '\n';
                    throw std::exception();
                }

                size_t payload_max_size_i;
                try
                {
                    payload_max_size_i =
                        cur.at("payload_max_size").get<size_t>();
                } catch (const std::exception& e)
                {
                    payload_max_size_i = 0;
                }

                std::string def_s;
                try
                {
                    def_s = cur.at("default_file").get<std::string>();
                    if (def_s == "")
                    {
                        def_s = std::string("index.html");
                    }
                } catch (const std::exception& e)
                {
                    def_s = std::string("index.html");
                }

                bool auto_index_i;
                try
                {
                    auto_index_i = cur.at("auto_index").get<bool>();
                } catch (const std::exception& e)
                {
                    auto_index_i = false;
                }

                // proxy values
                bool is_proxy_ = false;
                std::string proxy_ip_;
                std::string proxy_port_;
                std::map<std::string, std::string> proxy_set_header_;
                std::vector<std::string> proxy_remove_header_;
                std::map<std::string, std::string> set_header_;
                std::vector<std::string> remove_header_;
                // Check mandatory options for proxy
                if (cur.find("proxy_pass") != cur.end())
                {
                    if (cur["proxy_pass"].find("ip") == cur["proxy_pass"].end()
                        || cur["proxy_pass"].find("port")
                            == cur["proxy_pass"].end())
                    {
                        std::cerr
                            << "Error during json parsing : "
                            << "Proxy field : \'ip\' or \'port\' missing\n";
                    } else
                    {
                        is_proxy_ = true;
                        try
                        {
                            proxy_ip_ = cur.at("proxy_pass")
                                            .at("ip")
                                            .get<std::string>();
                        } catch (const std::exception& e)
                        {
                            proxy_ip_ = "";
                        }
                        try
                        {
                            proxy_port_ =
                                cur.at("proxy_pass").at("port").get<int>();
                        } catch (const std::exception& e)
                        {
                            proxy_port_ = -1;
                        }
                        try
                        {
                            auto map =
                                cur.at("proxy_pass").at("proxy_set_header");
                            for (auto it = map.begin(); it != map.end(); it++)
                            {
                                proxy_set_header_.emplace(it.key(), it.value());
                            }
                        } catch (const std::exception& e)
                        {}
                        try
                        {
                            proxy_remove_header_ =
                                cur.at("proxy_pass")
                                    .at("proxy_remove_header")
                                    .get<std::vector<std::string>>();
                        } catch (const std::exception& e)
                        {}
                        try
                        {
                            auto map = cur.at("proxy_pass").at("set_header");
                            for (auto it = map.begin(); it != map.end(); it++)
                            {
                                set_header_.emplace(it.key(), it.value());
                            }
                        } catch (const std::exception& e)
                        {}
                        try
                        {
                            remove_header_ =
                                cur.at("proxy_pass")
                                    .at("remove_header")
                                    .get<std::vector<std::string>>();
                        } catch (const std::exception& e)
                        {}
                    }
                }

                std::string health_endpoint_;
                std::string auth_basic_;
                std::vector<std::string> auth_basic_users_;
                bool default_vhost_;

                VHostConfig v = VHostConfig(
                    ip_s, port_i, serv_s, root_s, ssl_cert_i, ssl_key_i,
                    header_max_size_i, uri_max_size_i, payload_max_size_i,
                    def_s, auto_index_i, proxy_ip_, proxy_port_,
                    proxy_set_header_, proxy_remove_header_, set_header_,
                    remove_header_, health_endpoint_, auth_basic_,
                    auth_basic_users_, default_vhost_, is_proxy_);
                c.vhosts_.emplace_back(v);
            } else
            {
                std::cerr << "Invalid json" << '\n';
                throw std::exception();
            }
        }

        toConf = set_TimeoutConfig(path);

        if (c.vhosts_.size() == 0)
        {
            std::cerr << "Invalid json" << '\n';
            throw std::exception();
        }
        return c;
    }
} // namespace http
