#pragma once
#include <functional>
#include <boost/asio.hpp>
#include <google/protobuf/message.h>

#include "Macro.h"
#include "CircularBuffer.h"
#include "BufferPool.h"
#include "LockFreeQueue.h"
#include "MsgHandler.h"

namespace BLUE_BERRY
{
using boost::asio::ip::tcp;
class Session : public std::enable_shared_from_this<Session>
{
public:
	Session(tcp::socket socket_);
	//Session(tcp::socket&& socket_);
	Session(boost::asio::io_context& io_);
	virtual ~Session();

	// address function
	
	// sending proc
	// send buffer queue
	// status sending

	
	// closing proc
	// status connected

	// receiving proc
	// recv buffer

	// io complete function
	virtual void onClose() {}
	virtual void onRecvComplete(boost::system::error_code errCode_, std::size_t length_);
	virtual void onSendComplete(boost::system::error_code errCode_, std::size_t length_);
	virtual void onAcceptComplete();
	virtual void onConnectComplete(boost::system::error_code errCode_);

	virtual void recvPacketProc();
	virtual void SendPacket(short id_, google::protobuf::Message* msg_) {}
	virtual void setMsgHandler(MsgHandler<Session>* handler_) {}
	// socket
	tcp::socket& socket()
	{
		return _socket;
	}

	// recv data proc
	void setPacketProcHandler(std::function<void(CircularBufferPtr)> handler_) { _packetProc = handler_; }

	// send data
	void send(BufferHelperPtr sendBuff_);

	// close network
	void disconnect();

	bool isConnected();

	// connection
	void connect(const std::string& addr_, const std::string& port_);


private:
	void postRecv();
	void postSend();

private:
	tcp::socket _socket;

protected:
	// connected
	//boost::asio::io_context& _io;

	// connecting status
	std::atomic<bool> _connected;

	// recv buffer
	CircularBufferPtr _recvBuff;

	// reserved send buff queue
	LockFreeQueue<BufferHelperPtr, 65536> _reservedSendBuffs;
	std::atomic<long> _reservedSendBuffCount;

	// sending status
	std::atomic<bool> _sending;

	// send buffer
	CircularBufferPtr _sendBuff;

	// packet proc func
	std::function<void(SharedPtr<CircularBuffer>)> _packetProc;

};

DECLARE_SMART_PTR(Session);

}