
#include "boost\asio.hpp"

#include <iostream>



int main()
{
	std::string server_ip = "127.0.0.1";
	uint16_t port = 10086;

	boost::asio::io_context ioc;
	boost::asio::ip::tcp::endpoint server_ep(boost::asio::ip::address::from_string(server_ip), port);
	boost::asio::ip::tcp::acceptor server_accept(ioc);
	boost::asio::ip::tcp::socket comunicate_sock(ioc);

	try{
		char rbuf[1024] = {0};
		boost::system::error_code ec;
		server_accept.open(boost::asio::ip::tcp::v4());
		server_accept.bind(server_ep);
		server_accept.listen(5);
		server_accept.accept(comunicate_sock);

		while (true)
		{
			memset(rbuf, 0, sizeof(rbuf));
			size_t recv_size = comunicate_sock.read_some(boost::asio::mutable_buffer(rbuf, 1024), ec);
			if (ec == boost::asio::error::eof) {
				std::cout << "connection is closed!" << std::endl;
				break;
			}
			else if (ec){
				throw boost::system::system_error(ec);
			}

			std::cout << "recv from : " << comunicate_sock.remote_endpoint().address().to_string() << std::endl;
			std::cout << "content : " << rbuf << std::endl;

			//---
			boost::asio::write(comunicate_sock, boost::asio::const_buffer(rbuf, recv_size), ec);
		}

	}
	catch (boost::system::system_error& e){
		return e.code().value();
	}





	return 0;
}