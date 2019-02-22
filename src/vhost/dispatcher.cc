#include "dispatcher.hh"

namespace http
{
    void Dispatcher::add_vhost(VHostConfig config)
    {
        vhosts_.push_back(VHostFactory::Create(config));
    }

    int Dispatcher::dispatch_request(Connection &cnx)
    {
        //search to whom vhost the request is destined

        //Now we consider there is only one vhost
        auto vhost = (dispatcher.vhosts_[0]);
        vhost->respond(cnx.req_, cnx, 0, 0); //fix the remaining iter

        return 0;
    }
}