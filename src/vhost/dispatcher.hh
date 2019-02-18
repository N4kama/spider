/**
 * \file vhost/dispatcher.hh
 * \brief Dispatcher declaration.
 */

#pragma once

#include <iostream>
#include <vector>

#include "connection.hh"
#include "vhost-factory.hh"
#include "vhost-static-file.hh"

namespace http
{
    /**
     * \class Dispatcher
     * \brief Instance in charge of dispatching requests between vhosts.
     */
    class Dispatcher
    {
    public:
        Dispatcher() = default;
        Dispatcher(const Dispatcher&) = delete;
        Dispatcher& operator=(const Dispatcher&) = delete;
        Dispatcher(Dispatcher&&) = delete;
        Dispatcher& operator=(Dispatcher&&) = delete;

        void add_vhost(VHostConfig config);
        int dispatch_request(Connection &cnx);

        std::vector<shared_vhost> vhosts_;
    private:
        /* FIXME: Add members to store the information relative to the
        ** Dispatcher.
        */

       //Dispatcher is a global variable which stores the serveur config
    
       //Di
    };

    /**
     * \brief Service object.
     */
    extern Dispatcher dispatcher;
} // namespace http
