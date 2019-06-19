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

#ifndef MARISA_MARISA_HPP
#define MARISA_MARISA_HPP

#include "App/App.hpp"
#include "App/Middlewares/Middlewares.hpp"
#include "App/Request/Request.hpp"
#include "App/Response/Response.hpp"
#include "Protocol/HTTP/Parser.hpp"
#include "Types/Date.hpp"
#include "Types/MMap.hpp"
#include "Types/Mullet.hpp"
#include "Types/Base64.hpp"

using namespace Marisa::Types;

#define MARISA_VERSION_MAJOR		0
#define MARISA_VERSION_MINOR		0
#define MARISA_VERSION_REV		1

namespace Marisa {
	extern const char Version[];
}

#endif //MARISA_MARISA_HPP
