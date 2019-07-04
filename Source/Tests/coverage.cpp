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


class blocking_class_send : public Middleware {
public:
	void handler() override {
		try {
			response->send("Hello Marisa");
		} catch (...) {

		}
	}

	std::unique_ptr<Middleware> New() const override {
		return std::make_unique<blocking_class_send>();
	}
};

class blocking_class_write : public Middleware {
public:
	void handler() override {
		try {
			response->write("Hello Marisa");
			response->end();
		} catch (...) {

		}
	}

	std::unique_ptr<Middleware> New() const override {
		return std::make_unique<blocking_class_write>();
	}
};

int main() {

	App myapp;

	myapp.route("/blocking_class_send").on("GET").use(blocking_class_send());
	myapp.route("/blocking_class_write").on("GET").use(blocking_class_write());
	myapp.route("/blocking_simple").on("GET").use(Simple("Hello Marisa"));
	myapp.route("/blocking_lambda").on("GET").use(Lambda([](auto req, auto rsp, auto ctx){
		rsp->send("Hello Marisa");
	}));


	myapp.listen(46878);
	std::thread ([&]() {
		myapp.run(2);
		exit(0);
	}).detach();

	sleep(2);

	std::string cmd_get_prefix = "curl -v 127.0.0.1:46878/";

	for (auto &url : {"blocking_class_send", "blocking_class_write", "blocking_simple", "blocking_lambda"}) {
		auto t = cmd_get_prefix + url;
		system(t.c_str());
	}

	myapp.stop();

	while (true)
		sleep(-1);
}