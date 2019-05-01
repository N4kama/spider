#include "events/event-loop.hh"

using namespace http;

EventLoop::EventLoop()
    //:loop{EV_DEFAULT}
    : loop(ev_default_loop(0))
{}

EventLoop::EventLoop(struct ev_loop* old_loop)
    : loop{old_loop}
{}

EventLoop::~EventLoop()
{
    ev_loop_destroy(EV_DEFAULT_UC);
}

void EventLoop::register_watcher(EventWatcher* evt)
{
    ev_io_start(loop, &evt->watcher_get());
}

void EventLoop::unregister_watcher(EventWatcher* evt)
{
    ev_io_stop(loop, &evt->watcher_get());
}

static void sigint_cb(struct ev_loop* loop, ev_signal*, int)
{
    ev_break(loop, EVBREAK_ALL);
}

void EventLoop::register_sigint_watcher(ev_signal* evt_sig) const
{
    ev_signal_init(evt_sig, sigint_cb, SIGINT);
    ev_signal_start(loop, evt_sig);
}

void EventLoop::operator()() const
{
    ev_signal signal_watcher;
    register_sigint_watcher(&signal_watcher);
    ev_run(loop, 0);
}