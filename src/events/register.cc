#include "events/register.hh"

namespace http
{
    EventWatcherRegistry event_register;

    bool EventWatcherRegistry::unregister_ew(EventWatcher* ev)
    {
        return events_.erase(ev);
    }

    std::optional<std::shared_ptr<EventWatcher>>
    EventWatcherRegistry::at(EventWatcher* ev)
    {
        try
        {
            auto res = events_.at(ev);
            return res;
        }
        catch(const std::out_of_range& e)
        {
            return nullptr;
        }
    }
} // namespace http
