
#include "boost/asio.hpp"

#include <iostream>




int main()
{
	std::string ip = "127.0.0.1";
	uint16_t port = 10086;
	boost::asio::io_context ioc;
	//remote endpoint
	boost::asio::ip::address addr = boost::asio::ip::address::from_string(ip);
	boost::asio::ip::tcp::endpoint remote_ep(addr, port);
	//create socket
	boost::asio::ip::tcp::socket sock(ioc, boost::asio::ip::tcp::v4());
	boost::system::error_code ec;
	try {
		sock.open(boost::asio::ip::tcp::v4(), ec);
		sock.connect(remote_ep, ec);

		std::cout << "Enter message: " << std::endl;
		std::string buf("");
		std::cin >> buf;

		boost::asio::write(sock, boost::asio::const_buffer(buf.c_str(), buf.length()));

		char rbuf[1024] = { 0 };
		sock.read_some(boost::asio::mutable_buffer(rbuf, 1024));
		std::cout << "recv from remote : " << rbuf << std::endl;

	}
	catch (boost::system::system_error& e) {
		return e.code().value();
	}

	return 0;
}
