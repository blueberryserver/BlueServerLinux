#pragma once
#include "Acceptor.h"
namespace BLUE_BERRY
{

class Server
{
public:
	Server();
	~Server();
	
	void start();
	void stop();

private:
	AcceptorPtr _acceptor;
};


}