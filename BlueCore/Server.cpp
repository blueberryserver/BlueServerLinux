#include "Server.h"
#include <iostream>
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

namespace BLUE_BERRY
{


Server::Server()
{
	std::cout << "start server ...." << std::endl;
}


Server::~Server()
{
	std::cout << "stop server ...." << std::endl;
}
void Server::start()
{
	boost::asio::io_service io;

	_acceptor = std::make_shared<Acceptor>(io, 12300);
	_acceptor->start();
}
}