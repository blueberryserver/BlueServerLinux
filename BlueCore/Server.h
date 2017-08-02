#pragma once
#include <boost/asio.hpp>
#include <memory>

#include "Acceptor.h"
#include "Logger.h"
namespace BLUE_BERRY
{

template<typename T>
class Server
{
public:
	Server() {}
	virtual ~Server() {}
	
	void start(short port_)
	{
		_acceptor = std::make_shared< Acceptor<T> >(IOService::getIOService()->getIO(), port_);
		_acceptor->start();
		LOG(L_INFO_, "Start Server");
	}

	void stop()
	{
		_acceptor->stop();
		LOG(L_INFO_, "Stop Server");
	}

private:
	std::shared_ptr< Acceptor<T> > _acceptor;
};

}