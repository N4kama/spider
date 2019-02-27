#include "dispatcher.hh"

#include "../main.hh"
#include "vhost-factory.hh"

namespace http
{
    void Dispatcher::add_vhost(VHostConfig config)
    {
        vhosts_.push_back(VHostFactory::Create(config));
    }

    int Dispatcher::dispatch_request(Connection& cnx)
    {
        // Now we consider there is only one vhost
        try
        {
            // search to whom vhost the request is destined
            auto vhost = (http::dispatcher.vhosts_[0]);
            cnx.req_.config_ptr = std::make_shared<VHostConfig>(vhost->get_conf());
            vhost->respond(cnx.req_, cnx, 0, 0);
        } catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        return 0;
    }
} // namespace http