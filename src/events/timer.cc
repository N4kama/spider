#include "timer.hh"

#include <events/register.hh>
#include <events/sendv.hh>

namespace http
{
    TimerEW::TimerEW(shared_socket socket, shared_vhost vhost,
                     struct ev_loop* loop, TimeoutConfig t, int st)
        : sock_{socket}
        , vhost_{vhost}
        , loop_{loop}
        , toConf_{t}
        , state_{st}
    {
        if (state_ == 1)
            register_timer_watcher(&timeout_watcher_, toConf_.to_keep_alive_);
    }

    void TimerEW::timeout_ka_cb(struct ev_loop*, ev_timer* ev, int)
    {
        // ev_timer_stop(loop, ev);
        TimerEW* timer = reinterpret_cast<TimerEW*>(ev->data);
        shared_socket s = timer->sock_;
        shared_vhost v = timer->vhost_;
        std::shared_ptr<Response> r =
            std::make_shared<Response>(TIMEOUT, timer->state_);
        // s->killed_set(true);
        event_register.register_ew<http::SendEv, http::shared_socket,
                                   shared_vhost, std::shared_ptr<Response>>(
            std::forward<shared_socket>(s), std::forward<shared_vhost>(v),
            std::forward<std::shared_ptr<Response>>(r));
        // std::cout << "TIMEOUT\n";
    }

    void TimerEW::register_timer_watcher(ev_timer* timeout_watcher, double to)
    {
        ev_timer_init(timeout_watcher, TimerEW::timeout_ka_cb, to, 0);
        timeout_watcher->data = reinterpret_cast<void*>(this);
        ev_timer_start(loop_, timeout_watcher);
    }

    void TimerEW::unregister_timer_watcher()
    {
        ev_timer_stop(loop_, &timeout_watcher_);
    }

} // namespace http
