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

#include "Method.hpp"

using namespace Marisa::Protocol::HTTP;

const std::unordered_map<std::string, int> Marisa::Protocol::HTTP::Table_Method2Enum = {{ "DELETE", 0 }, {"GET", 1}, {"HEAD", 2}, {"POST", 3}, {"PUT", 4}, {"CONNECT", 5}, {"OPTIONS", 6}, {"TRACE", 7}, {"COPY", 8}, {"LOCK", 9}, {"MKCOL", 10}, {"MOVE", 11}, {"PROPFIND", 12}, {"PROPPATCH", 13}, {"SEARCH", 14}, {"UNLOCK", 15}, {"BIND", 16}, {"REBIND", 17}, {"UNBIND", 18}, {"ACL", 19}, {"REPORT", 20}, {"MKACTIVITY", 21}, {"CHECKOUT", 22}, {"MERGE", 23}, {"MSEARCH", 24}, {"NOTIFY", 25}, {"SUBSCRIBE", 26}, {"UNSUBSCRIBE", 27}, {"PATCH", 28}, {"PURGE", 29}, {"MKCALENDAR", 30}, {"LINK", 31}, {"UNLINK", 32}, {"SOURCE", 33}};


