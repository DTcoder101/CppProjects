#include "EchoServer.h"

int main()
{
	boost::asio::io_context ioc;

	EchoServer server(ioc, "127.0.0.1", 10086);
	ioc.run();
	return 0;
}