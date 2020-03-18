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

#include <string>

#include <ctime>

namespace Marisa {

	namespace Types {
		class Date {
		protected:
			bool valid = true;
			timespec time_{0};

		public:
			Date();

			Date(const std::string &__str);
			Date(timespec *__ts);

			void assign(const std::string &__str);
			void assign_unixtime(time_t __t) noexcept;
			void assign_timespec(timespec *__ts) noexcept;
			void assign_jstime(uint64_t __t) noexcept;

			std::string toGMTString() const noexcept; // JS style
			std::string toISOString() const noexcept; // JS style
			std::string toString() const noexcept; // JS style
			std::string toDateString() const noexcept; // JS style

			std::string toGoodString() const noexcept;

			const int64_t getTime() const noexcept; // JS style
			const time_t getTimeSeconds() const noexcept; // JS style

			const timespec& get_timespec() const noexcept;

		};
	}
}
