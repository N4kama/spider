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
        shared_vhost vhost =
            std::make_shared<VHostStaticFile>(cnx.sock_->get_vhost());
        cnx.req_.config_ptr = std::make_shared<VHostConfig>(vhost->get_conf());
        cnx.req_.get_path();
        vhost->respond(cnx.req_, cnx, 0, 0);
        return 0;
    }
} // namespace http
