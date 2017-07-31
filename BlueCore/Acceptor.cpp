#include "Acceptor.h"
#include "Session.h"
#include "Logger.h"
namespace BLUE_BERRY
{

Acceptor::Acceptor(boost::asio::io_service& io_, short port_)
	:_acceptor(io_, tcp::endpoint(tcp::v4(), port_))
{
}


Acceptor::~Acceptor()
{
}

void Acceptor::start()
{
	asyncAccept();
}

void Acceptor::stop()
{
}

void Acceptor::asyncAccept()
{
	auto session = std::make_shared<Session>(_acceptor.get_io_service());
	_acceptor.async_accept(session->socket(), std::bind(&Acceptor::onAccept, this, session, std::placeholders::_1));
}

void Acceptor::onAccept(SessionPtr session_, boost::system::error_code errCode_)
{
	if (!errCode_)
	{
		session_->onAcceptComplete();
	}

	asyncAccept();
}

}