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

static const char __ModuleName[] = "InstanceSSL";
static const char __ModuleName_Session[] = "SessionSSL";

InstanceSSL::InstanceSSL(Application::AppExposed &__ref_app) : InstanceTCP::InstanceTCP(__ref_app), ssl_ctx(boost::asio::ssl::context(boost::asio::ssl::context::tls)) {
	ModuleName = __ModuleName;

	ssl_ctx.set_options(
		boost::asio::ssl::context::default_workarounds
		| boost::asio::ssl::context::no_sslv2
		| boost::asio::ssl::context::no_sslv3
		| boost::asio::ssl::context::no_tlsv1
		| boost::asio::ssl::context::single_dh_use);

}

void InstanceSSL::prepare_next_session() {
	auto next_session = std::make_shared<SessionSSL>(*this, ssl_ctx);
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

void InstanceSSL::run_impl() {
	boost::system::error_code ec;
	acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
}



