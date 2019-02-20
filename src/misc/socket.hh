/**
 * \file misc/socket.hh
 * \brief Socket related syscalls.
 */

#pragma once

#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "misc/fd.hh"
#include "misc/sys-wrapper.hh"

namespace sys
{
    /**
     * \brief accept(2).
     */
    auto accept = make_wrapper<misc::FileDescriptor>(::accept);

    /**
     * \brief bind(2).
     */
    auto bind = make_wrapper<void>(::bind);

    /**
     * \brief connect(2).
     */
    auto connect = make_wrapper<void>(::connect);

    static int fcntl_wrapper(int fildes, int opts)
    {
        auto flags = ::fcntl(fildes, F_GETFL);
        return ::fcntl(fildes, F_SETFL, flags | opts);
    }
    /**
     * \brief fcntl(2).
     *
     * Since fcntl is a variadic syscall we need to call a helper function
     * with a fixed number of parameter to create a wrapper out of it.
     */
    auto fcntl_set = make_wrapper<int>(fcntl_wrapper);

    /**
     * \brief getsockname(2).
     */
    auto getsockname = make_wrapper<void>(::getsockname);

    /**
     * \brief listen(2).
     */
    auto listen = make_wrapper<void>(::listen);

    /**
     * \brief lseek(2).
     */
    auto lseek = make_wrapper<off_t>(::lseek);

    /**
     * \brief read(2).
     */
    auto read = make_wrapper<ssize_t>(::read);

    /**
     * \brief recv(2).
     */
    auto recv = make_wrapper<ssize_t>(::recv);

    /**
     * \brief send(2).
     */
    auto send = make_wrapper<ssize_t>(::send);

    /**
     * \brief sendfile(2).
     */
    auto sendfile = make_wrapper<ssize_t>(::sendfile);

    /**
     * \brief setsockopt(2).
     */
    auto setsockopt = make_wrapper<void>(::setsockopt);

    /**
     * \brief socket(2).
     */
    auto socket = make_wrapper<misc::FileDescriptor>(::socket);
} // namespace sys
