#include "Acceptor.h"
#include "Session.h"
#include <thread>
#include <boost/thread.hpp>
namespace BLUE_BERRY
{

Acceptor::Acceptor(boost::asio::io_service& io_, short port_)
	:_acceptor(io_, tcp::endpoint(tcp::v4(), port_)), _socket(io_)
{
}


Acceptor::~Acceptor()
{
}

void Acceptor::start()
{
	asyncAccept();

	boost::thread thread(std::bind(&Acceptor::run, this));

	thread.join();
}

void Acceptor::asyncAccept()
{
	_acceptor.async_accept(_socket, std::bind(&Acceptor::onAccept, this, std::placeholders::_1));
}

void Acceptor::onAccept(boost::system::error_code errCode_)
{
	if (!errCode_)
	{
		std::make_shared<Session>(std::move(_socket))->onAcceptComplete();
	}

	asyncAccept();
}

void Acceptor::run()
{
	_acceptor.get_io_service().run();
}

}