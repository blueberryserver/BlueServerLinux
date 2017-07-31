#pragma once
#include <boost/asio.hpp>
#include <functional>

#include "Macro.h"
#include "CircularBuffer.h"
#include "BufferPool.h"
#include "LockFreeQueue.h"

namespace BLUE_BERRY
{
using boost::asio::ip::tcp;
class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(boost::asio::io_service& io_);
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
	virtual void onSendComplete(boost::system::error_code errCode_, std::size_t length_);
	virtual void onAcceptComplete();
	virtual void onConnectComplete(boost::system::error_code errCode_);

	// socket
	tcp::socket& socket()
	{
		return _socket;
	}

	// recv data proc
	virtual void recvPacketProc();
	void setPacketProcHandler(std::function<void(CircularBuffer*)> handler_) { _packetProc = handler_; }

	// send data
	void send(BufferHelperPtr sendBuff_);

	// close network
	void disconnect();

	bool isConnected();

	// connection
	void connect(const char* addr_, short port_);


private:
	void postRecv();
	void postSend();

private:
	tcp::socket _socket;

protected:

	// connecting status
	std::atomic<bool> _connected;

	// recv buffer
	CircularBuffer* _recvBuff;

	// reserved send buff queue
	LockFreeQueue<BufferHelperPtr> _reservedSendBuffs;
	std::atomic<long> _reservedSendBuffCount;

	// sending status
	std::atomic<bool> _sending;

	// send buffer
	CircularBuffer* _sendBuff;

	// packet proc func
	std::function<void(CircularBuffer*)> _packetProc;

};
//typedef std::shared_ptr<Session> SessionPtr;
DECLARE_SMART_PTR(Session);

}