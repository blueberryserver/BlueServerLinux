#pragma once
#include <boost/asio.hpp>
namespace BLUE_BERRY
{
using boost::asio::ip::tcp;
class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(tcp::socket socket_);
	~Session();

	// address function
	
	// sending proc
	// send buffer queue
	// status sending

	
	// closing proc
	// status connected

	// receiving proc
	// recv buffer

	// io complete function
	virtual void onRecvComplete(boost::system::error_code errCode_, std::size_t length_);
	virtual void onSendComplete();
	virtual void onAcceptComplete();
	virtual void onConnectComplete();


private:
	void postRecv();

private:
	tcp::socket _socket;
	char _buffer[1024];
};

}