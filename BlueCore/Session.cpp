#include "Session.h"
#include <iostream>
namespace BLUE_BERRY
{

Session::Session(tcp::socket socket_)
	: _socket(std::move(socket_))
{
}


Session::~Session()
{
}

void Session::onRecvComplete(boost::system::error_code errCode_, std::size_t length_)
{
	std::cout << "recv complete" << std::endl;
}

void Session::onSendComplete()
{
	std::cout << "send complete" << std::endl;
}

void Session::onAcceptComplete()
{
	std::cout << "accept complete socket: " << 10 <<  std::endl;
}

void Session::onConnectComplete()
{
	std::cout << "connect complete" << std::endl;
}

void Session::postRecv()
{
	_socket.async_receive(boost::asio::buffer(_buffer, 1024), std::bind(&Session::onRecvComplete, this, std::placeholders::_1, std::placeholders::_2));

	postRecv();
}

}