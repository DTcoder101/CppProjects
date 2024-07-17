#ifndef __SECSION_H__
#define __SECSION_H__

#if _MSC_VER > 1000
#pragma once
#endif

#include "boost/asio.hpp"

#include <stdint.h>
#include <memory>
#include <queue>

class Secsion
{
	class MsgNode 
	{
	public:
		MsgNode(const char* msg_ptr, size_t lenght)
		{
			finished_len = 0;
			msg = new char[lenght];
			total_len = lenght;
			memcpy(msg, msg_ptr, lenght);
		}

		MsgNode(size_t lenght)
		{
			total_len = lenght;
			finished_len = 0;
			msg = new char[lenght];
		}

		~MsgNode()
		{
			if (msg) {
				delete[] msg;
				msg = nullptr;
			}
		}
	
	public:
		size_t GetTotalLen()
		{
			return total_len;
		}

		char* GetMsgAddr()
		{
			return msg;
		}

		void SetFinishedLen(size_t lenght)
		{
			if (finished_len >= total_len)
				return;

			finished_len += lenght;
		}

		size_t GetFinishedLen()
		{
			if (finished_len >= total_len)
				return total_len;
			else
				return finished_len;
		}

	private:
		char* msg;
		size_t total_len;
		size_t finished_len;
	};

	

public:
	Secsion(std::shared_ptr<boost::asio::ip::tcp::socket> communicat_sock);
	void Cnnect(const boost::asio::ip::tcp::endpoint& remote_ed);

	void WriteToSockErr(const std::string buf);
	void WriteCallbackErr(const boost::system::error_code& ec, size_t writed_bytes,
		std::shared_ptr<MsgNode> msg_node);

	void WriteToSock(const std::string buf);
	void WriteCallback(const boost::system::error_code& ec, size_t writed_bytes);

	void WriteAllToSock(const std::string buf);
	void WriteAllCallback(const boost::system::error_code& ec, size_t writed_bytes);

	void ReadFromSock(size_t want_read_bytes);
	void ReadCallback(const boost::system::error_code& ec, size_t read_bytes);
private:
	void SetBuffer(boost::asio::const_buffer& buf, std::shared_ptr<MsgNode> write_node, 
		size_t finished_bytes);
	void AsyncWrite(std::shared_ptr<MsgNode> write_node, size_t finished_bytes);
	void AsyncWriteAll(std::shared_ptr<MsgNode> write_node);
private:
	std::shared_ptr<boost::asio::ip::tcp::socket> sock;
	std::shared_ptr<MsgNode> write_node;
	std::shared_ptr<MsgNode> read_node;

	std::queue<std::shared_ptr<MsgNode>> write_qu;
	bool write_working;
	bool read_working;
};






#endif // !__SECSION_H__






