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

#include "Buffer.hpp"

namespace Marisa {
	class UpgradedConnection : public Socket<AddressFamily::Unix, SocketType::Stream> {
	private:
		using Socket::create;
		using Socket::listen;
		using Socket::bind;
		using Socket::connect;
		using Socket::accept;
	protected:
		MHD_UpgradeResponseHandle *urh_ = nullptr;
		std::vector<uint8_t> extra_data_;
	public:
		UpgradedConnection(MHD_UpgradeResponseHandle *urh, const void *extbuf, size_t extbuf_len, int fd) : Socket<AddressFamily::Unix, SocketType::Stream>(fd) {
			urh_ = urh;

			if (extbuf_len) {
				extra_data_.resize(extbuf_len);
				memcpy(extra_data_.data(), extbuf, extbuf_len);
			}
		}

		std::vector<uint8_t> extra_data() {
			return std::move(extra_data_);
		}

		void close() noexcept override {
			MHD_upgrade_action(urh_, MHD_UPGRADE_ACTION_CLOSE);
		}
	};

	class Response {
	protected:
		void *context = nullptr;

		Buffer output_buffer;

		std::pair<Socket<AddressFamily::Unix, SocketType::Stream>, Socket<AddressFamily::Unix, SocketType::Stream>> output_sp;
//		int output_sp[2] = {-1, -1};

		bool finalized = false;

		std::optional<std::chrono::system_clock::time_point> time_start;

		void init();

		void finish_time_measure(MHD_Response *resp);

		static void mhd_upgrade_callback(void *cls, struct MHD_Connection *con, void *con_cls,
						 const char *extra_in, size_t extra_in_size, MHD_socket sock, struct MHD_UpgradeResponseHandle *urh);

	public:
		std::optional<UpgradedConnection> upgraded_connection;

		std::unordered_map<std::string, std::string> header;
		int status = 200;
//			Date date;
//			std::string type = "text/html; charset=utf-8";

		Response(void *__context) : context(__context) {

		}

		void measure_execution_time();

		bool streamed() const noexcept;

		void write(const void *buf, size_t len);
		void write(const char *buf);

		template<typename T>
		void write(const T& buf) {
			write(buf.data(), buf.size());
		}

		void end();

		template<typename T>
		void send(const T& buf) {
			write(buf);
			end();
		}

		void send_persistent(const void *buf, size_t len);
		void send_persistent(const char *buf);

		void send_file(std::string_view path);

		void upgrade();

		~Response();
	};

	class ResponseExposed : public Response {
	public:
		using Response::output_sp;
		using Response::init;
	};


}