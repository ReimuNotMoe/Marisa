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

#include "Instance.hpp"

using namespace Marisa::Server;
using namespace Marisa::Application;
using namespace Marisa::Log;

void Instance::run() {
	run_impl();

	LogI("%s[0x%016" PRIxPTR "]:\tStarting server\n", ModuleName, (uintptr_t)this);
//	LogI("%s[0x%016" PRIxPTR "]:\trun: Starting %u threads for application\n", ModuleName, (uintptr_t)this, app.app_threads);
//	app_thread_pool = std::make_unique<ThreadPool>(app.app_threads);
	prepare_next_session();
	boost::system::error_code iosvc_error;
	io_service.run(iosvc_error);
	LogI("%s[0x%016" PRIxPTR "]:\trun: io_service runner exited: %s\n", ModuleName, (uintptr_t)this, iosvc_error.message().c_str());
}

void Instance::stop() {
	io_service.stop();
	LogI("%s[0x%016" PRIxPTR "]:\tStopping server\n", ModuleName, (uintptr_t)this);
}