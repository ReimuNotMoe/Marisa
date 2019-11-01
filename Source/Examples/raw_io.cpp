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

static const char buf[] = "Hello Marisa!\n";

class hello_raw : public Middleware_RawIO {
public:

	void handler() override {
		session->write_blocking({buf, sizeof(buf)-1});
		session->close_socket();
	}

	std::unique_ptr<Middleware_RawIO> New() const override {
		return std::make_unique<hello_raw>();
	}
};

class hello_raw_async : public Middleware_RawIO {
public:

	void handler() override {
		session->write_async({buf, sizeof(buf)-1}, *yield_context);
		session->close_socket();
	}

	std::unique_ptr<Middleware_RawIO> New() const override {
		return std::make_unique<hello_raw_async>();
	}
};

class echo_server : public Middleware_RawIO {
public:

	void handler() override {
		session->write_async({"Type something!\n", 0}, *yield_context);
		while (1) {
			auto buf0 = session->read_async(*yield_context);
			session->write_async(Buffer(buf0), *yield_context);
		}
	}

	std::unique_ptr<Middleware_RawIO> New() const override {
		return std::make_unique<echo_server>();
	}
};

int main() {
	App myapp;

	myapp.use_raw(hello_raw());
	myapp.listen(4444);
	// Run: nc 127.0.0.1 4444

	App myapp2, myapp3;

	myapp2.use_raw(hello_raw_async(), true);
	myapp2.listen(4445);
	// Run: nc 127.0.0.1 4445

	myapp3.config.connection.timeout_seconds = 10;
	myapp3.use_raw(echo_server(), true);
	myapp3.listen(4446);
	// Run: nc 127.0.0.1 4446

	std::thread([&](){
		myapp2.run(1);
	}).detach();

	std::thread([&](){
		myapp3.run(1);
	}).detach();

	myapp.run(1);
}