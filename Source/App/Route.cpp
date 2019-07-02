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

#include "Route.hpp"
#include "Middlewares/Lambda.hpp"

using namespace Marisa::Application;

Route &Route::stream() {
	mode_streamed = true;
	return *this;
}

Route &Route::async() {
	mode_async = true;
	return *this;
}

Route &Route::no_yield() {
	mode_no_yield = true;
	return *this;
}

RouteMethods &Route::on(const std::string &__method) {
	try {
		HTTP::Method hm(__method);
		auto hmi = (int) hm.operator http_method();
		auto it = routemethods_mapping.find(hmi);

		if (it == routemethods_mapping.end())
			routemethods_mapping.insert(std::pair<int, RouteMethods>(hmi, RouteMethods(*this)));

		return routemethods_mapping.find(hmi)->second;

	} catch (std::invalid_argument &e) {
		std::string sdup = __method;
		for (auto &it : sdup)
			it = (char)toupper(it);

		if (sdup != "ALL" && sdup != "*")
			throw e;

		return routemethods_default;
	}
}

RouteMethods &RouteMethods::use(const Middleware& __ref_mw) {
	middlewares.emplace_back(__ref_mw.New());
	return *this;
}
//
//RouteMethods &RouteMethods::use_lambda(void (*__func)(Request::RequestContext *, Response::ResponseContext *, Context *)) {
//	middlewares.emplace_back(Middlewares::Lambda(__func).New());
//	return *this;
//}
