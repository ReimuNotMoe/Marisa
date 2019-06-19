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


#include "../Marisa.hpp"

using namespace Marisa::Application;
using namespace Middlewares;


class send_single_file : public Middleware {
public:

	std::string path;

	explicit send_single_file(std::string __path) {
		path = std::move(__path);
	}

	void handler() override {
		response->send_file(path);
	}


	std::unique_ptr<Middleware> New() const override {
		return std::make_unique<send_single_file>(path);
	}
};


int main() {
	App myapp;

	auto &route = myapp.route("/test*");
	auto &route2 = myapp.route(std::regex("^/passw0rd", std::regex_constants::extended));

	route.on("*").use(send_single_file("/etc/issue"));
	route2.on("*").use(send_single_file("/etc/passwd"));

	myapp.listen(8080);
	myapp.run();
}