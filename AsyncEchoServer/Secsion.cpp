#include "Secsion.h"
#include "EchoServer.h"

void Secsion::WriteCallback(const boost::system::error_code& ec, std::shared_ptr<Secsion> self)
{
	if (ec) {
		std::cout << "Write Error : " << ec.what() << std::endl;
		_owner->RemoveActiveSecsion(_this_uuid);
		return;
	}

	memset(_buf, 0, BUF_SIZE * sizeof(BufType));

	Read(self);
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
	std::cout << "Read data : " << _buf << std::endl;

	Send(read_bytes, self);
}
