#pragma once
#include <boost/asio.hpp>
#include <memory>

#include "Session.h"
using boost::asio::ip::tcp;

namespace BLUE_BERRY
{

template<typename T>
class Acceptor
{
public:
	Acceptor(boost::asio::io_service& io_, short port_)
		:_acceptor(io_, tcp::endpoint(tcp::v4(), port_)) {}
	~Acceptor() {}
	void start()
	{
		asyncAccept();
	}
	void stop()
	{

	}

private:
	void asyncAccept()
	{
		auto session = std::make_shared<T>(_acceptor.get_io_service());
		_acceptor.async_accept(session->socket(), std::bind(&Acceptor::onAccept, this, session, std::placeholders::_1));
	}

	void onAccept(std::shared_ptr<T> session_, boost::system::error_code errCode_)
	{
		if (!errCode_)
		{
			session_->onAcceptComplete();
		}

		//asyncAccept();
	}


private:
	tcp::acceptor _acceptor;
};

}