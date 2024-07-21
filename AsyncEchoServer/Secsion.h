#pragma once

#include "boost\asio.hpp"
#include "boost\uuid\uuid_generators.hpp"
#include "boost\uuid\uuid_io.hpp"

#include "MsgNode.h"

#include <iostream>
#include <queue>
#include <mutex>

class EchoServer;

class Secsion : public std::enable_shared_from_this<Secsion>
{
	using BufType = char;
public:
	Secsion(boost::asio::io_context& ioc, EchoServer* owner);
	~Secsion() = default;

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
	void Read(std::shared_ptr<Secsion> self);

	void Send(char*msg, size_t send_bytes, std::shared_ptr<Secsion> self);

	void WriteCallback(const boost::system::error_code& ec, std::shared_ptr<Secsion> self);

	void ReadCallback(const boost::system::error_code& ec, size_t read_bytes, std::shared_ptr<Secsion> self);

private:
	boost::asio::ip::tcp::socket _com_sock;
	MsgNode _read_buf;
	std::string _this_uuid;
	EchoServer* _owner;
	std::queue<std::shared_ptr<MsgNode>> _write_que;
	bool _write_working;
	std::mutex _mut;
};
