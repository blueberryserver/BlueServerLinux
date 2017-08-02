#include "BlueSession.h"
#include "../BlueCore/Packet.h"
#include "../BlueCore/Logger.h"

namespace BLUE_BERRY
{

static MsgHandler<Session>* __msgHandler = nullptr;
void BlueSession::setMsgHandler(MsgHandler<Session>* handler_)
{
	__msgHandler = handler_;
}

BlueSession::BlueSession(boost::asio::io_service& io_)
	: Session(io_)
{
}


BlueSession::~BlueSession()
{
}

void BlueSession::recvPacketProc()
{
	auto rBufferPoint = _recvBuff->getReadableBuffer();
	auto recvBuffSize = _recvBuff->getContiguiousBytes();

	Packet packet(rBufferPoint, recvBuffSize);
	while (packet.next())
	{
		__msgHandler->execute(shared_from_this(), packet.getId(), packet.getData(), packet.getDataLength());
		_recvBuff->remove(packet.getPacketLength());
	}


	char buff[1024] = { 0, };
	memcpy(buff, rBufferPoint, recvBuffSize);

	LOG(L_INFO_, "Recv Complete", "buff size", (int)recvBuffSize, "data", buff);

	_recvBuff->remove(recvBuffSize);
}



}