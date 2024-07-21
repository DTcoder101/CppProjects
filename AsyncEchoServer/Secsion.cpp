#include "Secsion.h"
#include "EchoServer.h"

Secsion::Secsion(boost::asio::io_context& ioc, EchoServer* owner) :_com_sock(ioc)
{
	_owner = owner;
	_write_working = false;
	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	_this_uuid = boost::uuids::to_string(uuid);
}

void Secsion::Read(std::shared_ptr<Secsion> self)
{
	std::cout << "Wait Read..." << std::endl;

	boost::asio::mutable_buffer read_buf(_read_buf._data, _read_buf._total_len);
	auto read_token = std::bind(&Secsion::ReadCallback, this,
		std::placeholders::_1, std::placeholders::_2, self);

	_com_sock.async_read_some(read_buf, read_token);

	std::cout << "Read once!!!" << std::endl;
}

void Secsion::Send(char* msg, size_t send_bytes, std::shared_ptr<Secsion> self)
{
	std::lock_guard lg(_mut);

	std::shared_ptr<MsgNode> temp_ptr = std::make_shared<MsgNode>(msg, send_bytes);
	_write_que.push(temp_ptr);

	if (_write_working) {
		return;
	}

	std::shared_ptr<MsgNode>& msg_ptr = _write_que.front();
	auto send_token = std::bind(&Secsion::WriteCallback, this, std::placeholders::_1, self);
	_com_sock.async_send(boost::asio::buffer(msg_ptr->_data, msg_ptr->_total_len), send_token);

	_write_working = true;
}

void Secsion::WriteCallback(const boost::system::error_code& ec, std::shared_ptr<Secsion> self)
{
	if (ec) {
		std::cout << "Write Error : " << ec.what() << std::endl;
		_owner->RemoveActiveSecsion(_this_uuid);
		return;
	}
	
	std::lock_guard lg(_mut);

	_write_que.pop();
	if (_write_que.empty()) {
		_write_working = false;
		return;
	}

	std::shared_ptr<MsgNode>& msg_ptr = _write_que.front();
	auto send_token = std::bind(&Secsion::WriteCallback, this, std::placeholders::_1, self);
	_com_sock.async_send(boost::asio::buffer(msg_ptr->_data, msg_ptr->_total_len), send_token);
}

void Secsion::ReadCallback(const boost::system::error_code& ec, size_t read_bytes, std::shared_ptr<Secsion> self)
{
	if (ec) {
		std::cout << "Read Error : " << ec.what() << std::endl;
		_owner->RemoveActiveSecsion(_this_uuid);
		return;
	}

	std::cout << "Read from : " << _com_sock.remote_endpoint().address().to_string() <<
		" port is " << _com_sock.remote_endpoint().port() << std::endl;
	std::cout << "Read data : " << _read_buf._data << std::endl;

	Send(_read_buf._data, read_bytes, self);

	_read_buf.Clear();

	Read(self);
}
