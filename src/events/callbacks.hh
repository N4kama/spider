#pragma once
#include <iostream>
#include <netinet/in.h>

#include "events/event-loop.hh"
#include "events/events.hh"
void accept_cb(struct ev_loop* loop, struct ev_io *watcher, int revents);
void read_cb(struct ev_loop* loop, struct ev_io *watcher, int revents);
