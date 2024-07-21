#pragma once

#include "boost\asio.hpp"
#include "Secsion.h"

#include <iostream>
#include <map>

class EchoServer
{
public:
	EchoServer(boost::asio::io_context& ioc, std::string server_ip, uint16_t port) :
		_acceptor(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(server_ip), port)),
		the_ioc(ioc)
	{
		StartAccepte();
	}

public:
	void RemoveActiveSecsion(const std::string& uuid)
	{
		_secsion_map.erase(uuid);
	}
private:
	void StartAccepte()
	{
		std::cout << "wait for client connect..." << std::endl;

		std::shared_ptr<Secsion> new_secsion = std::make_shared<Secsion>(the_ioc, this);
		
		auto accept_token = std::bind(&EchoServer::AcceptorCallback, this, std::placeholders::_1, new_secsion);
		_acceptor.async_accept(new_secsion->GetComSock(), accept_token);
	}
	void AcceptorCallback(const boost::system::error_code& ec, std::shared_ptr<Secsion> new_secsion)
	{
		if (ec) {
			std::cout << ec.what() << std::endl;
			return;
		}

		_secsion_map.insert(std::make_pair(new_secsion->GetUniqueId(), new_secsion));
		new_secsion->Start();

		std::cout << "client is connecting!!!" << std::endl;

		StartAccepte();
	}

private:
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::io_context& the_ioc;
	std::map<std::string, std::shared_ptr<Secsion>> _secsion_map;
};

