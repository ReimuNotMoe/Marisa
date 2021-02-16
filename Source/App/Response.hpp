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

		/**
		 * Extra data sent by the client during the upgrade process.
		 *
		 */
		std::vector<uint8_t> extra_data() {
			return std::move(extra_data_);
		}

		/**
		 * Close the upgraded connection.
		 *
		 */
		void close() noexcept override {
			MHD_upgrade_action(urh_, MHD_UPGRADE_ACTION_CLOSE);
		}
	};

	class Response {
	protected:
		void *context = nullptr;

		Buffer output_buffer;

		std::pair<Socket<AddressFamily::Unix, SocketType::Stream>, Socket<AddressFamily::Unix, SocketType::Stream>> output_sp;

		bool finalized = false;

		std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> time_start;

		void init();

		void finish_time_measure(MHD_Response *resp);

		static void mhd_upgrade_callback(void *cls, struct MHD_Connection *con, void *con_cls,
						 const char *extra_in, size_t extra_in_size, MHD_socket sock, struct MHD_UpgradeResponseHandle *urh);

	public:
		std::unordered_map<std::string, std::string> header;	/*!< HTTP headers */
		int status = 200;					/*!< HTTP status code */

		std::optional<UpgradedConnection> upgraded_connection;	/*!< Context of upgraded connection */

		Response(void *__context) : context(__context) {

		}

		/**
		 * Enable middleware execution time measurement.
		 *
		 * Can only be used in normal mode.
		 * The execution time will be added to the header of response with key "X-Marisa-Execution-Time".
		 *
		 */
		void measure_execution_time();

		/**
		 * Check if current route is streamed.
		 *
		 * @return true for streamed.
		 *
		 */
		bool streamed() const noexcept;

		/**
		 * Write data to client.
		 *
		 * In normal mode, the data will be buffered until end().
		 * In streamed mode, the data will be sent to client immediately. So you must add all required headers before calling this function.
		 *
		 * @param buf	Pointer to buffer.
		 * @param len	Length of buffer.
		 */
		void write(const void *buf, size_t len);

		/**
		 * Write data to client.
		 *
		 * In normal mode, the data will be buffered until end().
		 * In streamed mode, the data will be sent to client immediately. So you must add all required headers before calling this function.
		 *
		 * @param buf	C-style null-terminated string.
		 */
		void write(const char *buf);

		/**
		 * Write data to client.
		 *
		 * In normal mode, the data will be buffered until end().
		 * In streamed mode, the data will be sent to client immediately. So you must add all required headers before calling this function.
		 *
		 * @param buf	STL-style contiguous container which element size is 1. e.g. std::string, std::vector<uint8_t>
		 */
		template<typename T>
		void write(const T& buf) {
			write(buf.data(), buf.size());
		}

		/**
		 * End transmitting data to client.
		 *
		 * After calling this function, subsequent calls to write() and send*() will be useless. Trailing middlewares will not be executed.
		 * In normal mode, all buffered data will soon be sent to client.
		 * In streamed mode, the stream will soon be closed.
		 *
		 */
		void end();

		/**
		 * Send data to client.
		 *
		 * A convenient combination of write() and end().
		 *
		 * @param buf	Pointer to buffer.
		 * @param len	Length of buffer.
		 */
		void send(const void *buf, size_t len) {
			write(buf, len);
			end();
		}

		/**
		 * Send data to client.
		 *
		 * A convenient combination of write() and end().
		 *
		 * @param buf	C-style null-terminated string.
		 */
		void send(const char *buf) {
			write(buf);
			end();
		}

		/**
		 * Send data to client.
		 *
		 * A convenient combination of write() and end().
		 *
		 * @param buf	STL-style contiguous container which element size is 1. e.g. std::string, std::vector<uint8_t>
		 */
		template<typename T>
		void send(const T& buf) {
			write(buf);
			end();
		}

		/**
		 * Send predefined status page to client.
		 *
		 * Can only be used in normal mode.
		 * After calling this function, subsequent calls to write() and send*() will be useless. Trailing middlewares will not be executed.
		 *
		 * @param code	HTTP status code. Use -1 to keep it unchanged.
		 */
		void send_status_page(int code = -1);

		/**
		 * Send data in persistent storage to client.
		 *
		 * Can only be used in normal mode.
		 * After calling this function, subsequent calls to write() and send*() will be useless. Trailing middlewares will not be executed.
		 *
		 * @param buf	Pointer to buffer.
		 * @param len	Length of buffer.
		 */
		void send_persistent(const void *buf, size_t len);

		/**
		 * Send data in persistent storage to client.
		 *
		 * Can only be used in normal mode.
		 * After calling this function, subsequent calls to write() and send*() will be useless. Trailing middlewares will not be executed.
		 *
		 * @param buf	C-style null-terminated string.
		 */
		void send_persistent(const char *buf);

		/**
		 * Send file to client.
		 *
		 * Can only be used in normal mode.
		 * After calling this function, subsequent calls to write() and send*() will be useless. Trailing middlewares will not be executed.
		 *
		 * @param path	Path to the file.
		 */
		void send_file(std::string_view path);

		/**
		 * Send upgrade request to client.
		 *
		 * Can only be used in normal mode.
		 * After calling this function, subsequent calls to write() and send*() will be useless. Trailing middlewares will not be executed.
		 * Can be used to implement websockets.
		 *
		 */
		void upgrade(); // TODO: Blocking wait for upgrade done

		~Response();
	};

	class ResponseExposed : public Response {
	public:
		using Response::output_sp;
		using Response::init;
		using Response::finalized;
	};


}