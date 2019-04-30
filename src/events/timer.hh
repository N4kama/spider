/**
 * \file events/timer.hh
 * \brief TimerEW declaration.
 */

#pragma once

#include "config/config.hh"
#include "events/events.hh"
#include "socket/socket.hh"
#include "vhost/vhost.hh"

namespace http
{
    /**
     * \class TimerEW
     * \brief Workflow for timeout socket.
     */
    class TimerEW
    {
    public:
        /**
         * \brief Create a TimerEW from a listener socket.
         */
        TimerEW(shared_socket socket, shared_vhost vhost, struct ev_loop* loop,
                TimeoutConfig toConf, int st);

        void register_timer_watcher(ev_timer* timeout_watcher, double to);

        static void timeout_ka_cb(struct ev_loop* loop, ev_timer*, int);
        void rep_error(STATUS_CODE);
        void unregister_timer_watcher();

        /**
         * \brief Start timer for each connection.
         */

    private:
        shared_socket sock_;
        shared_vhost vhost_;
        struct ev_loop* loop_;
        TimeoutConfig toConf_;
        int state_;
        ev_timer timeout_watcher_;
    };
} // namespace http
