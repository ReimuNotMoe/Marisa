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
#include "../App/App.hpp"
#include "../App/Context.hpp"
#include "../Log/Log.hpp"
#include "../Utilities/Utilities.hpp"


using namespace Marisa::Server;
using namespace Marisa::Utilities;
using namespace Marisa::Application;
using namespace Marisa::Log;

static const char __ModuleName[] = "InstanceUnix";
static const char __ModuleName_Session[] = "SessionUnix";

InstanceUnix::InstanceUnix(Application::AppExposed &__ref_app) : Instance::Instance(__ref_app), acceptor(io_service){
	ModuleName = __ModuleName;
}

void InstanceUnix::prepare_next_session() {
	auto next_session = std::make_shared<SessionUnix>(*this);
	next_session->ModuleName = __ModuleName_Session;
	acceptor.async_accept(next_session->socket(), [&, this_session = next_session](const boost::system::error_code &error){
		if (!error) {
#ifdef DEBUG
			LogD("%s[0x%016" PRIxPTR "]:\thandler_accept: New session accepted, ptr=%p\n", ModuleName, (uintptr_t)this, this_session.get());
#endif
			this_session->start();

		} else {
			LogE("%s[0x%016" PRIxPTR "]:\thandler_accept: %s, deleting session %p\n", ModuleName, (uintptr_t)this, error.message().c_str(), this_session.get());
		}

		prepare_next_session();
	});
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tprepare_next_session: Done, ptr=%p\n", ModuleName, (uintptr_t)this, next_session.get());
#endif
}

void InstanceUnix::listen(const std::string &__path) {
	boost::asio::local::stream_protocol::endpoint ep;
	ep.path(__path);
	acceptor = boost::asio::local::stream_protocol::acceptor(io_service, ep);
	LogI("%s[0x%016" PRIxPTR "]:\tWill listen on unix domain socket `%s'\n", ModuleName, (uintptr_t)this, __path.c_str());
}

void InstanceUnix::run_impl() {

}