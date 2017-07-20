#pragma once
#include <boost/asio.hpp>
#include "Acceptor.h"
namespace BLUE_BERRY
{

class Server
{
public:
	Server();
	~Server();
	
	void start(short port_);
	void stop();

private:
	AcceptorPtr _acceptor;
};


}