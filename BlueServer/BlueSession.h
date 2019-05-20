#pragma once
#include "../BlueCore/Session.h"	
#include "../BlueCore/MsgHandler.h"
#include "../BlueCore/MemoryPool.h"

namespace BLUE_BERRY
{
using boost::asio::ip::tcp;

class BlueSession : public Session
{
	enum { CLOSED = 10000, };
public:
	BlueSession(tcp::socket socket_);
	BlueSession(boost::asio::io_context& io_);
	virtual ~BlueSession();

	// io complete function
	virtual void onClose() override;
	virtual void onAcceptComplete() override;

	// recv data proc
	virtual void recvPacketProc() override;
	virtual void SendPacket(short id_, google::protobuf::Message* msg_) override;

	// setting handler
	virtual void setMsgHandler(MsgHandler<Session>* handler_);

DECLARE_NEW_DELETE
private:
	MsgHandler<Session>* _msgHandler;
};

DECLARE_SMART_PTR(BlueSession);
}