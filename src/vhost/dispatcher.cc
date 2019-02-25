#include "dispatcher.hh"
#include "vhost-factory.hh"
#include "../main.hh"

namespace http
{
    void Dispatcher::add_vhost(VHostConfig config)
    {
        vhosts_.push_back(VHostFactory::Create(config));
    }

    int Dispatcher::dispatch_request(Connection& cnx)
    {
        // search to whom vhost the request is destined

        // Now we consider there is only one vhost
        try
        {
            auto vhost = (http::dispatcher.vhosts_[0]);
            vhost->respond(cnx.req_, cnx, 0, 0);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        return 0;
    }
} // namespace http