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

#include "../CommonIncludes.hpp"

// Buffer suitable for MHD

namespace Marisa {
	class Buffer {
	private:
		void *memory_ = nullptr;
		size_t size_ = 0;
	public:
		Buffer() = default;

		void *memory() const noexcept {
			return memory_;
		}

		size_t size() const noexcept {
			return size_;
		}

		template<typename T>
		void insert(const std::vector<T>& buf) {
			insert(buf.data(), buf.size() * sizeof(T));
		}

		template<typename T>
		void insert(const T& buf) {
			insert(buf.data(), buf.size());
		}

		void insert(const void *buf, size_t len) {
			auto offset = size_;
			size_ += len;

			if (!memory_) {
				memory_ = malloc(size_);
			} else {
				memory_ = realloc(memory_, size_);
			}

			memcpy((uint8_t *)memory_+offset, buf, len);
		}

	};

}