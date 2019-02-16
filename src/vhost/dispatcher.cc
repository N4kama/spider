#include "dispatcher.hh"

namespace http
{

    void Dispatcher::add_vhost(const VHostConfig &vh_config)
    {
        vhosts_.push_back(VHostFactory::Create(vh_config));
    }

    void Dispatcher::dispatch_request(const Request &request), Connection &cnx)
    {
        //search to whom vhost the request is destined

        //Now we consider there is only one vhost
        VHostStaticFile &vhost = *(vhosts_[0]);
        vhost.respond(request, cnx, nullptr, nullptr); //fix the remaining iter
    }

}