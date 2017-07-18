#pragma once
#include <boost/asio.hpp>
using boost::asio::ip::tcp;

namespace BLUE_BERRY
{

class Acceptor
{
public:
	Acceptor(boost::asio::io_service& io_, short port_);
	~Acceptor();
	void start();

private:
	void asyncAccept();
	void onAccept(boost::system::error_code errCode_);
	void run();
private:
	tcp::acceptor _acceptor;
	tcp::socket _socket;
};

typedef std::shared_ptr<Acceptor> AcceptorPtr;
}