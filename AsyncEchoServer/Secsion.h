#pragma once

#include "boost\asio.hpp"
#include "boost\uuid\uuid_generators.hpp"
#include "boost\uuid\uuid_io.hpp"

#define BUF_SIZE 1024

class EchoServer;

class Secsion : public std::enable_shared_from_this<Secsion>
{
	using BufType = char;
public:
	Secsion(boost::asio::io_context& ioc, EchoServer* owner) :_com_sock(ioc)
	{
		_owner = owner;
		_buf = new BufType[BUF_SIZE];
		memset(_buf, 0, BUF_SIZE * sizeof(BufType));
		boost::uuids::uuid uuid = boost::uuids::random_generator()();
		_this_uuid = boost::uuids::to_string(uuid);
	}
	~Secsion()
	{
		delete[] _buf;
	}

public:
	boost::asio::ip::tcp::socket& GetComSock()
	{
		return _com_sock;
	}

	void Start()
	{
		Read(shared_from_this());
	}

	const std::string& GetUniqueId()
	{
		return _this_uuid;
	}
private:
	void Read(std::shared_ptr<Secsion> self)
	{
		boost::asio::mutable_buffer read_buf(_buf, BUF_SIZE);

		auto read_token = std::bind(&Secsion::ReadCallback, this,
			std::placeholders::_1, std::placeholders::_2, self);
		_com_sock.async_read_some(read_buf, read_token);
	}

	void Send(size_t send_bytes, std::shared_ptr<Secsion> self)
	{
		auto send_token = std::bind(&Secsion::WriteCallback, this, std::placeholders::_1, self);
		_com_sock.async_send(boost::asio::buffer(_buf, send_bytes), send_token);
	}

	void WriteCallback(const boost::system::error_code& ec, std::shared_ptr<Secsion> self);

	void ReadCallback(const boost::system::error_code& ec, size_t read_bytes, std::shared_ptr<Secsion> self);

private:
	boost::asio::ip::tcp::socket _com_sock;
	BufType* _buf;
	std::string _this_uuid;
	EchoServer* _owner;
};
