#include "Secsion.h"

Secsion::Secsion(std::shared_ptr<boost::asio::ip::tcp::socket> communicat_sock)
{
	sock = communicat_sock;
	write_working = false;
	read_working = false;
}

void Secsion::Cnnect(const boost::asio::ip::tcp::endpoint& remote_ed)
{
	try{
		sock->connect(remote_ed);
	}
	catch (const boost::system::system_error& e){
		throw boost::system::system_error(e);
	}
}

void Secsion::WriteToSockErr(const std::string buf)
{
	void* msg_addr = nullptr;
	size_t len = 0;

	write_node = std::make_shared<MsgNode>(buf.c_str(), buf.length());
	msg_addr = write_node->GetMsgAddr();
	len = write_node->GetTotalLen();

	boost::asio::const_buffer write_buf(msg_addr, len);

	auto WriteToken = std::bind(&Secsion::WriteCallbackErr, this, 
		std::placeholders::_1, std::placeholders::_2, write_node);

	sock->async_write_some(write_buf, WriteToken);
}

void Secsion::WriteCallbackErr(const boost::system::error_code& ec, size_t writed_bytes, 
	std::shared_ptr<MsgNode> msg_node)
{
	size_t finished_len = msg_node->GetFinishedLen();
	size_t total_len = msg_node->GetTotalLen();
	if (finished_len + writed_bytes >= total_len) {
		return;
	}

	msg_node->SetFinishedLen(writed_bytes);
	finished_len = msg_node->GetFinishedLen();

	char* msg_addr = msg_node->GetMsgAddr() + finished_len;
	size_t surplus_bytes = total_len - finished_len;
	boost::asio::const_buffer write_buf(msg_addr, surplus_bytes);
	auto WriteToken = std::bind(&Secsion::WriteCallbackErr, this,
		std::placeholders::_1, std::placeholders::_2, msg_node);

	sock->async_write_some(write_buf, WriteToken);
}

void Secsion::WriteToSock(const std::string buf)
{
	write_qu.emplace(new MsgNode(buf.c_str(), buf.length()));

	if (write_working) {
		return;
	}

	AsyncWrite(write_qu.front(), 0);
	write_working = true;
}

void Secsion::WriteCallback(const boost::system::error_code& ec, size_t writed_bytes)
{
	std::shared_ptr<MsgNode> msg_node = write_qu.front();
	size_t finished_len = msg_node->GetFinishedLen();
	size_t total_len = msg_node->GetTotalLen();

	if (finished_len + writed_bytes >= total_len) {
		write_qu.pop();

		if (write_qu.empty()) {
			write_working = false;
			return;
		}
		else {
			msg_node = write_qu.front();
			writed_bytes = 0;
		}
	}

	msg_node->SetFinishedLen(writed_bytes);
	finished_len = msg_node->GetFinishedLen();

	AsyncWrite(msg_node, finished_len);
}

void Secsion::WriteAllToSock(const std::string buf)
{
	write_qu.emplace(new MsgNode(buf.c_str(), buf.length()));

	if (write_working) {
		return;
	}

	AsyncWriteAll(write_qu.front());
	write_working = true;
}

void Secsion::WriteAllCallback(const boost::system::error_code& ec, size_t writed_bytes)
{
	write_qu.pop();

	if (write_qu.empty()) {
		write_working = false;
		return;
	}

	AsyncWriteAll(write_qu.front());
}

void Secsion::ReadFromSock(size_t want_read_bytes)
{
	if (read_working) {
		return;
	}

	read_node = std::make_shared<MsgNode>(want_read_bytes);
	void* read_buf_addr = read_node->GetMsgAddr();
	size_t read_buf_len = read_node->GetTotalLen();
	boost::asio::mutable_buffer read_buf(read_buf_addr, read_buf_len);

	auto ReadToken = std::bind(Secsion::ReadCallback, this, std::placeholders::_1, std::placeholders::_2);

	sock->async_read_some(read_buf, ReadToken);
	read_working = true;
}

void Secsion::ReadCallback(const boost::system::error_code& ec, size_t read_bytes)
{
	read_node->SetFinishedLen(read_bytes);
	size_t finished_len = read_node->GetFinishedLen();
	size_t total_len = read_node->GetTotalLen();
	if (finished_len >= total_len){
		read_working = false;
		read_node = nullptr;
		return;
	}

	void* read_buf_addr = read_node->GetMsgAddr() + finished_len;
	size_t read_buf_len = read_node->GetTotalLen() - finished_len;
	boost::asio::mutable_buffer read_buf(read_buf_addr, read_buf_len);

	auto ReadToken = std::bind(Secsion::ReadCallback, this, std::placeholders::_1, std::placeholders::_2);

	sock->async_read_some(read_buf, ReadToken);
}

void Secsion::SetBuffer(boost::asio::const_buffer& buf, std::shared_ptr<MsgNode> write_node, 
	size_t finished_bytes)
{
	char* msg_addr = write_node->GetMsgAddr() + finished_bytes;
	size_t surplus_bytes = write_node->GetTotalLen() - finished_bytes;

	buf = boost::asio::const_buffer(msg_addr, surplus_bytes);
}

void Secsion::AsyncWrite(std::shared_ptr<MsgNode> write_node, size_t finished_bytes)
{
	boost::asio::const_buffer write_buf;
	SetBuffer(write_buf, write_node, finished_bytes);

	auto WriteToken = std::bind(&Secsion::WriteCallback, this,
		std::placeholders::_1, std::placeholders::_2);

	sock->async_write_some(write_buf, WriteToken);
}

void Secsion::AsyncWriteAll(std::shared_ptr<MsgNode> write_node)
{
	boost::asio::const_buffer write_buf;
	SetBuffer(write_buf, write_node, 0);

	auto WriteAllToken = std::bind(&Secsion::WriteAllCallback, this,
		std::placeholders::_1, std::placeholders::_2);

	sock->async_send(write_buf, WriteAllToken);
}


