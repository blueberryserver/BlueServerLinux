#pragma once
#include "../BlueCore/Session.h"
#include "../BlueCore/MsgHandler.h"

namespace BLUE_BERRY
{

class BlueSession : public Session
{
public:
	BlueSession(boost::asio::io_service& io_);
	virtual ~BlueSession();

	// io complete function
	//virtual void onRecvComplete(boost::system::error_code errCode_, std::size_t length_);
	//virtual void onSendComplete(boost::system::error_code errCode_, std::size_t length_);
	//virtual void onAcceptComplete();
	//virtual void onConnectComplete(boost::system::error_code errCode_);


	// recv data proc
	virtual void recvPacketProc() override;
	virtual void SendPacket(short id_, google::protobuf::Message* msg_) override;

	// setting handler
	static void setMsgHandler(MsgHandler<Session>* handler_);
};

DECLARE_SMART_PTR(BlueSession);
}