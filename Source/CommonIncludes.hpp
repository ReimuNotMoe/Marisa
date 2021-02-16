/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include <iostream>
#include <istream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <memory>
#include <regex>
#include <string>
#include <functional>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <variant>
#include <chrono>

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/poll.h>

#include <microhttpd.h>

#if MHD_VERSION < 0x00097001
#define MHD_Result int
#endif

#ifdef __CYGWIN__
#define ERESTART 85
#endif

#include <IODash.hpp>
#include <ReGlob.hpp>

#include <fmt/core.h>

#include <ThreadPool.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

#include <cpp-base64X.hpp>

using namespace YukiWorkshop;
using namespace SudoMaker;
using namespace IODash;