#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "../../lib/json.hpp"


class Vhost
{
  public:
	Vhost(const std::string &ip, const int port,
		  const std::string &server_name, const std::string &root,
		  const std::string &default_file = "index.thml");
	void print(void);

  private:
	const std::string ip_;
	const int port_;
	const std::string server_name_;
	const std::string root_;
	const std::string default_file_;
};

class Config
{
  public:
	void print(void);
	void parse_json(nlohmann::json j);
	nlohmann::json getJson(const std::string &s);
	nlohmann::json getVHosts(nlohmann::json j);

  private:
	std::vector<Vhost> vhosts_;
};