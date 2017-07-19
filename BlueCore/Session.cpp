#include "Session.h"
#include <iostream>
#include "Logger.h"
namespace BLUE_BERRY
{

Session::Session(boost::asio::io_service& io_)
	: _socket(io_)
{
}


Session::~Session()
{
}

void Session::onRecvComplete(boost::system::error_code errCode_, std::size_t length_)
{
	LOG(L_INFO_, "Recv Complete ", "socket", (int)_socket.native(), "length", (int)length_);
	if (length_ == 0) return;
	postRecv();
}

void Session::onSendComplete()
{
	std::cout << "send complete" << std::endl;
}

void Session::onAcceptComplete()
{
	LOG(L_INFO_, "Accept Complete ", "socket", (int)_socket.native());
	postRecv();
}

void Session::onConnectComplete()
{
	std::cout << "connect complete" << std::endl;
}

void Session::postRecv()
{
	_socket.async_receive(boost::asio::buffer(_buffer, 1024), std::bind(&Session::onRecvComplete, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

}
