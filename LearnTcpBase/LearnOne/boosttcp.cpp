#pragma once

#include "boosttcp.h"
#include "boost/asio.hpp"

int CreateEndpoint()
{
	std::string raw_ip = "172.0.0.1";
	uint16_t port = 3333;
	boost::system::error_code ec;

	boost::asio::ip::address ip_addr = boost::asio::ip::address::from_string(raw_ip, ec);
	if (ec.value() != 0) {
		return ec.value();
	}
	
	boost::asio::ip::tcp::endpoint ep(ip_addr, port);

	return 0;
}

int ServerEndpoint()
{
	uint16_t port = 3333;

	boost::asio::ip::address ip_addr = boost::asio::ip::address_v6::any();

	boost::asio::ip::tcp::endpoint ep(ip_addr, port);

	return 0;
}

int CreateTcpSocket()
{
	boost::asio::io_context ioc;
	boost::asio::ip::tcp protocol = boost::asio::ip::tcp::v4();
	boost::asio::ip::tcp::socket sock(ioc);
	boost::system::error_code ec;
	sock.open(protocol, ec);
	if (ec.value() != 0) {
		return ec.value();
	}

	return 0;
}

int CreateAcceptorSocket()
{
	//legcy acceptor 
	boost::asio::io_context ioc;
	/*boost::asio::ip::tcp::acceptor accept(ioc);
	boost::asio::ip::tcp protocol = boost::asio::ip::tcp::v4();
	boost::system::error_code ec;
	accept.open(protocol, ec);
	if (ec.value() != 0) {
		return ec.value();
	}*/

	//new 
	boost::asio::ip::tcp::acceptor accept(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 3333));


	return 0;
}

int BindAcceptor()
{
	boost::asio::io_context ioc;
	uint16_t port = 3333;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), port);

	boost::asio::ip::tcp::acceptor accept(ioc, ep.protocol());
	boost::system::error_code ec;
	accept.bind(ep, ec);
	if (ec.value() != 0) {
		return ec.value();
	}

	return 0;
}

int ClientConnect()
{
	boost::asio::io_context ioc;
	std::string server_ip = "127.0.0.1";
	uint16_t port = 3333;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(server_ip), port);

	try {
		boost::asio::ip::tcp::socket sock(ioc, ep.protocol());
		sock.connect(ep);
	}
	catch (boost::system::system_error& e) {
		return e.code().value();
	}

	return 0;
}

int DnsConnect()
{
	std::string host = "baidu.com";
	std::string port = "3000";
	boost::asio::io_context ioc;

	boost::asio::ip::tcp::resolver::query resolv(host, port, boost::asio::ip::tcp::resolver::query::numeric_service);
	boost::asio::ip::tcp::resolver res(ioc);

	boost::asio::ip::tcp::resolver::iterator iter = res.resolve(resolv);

	try {
		boost::asio::ip::tcp::socket sock(ioc);
		boost::asio::connect(sock, iter);
	}
	catch (boost::system::system_error& e) {


	}
}

int ServerAccept()
{
	boost::asio::io_context ioc;
	uint16_t port = 3333;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(), port);

	try {
		boost::asio::ip::tcp::acceptor accept(ioc);
		accept.bind(ep);
		accept.listen(5);

		boost::asio::ip::tcp::socket com_sock(ioc);
		accept.accept(com_sock);

	}
	catch (boost::system::system_error& e) {



	}
}


void UseBufferSequence()
{
	std::string str = "hello world!";
	boost::asio::const_buffer as_buffer(str.c_str(), str.length());
	std::vector<boost::asio::const_buffer> const_buf;
	const_buf.push_back(as_buffer);
}

void UseConstBuffer()
{
	boost::asio::const_buffers_1 buf = boost::asio::buffer("hello world!");
}

void UseCharArray()
{
	const size_t buf_size = 48;

	std::unique_ptr<char[]> buf(new char[buf_size]);

	boost::asio::const_buffers_1 cbuf = boost::asio::buffer(static_cast<const void*>(buf.get()), buf_size);
}

void WirteToSocket(boost::asio::ip::tcp::socket& sock)
{
	std::string buf = "hello world!";
	std::size_t current_wirte_bytes = 0;

	while (current_wirte_bytes != buf.length()){
		current_wirte_bytes += sock.write_some(boost::asio::buffer(buf.c_str() + current_wirte_bytes,
			buf.length() - current_wirte_bytes));
	}
}