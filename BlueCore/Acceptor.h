#pragma once
#include <boost/asio.hpp>
#include "Session.h"
using boost::asio::ip::tcp;

namespace BLUE_BERRY
{

class Acceptor
{
public:
	Acceptor(boost::asio::io_service& io_, short port_);
	~Acceptor();
	void start();
	void stop();

private:
	void asyncAccept();
	void onAccept(SessionPtr session_, boost::system::error_code errCode_);

private:
	tcp::acceptor _acceptor;
	//tcp::socket _socket;
};

typedef std::shared_ptr<Acceptor> AcceptorPtr;
}