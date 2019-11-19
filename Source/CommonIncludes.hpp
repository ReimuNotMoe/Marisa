/*
    This file is part of Marisa.
    Copyright (C) 2018-2019 ReimuNotMoe

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef MARISA_COMMONINCLUDES_HPP
#define MARISA_COMMONINCLUDES_HPP

//#define DEBUG

#include <string>
#include <thread>
#include <memory>
#include <deque>
#include <vector>
#include <algorithm>
#include <mutex>
#include <sstream>
#include <regex>
#include <future>
#include <functional>
#include <condition_variable>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstdint>
#include <cinttypes>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

#include <sys/un.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <zlib.h>

// io_context is only supported by boost 1.67+, so we use the old API here
// At least this is supported by boost 1.65 - 1.70
#define BOOST_COROUTINES_NO_DEPRECATION_WARNING

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/system_timer.hpp>
#include <boost/thread/thread.hpp>

//#include "3rdParty/ThreadPool/ThreadPool.h"
//#include "3rdParty/thread-pool/include/ThreadPool.h"

#endif //MARISA_COMMONINCLUDES_HPP
