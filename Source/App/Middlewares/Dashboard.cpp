/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "Dashboard.hpp"

#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace Marisa::Middlewares;

void Dashboard::handler() {
	auto pid = getpid();
	utsname uname_;
	uname(&uname_);
	rusage rusage_;
	getrusage(RUSAGE_SELF, &rusage_);

	auto page = fmt::format("<!DOCTYPE html>\n"
				"<html>\n"
				"<head><title>Marisa Dashboard</title></head>\n"
				"<body bgcolor=\"white\">\n"
				"<h1>Marisa Dashboard</h1>\n"
				"<hr width=\"100%\">"
				"OS: {os}<br>"
				"PID: {pid}<br>"
				"Hardware concurrency: {cpus}<br>"
				"Memory usage: {rss:.3f} KiB<br>"
				"CPU time spent in userspace: {cputime_user:.6f} s<br>"
				"CPU time spent in kernel: {cputime_kern:.6f} s<br>"
				"Voluntary context switches: {nvcsw}<br>"
				"Involuntary context switches: {nivcsw}<br>"
				"<hr width=\"100%\">Marisa/" MARISA_VERSION "\n"
				"</body>\n"
				"</html>",
				fmt::arg("os", uname_.sysname),
				fmt::arg("pid", pid),
				fmt::arg("cpus", std::thread::hardware_concurrency()),
				fmt::arg("nvcsw", rusage_.ru_nvcsw),
				fmt::arg("nivcsw", rusage_.ru_nivcsw),
				fmt::arg("cputime_user", (double)rusage_.ru_utime.tv_sec + (double)rusage_.ru_utime.tv_usec / 1000000),
				fmt::arg("cputime_kern", rusage_.ru_stime.tv_sec + (double)rusage_.ru_stime.tv_usec / 1000000),
				fmt::arg("pid", pid),
				fmt::arg("rss", ((float)Util::memory_usage() / 1024) - (float)rusage_.ru_ixrss)
	);

	response->send(page);
}
