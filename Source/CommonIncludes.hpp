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

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/poll.h>

#include <microhttpd.h>
#include <gcrypt.h>

#include <IODash.hpp>
#include <ReGlob.hpp>

#include <fmt/core.h>

#include <threadpool11.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>

#include <cpp-base64X.hpp>

using namespace YukiWorkshop;
using namespace SudoMaker;
using namespace IODash;