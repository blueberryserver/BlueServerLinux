#include "Server.h"
#include <iostream>
#include "Logger.h"
#include "IOService.h"
using boost::asio::ip::tcp;

namespace BLUE_BERRY
{


Server::Server()
{
}

Server::~Server()
{

}
void Server::start(short port_)
{
	_acceptor = std::make_shared<Acceptor>(IOService::getIOService()->getIO(), port_);
	_acceptor->start();
	LOG(L_INFO_, "Start Server");
}

void Server::stop()
{
	_acceptor->stop();
	LOG(L_INFO_, "Stop Server");
}

}