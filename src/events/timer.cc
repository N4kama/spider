#include "timer.hh"

#include <config/config.hh>
#include <events/register.hh>
#include <events/sendv.hh>

namespace http
{
    TimerEW::TimerEW(shared_socket socket, shared_vhost vhost,
                     struct ev_loop* loop, int st)
        : sock_{socket}
        , vhost_{vhost}
        , loop_{loop}
        , state_{st}
    {
        timeout_watcher_ = std::make_shared<ev_timer>();
        if (state_ == 1)
            register_timer_watcher(timeout_watcher_.get(),
                                   toConf.to_keep_alive_);
        else if (state_ == 2)
            register_timer_watcher(timeout_watcher_.get(),
                                   toConf.to_transaction_);
        else if (state_ == 3)
            register_timer_watcher(timeout_watcher_.get(),
                               toConf.to_throughput_time_);
    }

    void TimerEW::timeout_ka_cb(struct ev_loop*, ev_timer* ev, int)
    {
        TimerEW* timer = reinterpret_cast<TimerEW*>(ev->data);
        shared_socket s = timer->sock_;
        shared_vhost v = timer->vhost_;
        
        if (timer->state_ == 1 || timer->state_ == 2)
        {
            std::shared_ptr<Response> r =
                std::make_shared<Response>(TIMEOUT, timer->state_);
            s->killed_set(true);
            event_register.register_ew<http::SendEv, http::shared_socket,
                                       shared_vhost, std::shared_ptr<Response>>(
                std::forward<shared_socket>(s), std::forward<shared_vhost>(v),
                std::forward<std::shared_ptr<Response>>(r));
        } else if (timer->state_ == 3)
        {
            if ((s->get_recv_data() - timer->prev_data_) < toConf.to_throughput_val_)
            {
                std::shared_ptr<Response> r =
                    std::make_shared<Response>(TIMEOUT, timer->state_);
                s->killed_set(true);
                event_register
                    .register_ew<http::SendEv, http::shared_socket,
                                 shared_vhost, std::shared_ptr<Response>>(
                        std::forward<shared_socket>(s),
                        std::forward<shared_vhost>(v),
                        std::forward<std::shared_ptr<Response>>(r));
            }
            else
            {
                timer->prev_data_ = s->get_recv_data();
                timer->reset_timer_watcher(toConf.to_throughput_time_);
            }
        }
    }

    void TimerEW::register_timer_watcher(ev_timer* timeout_watcher, double to)
    {
        ev_timer_init(timeout_watcher, TimerEW::timeout_ka_cb, to, 0.);
        timeout_watcher->data = reinterpret_cast<void*>(this);
        ev_timer_start(loop_, timeout_watcher);
    }

    void TimerEW::reset_timer_watcher(double to)
    {
        ev_timer_stop(loop_, timeout_watcher_.get());
        ev_timer_set(timeout_watcher_.get(), to, 0.);
        ev_timer_start(loop_, timeout_watcher_.get());
    }

    void TimerEW::restart_timer_watcher(double to)
    {
        ev_timer_set(timeout_watcher_.get(), to, 0.);
        ev_timer_start(loop_, timeout_watcher_.get());
    }

    void TimerEW::unregister_timer_watcher()
    {
        ev_timer_stop(loop_, timeout_watcher_.get());
    }

} // namespace http
