#include "events/event-loop.hh"

using namespace http;

EventLoop::EventLoop()
    //:loop{EV_DEFAULT}
{
    ev_loop_new(0);
}

EventLoop::EventLoop(struct ev_loop* old_loop)
    :loop{old_loop}
{}

EventLoop::~EventLoop()
{
    ev_loop_destroy(loop);
}

void EventLoop::register_watcher(EventWatcher* evt)
{
    ev_io_start(loop, &evt->watcher_get());
}

void EventLoop::unregister_watcher(EventWatcher* evt)
{
    ev_io_stop(loop, &evt->watcher_get()); //one arg
    //ev_io_stop(watcher_get(evt));
}


static void sigint_cb (struct ev_loop *loop, ev_signal *w, int revents)
{
    revents = revents;
    w = w;
    ev_break(loop, EVBREAK_ALL);
}

void EventLoop::register_sigint_watcher(ev_signal* evt_sig) const
{
    ev_signal_init(evt_sig, sigint_cb, SIGINT);
    ev_signal_start(loop, evt_sig);
}

void EventLoop::operator()() const
{
    ev_run(loop, 0);
}