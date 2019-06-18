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

#include "App.hpp"
#include "../Utilities/Utilities.hpp"

using namespace Marisa;

const char ModuleName[] = "Application";

void App::listen(uint16_t __port, size_t __instances) {
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v6(), __port);

	for (size_t i=0; i<__instances; i++) {
		tcp_servers.emplace_back(std::make_unique<InstanceTCP>(static_cast<AppExposed &>(*this)));
		tcp_servers.back()->listen(ep);
	}

	LogI("%s[0x%016" PRIxPTR "]:\tListening dual-stack on port %" PRIu16 ", %zu instances\n", ModuleName, (uintptr_t)this, __port, __instances);
}


void App::listen(const std::string &__address, uint16_t __port, size_t __instances) {
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(__address), __port);

	for (size_t i=0; i<__instances; i++) {
		tcp_servers.emplace_back(std::make_unique<InstanceTCP>(static_cast<AppExposed &>(*this)));
		tcp_servers.back()->listen(ep);
	}

	LogI("%s[0x%016" PRIxPTR "]:\tListening on %s:%" PRIu16 ", %zu instances\n", ModuleName, (uintptr_t)this, __address.c_str(), __port, __instances);
}

void App::listen_ssl(uint16_t __port, void (*__ssl_setup)(boost::asio::ssl::context &), size_t __instances) {
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v6(), __port);

	for (size_t i=0; i<__instances; i++) {
		ssl_servers.emplace_back(std::make_unique<InstanceSSL>(static_cast<AppExposed &>(*this)));
		__ssl_setup(ssl_servers.back()->ssl_context());
		ssl_servers.back()->listen(ep);
	}

	LogI("%s[0x%016" PRIxPTR "]:\tListening dual-stack SSL on port %" PRIu16 ", %zu instances\n", ModuleName, (uintptr_t)this, __port, __instances);
}

void App::listen_ssl(const std::string &__address, uint16_t __port, void (*__ssl_setup)(boost::asio::ssl::context &), size_t __instances) {
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(__address), __port);

	for (size_t i=0; i<__instances; i++) {
		ssl_servers.emplace_back(std::make_unique<InstanceSSL>(static_cast<AppExposed &>(*this)));
		__ssl_setup(ssl_servers.back()->ssl_context());
		ssl_servers.back()->listen(ep);
	}

	LogI("%s[0x%016" PRIxPTR "]:\tListening dual-stack SSL on port %" PRIu16 ", %zu instances\n", ModuleName, (uintptr_t)this, __port, __instances);
}

void App::listen_unix(const std::string &__path) {
	unix_servers.emplace_back(std::make_unique<InstanceUnix>(static_cast<AppExposed &>(*this)));
	unix_servers.back()->listen(__path);
}

Route &App::route(const std::string &__route) {
	if (__route.empty())
		throw std::invalid_argument("route path can't be an empty string");

	if (__route[0] == '*') {
		route_global = std::make_shared<Route>(static_cast<AppExposed &>(*this));
		return *route_global;
	}

	if (__route[0] != '/')
		throw std::invalid_argument("non-global route path must begin with a slash '/'");

	auto route_sptr = std::make_shared<Route>(static_cast<AppExposed &>(*this));

	std::regex route_regex;

	if (__route[__route.size()-1] == '*') {
		std::string buf = "^(";
		buf.insert(buf.end(), __route.begin(), __route.end()-1);
		buf += ")(.*)";
		route_regex = std::regex(buf, std::regex_constants::extended);
	} else {
		route_regex = std::regex("^" + __route + "$", std::regex_constants::extended);
	}

	LogI("%s[0x%016" PRIxPTR "]:\troute: adding route 0x%016" PRIxPTR "\n", ModuleName, (uintptr_t)this, (uintptr_t)route_sptr.get());
	route_mapping.emplace_back(std::move(route_regex), route_sptr);

	return *route_sptr;
}

Route &App::route(std::regex __route_regexp) {
	auto route_sptr = std::make_shared<Route>(static_cast<AppExposed &>(*this));
	route_mapping.emplace_back(std::move(__route_regexp), route_sptr);

	return *route_sptr;
}
void App::init_seq_routemap() {
//	route_mapping_seq.clear();
//
//	for (auto &it: route_mapping) {
//		route_mapping_seq.emplace_back(it.first, it.second);
//	}
}

void App::run(size_t __threads_per_io_service) {
	io_threads = __threads_per_io_service;

	run_io(io_threads);
}

void App::run_io(size_t __threads) {
	if (!tcp_servers.empty()) {
		for (auto &it : tcp_servers) {
			for (size_t i = 0; i < __threads; i++) {
				runners.emplace_back(std::thread([&](){
					it->run();
				}));
			}
		}

		LogI("%s[0x%016" PRIxPTR "]:\trun: Started %ld threads for each TCP server instance, total %ld\n", ModuleName, (uintptr_t)this, __threads, __threads * tcp_servers.size());
	}

	if (!ssl_servers.empty()) {
		for (auto &it : ssl_servers) {
			for (size_t i = 0; i < __threads; i++) {
				runners.emplace_back(std::thread([&](){
					it->run();
				}));
			}
		}

		LogI("%s[0x%016" PRIxPTR "]:\trun: Started %ld threads for each SSL server instance, total %ld\n", ModuleName, (uintptr_t)this, __threads, __threads * ssl_servers.size());
	}

	if (!unix_servers.empty()) {
		for (auto &it : unix_servers) {
			for (size_t i = 0; i < __threads; i++) {
				runners.emplace_back(std::thread([&](){
					it->run();
				}));
			}
		}

		LogI("%s[0x%016" PRIxPTR "]:\trun: Started %ld threads for each UnixSocket server instance, total %ld\n", ModuleName, (uintptr_t)this, __threads, __threads * unix_servers.size());
	}


	for (auto &it : runners) {
		it.join();
	}
}