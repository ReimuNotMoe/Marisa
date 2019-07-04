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


class hello_world : public Middleware {
public:

	void handler() override {
		std::stringstream sstr;
		sstr << "<html><head>";
		sstr << R"(<meta http-equiv="content-type" content="text/html; charset=UTF-8">)";
		sstr << "<title>Hello Marisa!</title>";
		sstr << "</head>";
		sstr << "<body>";
		sstr << "<h1>Hello Marisa!</h1>";
		sstr << "</body>";
		sstr << "</html>";

		try {
			response->send(sstr.str());
		} catch (...) {

		}
	}

	std::unique_ptr<Middleware> New() const override {
		return std::make_unique<hello_world>();
	}
};


int main() {
	App myapp;

	myapp.route("*").on("GET").use(hello_world());

	myapp.listen(8080);
	myapp.run(1);
}