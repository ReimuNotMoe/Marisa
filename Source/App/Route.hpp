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

#pragma once

#include "../Server/Instance.hpp"
#include "../Protocol/HTTP/HTTP.hpp"
#include "Middleware.hpp"
#include "Context.hpp"

using namespace Marisa::Protocol;


namespace Marisa {
	namespace Application {
		class App;
		class AppExposed;
		class Context;
		class Route;
		class Middleware;

		class RouteMethods {
		protected:
			Route& route;
			std::vector<std::unique_ptr<Middleware>> middlewares;

		public:
			explicit RouteMethods(Route &__ref_route) : route(__ref_route) {};

			RouteMethods &use(const Middleware& __ref_mw);
//			RouteMethods &use_lambda(void (*__func)(Request::RequestContext *, Response::ResponseContext *, Context *));
		};

		class RouteMethodsExposed : public RouteMethods {
		public:
			using RouteMethods::route;
			using RouteMethods::middlewares;

			explicit RouteMethodsExposed(Route &__ref_route) : RouteMethods::RouteMethods(__ref_route) {};
		};

		class Route {
		protected:
			Application::AppExposed &app;
			std::unordered_map<int, RouteMethods> routemethods_mapping; // HTTP method => Route methods
			RouteMethods routemethods_default;
			bool mode_streamed = false;
			bool mode_async = false;
			bool mode_no_yield = false;

		public:
			explicit Route(Application::AppExposed &__ref_app) : app(__ref_app), routemethods_default(*this) {};

			Route &stream();

			Route &async();

			Route &no_yield();

			RouteMethods &on(const std::string &__method);
		};

		class RouteExposed : public Route {
		public:
			using Route::app;
			using Route::routemethods_mapping;
			using Route::routemethods_default;
			using Route::mode_streamed;
			using Route::mode_async;
			using Route::mode_no_yield;

		};
	}
}
