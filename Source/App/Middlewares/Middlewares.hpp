/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#pragma once

#include "../Context.hpp"
#include "../../Util/Util.hpp"
#include "../../Version.hpp"


namespace Marisa::Middlewares {

	extern std::function<void(Request *, Response *, Context *)> Dashboard();
	extern std::function<void(Request *, Response *, Context *)> Redirection(std::string location, int status_code = 302);
	extern std::function<void(Request *, Response *, Context *)> Simple(std::string str);
	extern std::function<void(Request *, Response *, Context *)> StaticFiles(std::string base_path, bool list_files = false);
	extern std::function<void(Request *, Response *, Context *)> Websocket();

}