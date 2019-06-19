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

#ifndef MARISA_SERVER_CONNECTIONCONTEXT_HPP
#define MARISA_SERVER_CONNECTIONCONTEXT_HPP


namespace Marisa {
	namespace Server {
		class Address {
		protected:
		public:
			virtual std::string to_string() const = 0;
			virtual bool is_loopback() const = 0;
			virtual bool is_v4() const = 0;
			virtual bool is_v6() const = 0;
			virtual bool is_unix() const = 0;

			virtual ~Address() {

			}
		};

		class AddressTCP : public Address {
		protected:
		private:
			boost::asio::ip::address baddr;
		public:
			AddressTCP(boost::asio::ip::address __baddr) : baddr(std::move(__baddr)) {

			}

			std::string to_string() const override {
				return baddr.to_string();
			}

			bool is_loopback() const override {
				return baddr.is_loopback();
			};

			bool is_v4() const override {
				return baddr.is_v4();
			};

			bool is_v6() const override {
				return baddr.is_v6();
			};

			bool is_unix() const override {
				return false;
			};

		};

		class AddressUnix : public Address {
		protected:
		private:
			std::string baddr;
		public:
			AddressUnix(std::string __baddr) : baddr(std::move(__baddr)) {

			}

			std::string to_string() const override {
				return baddr;
			}

			bool is_loopback() const override {
				return false;
			};

			bool is_v4() const override {
				return false;
			};

			bool is_v6() const override {
				return false;
			};

			bool is_unix() const override {
				return true;
			};

		};

		class Endpoint {
		protected:
			std::unique_ptr<Address> __address;
		public:
			Endpoint() = default;

			virtual uint16_t port() const = 0;

			const Address& address() const {
				return *__address;
			};

		};

		class EndpointTCP : public Endpoint {
		protected:
		private:
			boost::asio::ip::tcp::endpoint bep;

		public:
			EndpointTCP(boost::asio::ip::tcp::endpoint __bep) : bep(std::move(__bep)) {
				__address = std::make_unique<AddressTCP>(bep.address());
			}

			uint16_t port() const override {
				return bep.port();
			}

		};

		class EndpointUnix : public Endpoint {
		protected:
		private:
			boost::asio::local::stream_protocol::endpoint bep;

		public:
			EndpointUnix(boost::asio::local::stream_protocol::endpoint __bep) : bep(std::move(__bep)) {
				__address = std::make_unique<AddressUnix>(bep.path());
			}

			uint16_t port() const override {
				return 0;
			}

		};


		class Socket {
		protected:
			std::unique_ptr<Endpoint> __local;
			std::unique_ptr<Endpoint> __remote;
		public:

			virtual int fd() const = 0;
			virtual bool is_unix() const = 0;
			virtual bool is_tcp() const = 0;

			const Endpoint& local_endpoint() const {
				return *__local;
			}

			const Endpoint& remote_endpoint() const {
				return *__remote;
			}

		};

		class SocketTCP : public Socket {
		protected:
			boost::asio::ip::tcp::socket& bsocket;
		public:
			SocketTCP(boost::asio::ip::tcp::socket& __bs) : bsocket(__bs) {
				__remote = std::make_unique<EndpointTCP>(bsocket.remote_endpoint());
				__local = std::make_unique<EndpointTCP>(bsocket.local_endpoint());
			}

			int fd() const override {
				return bsocket.lowest_layer().native_handle();
			};

			bool is_tcp() const override {
				return true;
			}

			bool is_unix() const override {
				return false;
			}
		};

		class SocketUnix : public Socket {
		protected:
			boost::asio::local::stream_protocol::socket& bsocket;
		public:
			SocketUnix(boost::asio::local::stream_protocol::socket& __bs) : bsocket(__bs) {
				__remote = std::make_unique<EndpointUnix>(bsocket.remote_endpoint());
				__local = std::make_unique<EndpointUnix>(bsocket.local_endpoint());
			}

			int fd() const override {
				return bsocket.lowest_layer().native_handle();
			};

			bool is_tcp() const override {
				return true;
			}

			bool is_unix() const override {
				return false;
			}
		};


	}
}

#endif //MARISA_SERVER_CONNECTIONCONTEXT_HPP
