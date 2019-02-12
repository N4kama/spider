#include "parsing/json.hh"

int main(int argc, char *argv[])
{
    try
    {
        Config c = Config();
        nlohmann::json j = c.getJson(argv[1]);
        nlohmann::json j2 = c.getVHosts(j);
        c.parse_json(j2);
    }
    catch (const std::exception &e)
    {
        return 1;
    }
    return 0;
}
