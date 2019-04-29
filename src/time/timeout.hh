/**
 * \file time/timeout.hh
 * \brief Timeout declaration.
 */

#pragma once

#include <iostream>

namespace http
{
    struct timeout
    {
        timeout()
        {
            keep_alive = 0;
            transaction = 0;
            throughput_time = 0;
            throughput_val = 0;
        }
        float keep_alive;

        float transaction;

        float throughput_val;

        unsigned throughput_time;
    };
} // namespace http
