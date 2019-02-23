#include "events/register.hh"

namespace http
{
    EventWatcherRegistry event_register;
    std::optional<std::shared_ptr<EventWatcher>> EventWatcherRegistry::at(EventWatcher*)
    {
        //FIX ME
        return std::nullopt;
    }
}
