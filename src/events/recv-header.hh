#pragma once

#include "events/events.hh"

namespace http
{

    class recvheader : public EventWatcher
    {
    public:


        void operator()();


    protected:
        /**
         * \brief Callback for event handling.
         *
         * Each time an event occurs in the loop, this callback will be called
         * with its associated ev_io. This method is called by libev and uses
         * dynamic dispatch to run the correct overload of the operator().
         *
         * \param watcher ev_io* which received an event.
         */
        static void event_callback(struct ev_loop* loop, ev_io* w, int revents);

        /**
         * \brief Libev's io watcher.
         */
        ev_io watcher_;
    };
} // namespace http
