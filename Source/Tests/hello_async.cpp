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

int main(int argc, char **argv) {
	App myapp2;

	// Middlewares in an async route must not do blocking operations, otherwise the event loop will stuck
	myapp2.route("/").async().on("GET").use(Simple("Hello Marisa!"));

	int nr_instances = 1;

	if (argv[1])
		nr_instances = strtol(argv[1], nullptr, 10);

	myapp2.listen(8080, nr_instances);
	myapp2.run();
}