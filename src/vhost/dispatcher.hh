/**
 * \file vhost/dispatcher.hh
 * \brief Dispatcher declaration.
 */

#pragma once

#include <iostream>
#include <vector>

#include "vhost.hh"

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
        int dispatch_request(shared_socket& s, TimeoutConfig toCon);
        shared_vhost get_vhost(int i)
        {
            return vhosts_[i];
        }

        size_t nb_of_vhost()
        {
            return vhosts_.size();
        }

        int check_integrity();

    private:
        /* FIXME: Add members to store the information relative to the
         ** Dispatcher.
         */
        std::vector<shared_vhost> vhosts_;

        // Dispatcher is a global variable which stores the server config
    };
    shared_vhost find_vhost(shared_socket s);

    /**
     * \brief Service object.
     */
} // namespace http
