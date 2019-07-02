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

static const char __ModuleName[] = "InstanceTCP";
static const char __ModuleName_Session[] = "SessionTCP";

InstanceTCP::InstanceTCP(Application::AppExposed &__ref_app) : Instance::Instance(__ref_app), acceptor(io_service) {
	ModuleName = __ModuleName;
}

void InstanceTCP::listen(const boost::asio::ip::tcp::endpoint& __ep) { // Dual-stack
	acceptor = boost::asio::ip::tcp::acceptor(io_service);
	acceptor.open(__ep.protocol());
	boost::system::error_code ec;
	acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
	acceptor.set_option(so_reuseport(true), ec);
	if (__ep.address().is_v6() && __ep.address().is_unspecified())
		acceptor.set_option(boost::asio::ip::v6_only(false), ec);
	acceptor.bind(__ep);

	acceptor.listen(128);
}



void InstanceTCP::prepare_next_session() {
	auto next_session = std::make_shared<SessionTCP>(*this);
	next_session->ModuleName = __ModuleName_Session;

	acceptor.async_accept(next_session->socket(), io_strand.wrap([&, this_session = next_session](const boost::system::error_code &error){
		if (!error) {
#ifdef DEBUG
			LogD("%s[0x%016" PRIxPTR "]:\thandler_accept: New session accepted, ptr=%p\n", ModuleName, (uintptr_t)this, this_session.get());
#endif
			this_session->start();

		} else {
			LogE("%s[0x%016" PRIxPTR "]:\thandler_accept: %s, deleting session %p\n", ModuleName, (uintptr_t)this, error.message().c_str(), this_session.get());
		}

		prepare_next_session();
	}));
#ifdef DEBUG
	LogD("%s[0x%016" PRIxPTR "]:\tprepare_next_session: Done, ptr=%p\n", ModuleName, (uintptr_t)this, next_session.get());
#endif
}

void InstanceTCP::run_impl() {

}




