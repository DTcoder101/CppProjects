#pragma once

#include "boost\asio.hpp"
#include <iostream>

#define BUF_SIZE 1024



class EchoServer
{
	class Secsion
	{
		using BufType = char;
	public:
		Secsion(boost::asio::io_context& ioc) :_com_sock(ioc)
		{
			_buf = new BufType[BUF_SIZE];
			memset(_buf, 0, BUF_SIZE * sizeof(BufType));
		}
		~Secsion()
		{
			delete[] _buf;
		}
		boost::asio::ip::tcp::socket& GetComSock()
		{
			return _com_sock;
		}

		void Start()
		{
			Read();
		}
	private:

		void Read()
		{
			boost::asio::mutable_buffer read_buf(_buf, BUF_SIZE);

			auto read_token = std::bind(&Secsion::ReadCallback, this,
				std::placeholders::_1, std::placeholders::_2);
			_com_sock.async_read_some(read_buf, read_token);
		}

		void Send(size_t send_bytes)
		{
			auto send_token = std::bind(&Secsion::WriteCallback, this, std::placeholders::_1, std::placeholders::_2);
			_com_sock.async_send(boost::asio::buffer(_buf, send_bytes), send_token);
		}

		void WriteCallback(const boost::system::error_code& ec, const size_t send_bytes)
		{
			if (ec) {
				std::cout << "Write Error : " << ec.what() << std::endl;
				delete this;
				return;
			}

			memset(_buf, 0, BUF_SIZE * sizeof(BufType));

			Read();
		}
		void ReadCallback(const boost::system::error_code& ec, size_t read_bytes)
		{
			if (ec) {
				std::cout << "Read Error : " << ec.what() << std::endl;
				delete this;
				return;
			}

			std::cout << "Read from : " << _com_sock.remote_endpoint().address().to_string() << 
				" port is " << _com_sock .remote_endpoint().port() << std::endl;
			std::cout << "Read data : " << _buf << std::endl;

			Send(read_bytes);
		}


	private:
		boost::asio::ip::tcp::socket _com_sock;
		BufType* _buf;
	};


public:
	EchoServer(boost::asio::io_context& ioc, std::string server_ip, uint16_t port) :
		_acceptor(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(server_ip), port)),
		the_ioc(ioc)
	{
		StartAccepte();
	}

private:
	void StartAccepte()
	{
		std::cout << "wait for client connect..." << std::endl;

		Secsion* new_secsion = new Secsion(the_ioc);
		auto accept_token = std::bind(&EchoServer::AcceptorCallback, this, std::placeholders::_1, new_secsion);
		_acceptor.async_accept(new_secsion->GetComSock(), accept_token);
	}
	void AcceptorCallback(const boost::system::error_code& ec, Secsion* new_secsion)
	{
		if (new_secsion != nullptr) {
			if (ec) {
				delete new_secsion;
			}
			new_secsion->Start();
		}

		std::cout << "client is connecting!!!" << std::endl;
		
		StartAccepte();
	}

private:
	boost::asio::ip::tcp::acceptor _acceptor;
	boost::asio::io_context& the_ioc;
};

