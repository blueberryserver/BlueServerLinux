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
	Acceptor(boost::asio::io_context& io_, short port_)
		: _acceptor(io_, tcp::endpoint(tcp::v4(), port_)) {}
	~Acceptor() = default;
	void start()
	{
		_acceptor.set_option(tcp::acceptor::reuse_address(true));
		_acceptor.listen();

		asyncAccept();
	}
	void stop()
	{

	}

private:
	void asyncAccept()
	{
		_acceptor.async_accept([this](boost::system::error_code ec_, tcp::socket socket_) {
			if (!ec_)
			{
				std::make_shared<T>(std::move(socket_))->onAcceptComplete();
			}
			asyncAccept();
		});			
	}

private:
	tcp::acceptor _acceptor;
};

}