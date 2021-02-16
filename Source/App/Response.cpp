/*
    This file is part of Marisa.
    Copyright (C) 2015-2021 ReimuNotMoe <reimu@sudomaker.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the MIT License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include "Response.hpp"
#include "Context.hpp"

using namespace Marisa;

#define logger	(((Context *)context)->logger)

static const char ModuleName[] = "Response";

void Response::init() {
	auto *ctx = (Context *)context;

	if (ctx->streamed()) {
		output_sp = socket_pair<SocketType::Stream>();

		logger->debug(R"([{} @ {:x}] output_sp fds: {}, {})", ModuleName, (intptr_t) this, output_sp.first.fd(), output_sp.second.fd());

		output_sp.first.set_nonblocking();
		output_sp.second.set_nonblocking();
	}
}

void Response::mhd_upgrade_callback(void *cls, struct MHD_Connection *con, void *con_cls, const char *extra_in, size_t extra_in_size,
				    MHD_socket sock, struct MHD_UpgradeResponseHandle *urh) {
	auto *resp_ctx = (Response *)cls;
	auto *context = con_cls;

	logger->debug(R"([{} @ {:x}] mhd_upgrade_callback called)", ModuleName, (intptr_t)resp_ctx);

	resp_ctx->upgraded_connection = {urh, extra_in, extra_in_size, sock};
}

Response::~Response() {
	///close(output_sp[1]);
}

void Response::measure_execution_time() {
	time_start = std::chrono::high_resolution_clock::now();
}

void Response::finish_time_measure(MHD_Response *resp) {
	if (time_start) {
		auto time_end = std::chrono::high_resolution_clock::now();
		auto duration_ms = (float)std::chrono::duration_cast<std::chrono::microseconds>(time_end - *time_start).count() / 1000;
		MHD_add_response_header(resp, "X-Marisa-Execution-Time", std::to_string(duration_ms).c_str());
	}
}

bool Response::streamed() const noexcept {
	auto *ctx = (ContextExposed *)context;

	if (ctx->route)
		return ctx->route->mode_streamed;
	else
		return false;
}

void Response::write(const void *buf, size_t len) {
	logger->debug(R"([{} @ {:x}] write: bufsize: {})", ModuleName, (intptr_t)this, len);

	if (!finalized) {
		if (streamed()) {
			size_t sent = 0, left = len;

			while (1) {
				auto rc_send = output_sp.second.send((uint8_t *)buf+sent, left, MSG_NOSIGNAL);

				logger->debug(R"([{} @ {:x}] write: rc: {}, errno: {})", ModuleName, (intptr_t)this, rc_send, errno);

				if (rc_send > 0) {
					sent += rc_send;
					left -= rc_send;
					logger->debug(R"([{} @ {:x}] write: {}/{}, left: {})", ModuleName, (intptr_t)this, sent, len, left);

					if (!left) {
						break;
					}
				} else if (rc_send == 0) {
					logger->debug(R"([{} @ {:x}] write: other end shutdown'd fd)", ModuleName, (intptr_t)this);
					finalized = true;
					output_sp.second.close();
					break;
				} else {
					if (errno == EWOULDBLOCK || errno == EAGAIN || errno == ERESTART) {
						logger->debug(R"([{} @ {:x}] write EAGAIN, resume_connection)", ModuleName, (intptr_t)this);

						((ContextExposed *) context)->resume_connection();
						std::this_thread::yield();
					} else {
						logger->error(R"([{} @ {:x}] write errno: {})", ModuleName, (intptr_t)this, errno);

						finalized = true;
						try {
							output_sp.second.shutdown();
						} catch (std::exception &e) {
							logger->warn("[{} @ {:x}] shutdown failed: {}", ModuleName, (intptr_t)this, e.what());

						}
						break;
					}
				}
			}
		} else {
			output_buffer.insert(buf, len);
		}

		logger->debug(R"([{} @ {:x}] write: done)", ModuleName, (intptr_t)this);

	} else {
		logger->warn(R"([{} @ {:x}] write() called but already finalized)", ModuleName, (intptr_t)this);
	}
}

void Response::write(const char *buf) {
	write(buf, strlen(buf));
}

void Response::send_status_page(int code) {
	if (code != -1)
		status = code;

	auto &page = Util::default_status_page(status);
	header["Content-Type"] = "text/html";
	send_persistent(page.c_str(), page.size());
}

void Response::send_persistent(const void *buf, size_t len) {
	auto *ctx = (ContextExposed *)context;

	if (!finalized) {
		if (streamed()) {
			throw std::logic_error("send_persistent can't be used in streamed mode");
		} else {
			auto *resp = MHD_create_response_from_buffer(len, (void *)buf, MHD_RESPMEM_PERSISTENT);
			for (auto &it : header) {
				MHD_add_response_header(resp, it.first.c_str(), it.second.c_str());
			}
			finish_time_measure(resp);
			MHD_queue_response(ctx->mhd_conn, status, resp);
			MHD_destroy_response(resp);
			ctx->resume_connection();
			finalized = true;
		}
	} else {
		logger->warn(R"([{} @ {:x}] send_persistent() called but already finalized)", ModuleName, (intptr_t)this);
	}
}

void Response::send_persistent(const char *buf) {
	send_persistent(buf, strlen(buf));
}

void Response::send_file(std::string_view path) {
	auto *ctx = (ContextExposed *)context;

	if (!finalized) {
		if (streamed()) {
			throw std::logic_error("send_file can't be used in streamed mode");
		} else {
			int fd = open(path.data(), O_RDONLY);

			if (fd > 0) {
				struct stat sbuf;
				fstat(fd, &sbuf);

				auto *resp = MHD_create_response_from_fd64(sbuf.st_size, fd);
				for (auto &it : header) {
					MHD_add_response_header(resp, it.first.c_str(), it.second.c_str());
				}
				finish_time_measure(resp);
				MHD_queue_response(ctx->mhd_conn, status, resp);
				MHD_destroy_response(resp);
				ctx->resume_connection();

				finalized = true;
			} else {
				throw std::system_error(errno, std::system_category(), "failed to open file");
			}

		}
	} else {
		logger->warn(R"([{} @ {:x}] send_file() called but already finalized)", ModuleName, (intptr_t)this);
	}
}

void Response::end() {
	using namespace std::chrono_literals;
	auto *ctx = (ContextExposed *)context;

	if (!finalized) {
		if (streamed()) {
			output_sp.second.close();

			size_t rcnt = 0;
			while (!ctx->streamed_response_done) {
				logger->debug(R"([{} @ {:x}] poll response_done {} times)", ModuleName, (intptr_t)this, rcnt);
				ctx->resume_connection();
				std::this_thread::sleep_for(std::chrono::milliseconds((int)pow(2, rcnt)));
				rcnt++;
			}

			logger->debug(R"([{} @ {:x}] end in stream mode)", ModuleName, (intptr_t)this);
		} else {
			MHD_Response *resp = nullptr;
			if (output_buffer.size())
				resp = MHD_create_response_from_buffer(output_buffer.size(), output_buffer.memory(), MHD_RESPMEM_MUST_FREE);
			else
				resp = MHD_create_response_from_buffer(0, (void *)"", MHD_RESPMEM_PERSISTENT);

			for (auto &it : header) {
				MHD_add_response_header(resp, it.first.c_str(), it.second.c_str());
			}

			finish_time_measure(resp);
			MHD_queue_response(ctx->mhd_conn, status, resp);
			MHD_destroy_response(resp);


			ctx->resume_connection();

			logger->debug(R"([{} @ {:x}] end in normal mode)", ModuleName, (intptr_t)this);
		}

		finalized = true;
	} else {
		logger->warn(R"([{} @ {:x}] end() called but already finalized)", ModuleName, (intptr_t)this);

	}
}

void Response::upgrade() {
	auto *ctx = (ContextExposed *)context;

	if (!finalized) {
		if (streamed()) {
			throw std::logic_error("upgrade can't be used in streamed mode");
		} else {
			auto *resp = MHD_create_response_for_upgrade(&mhd_upgrade_callback, this);
			for (auto &it : header) {
				MHD_add_response_header(resp, it.first.c_str(), it.second.c_str());
			}
			finish_time_measure(resp);
			MHD_queue_response(ctx->mhd_conn, status, resp);
			MHD_destroy_response(resp);
			ctx->resume_connection();

			finalized = true;
		}
	} else {
		logger->warn(R"([{} @ {:x}] upgrade() called but already finalized)", ModuleName, (intptr_t)this);
	}
}











