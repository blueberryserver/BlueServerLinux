#include "BlueSession.h"
#include "../BlueCore/Packet.h"
#include "../BlueCore/Logger.h"
#include "DefaultHandler.h"
#include "LoginHandler.h"

namespace BLUE_BERRY
{

void BlueSession::setMsgHandler(MsgHandler<Session>* handler_)
{
	_msgHandler = handler_;
}

BlueSession::BlueSession(tcp::socket socket_)
	: Session(std::move(socket_)), _msgHandler(DefaultHandler::getDefaultHandler())
{}

BlueSession::BlueSession(boost::asio::io_context& io_)
	: Session(io_), _msgHandler(DefaultHandler::getDefaultHandler())
{}

BlueSession::~BlueSession()
{
	LOG(L_INFO_, " ");
}

void BlueSession::onClose()
{
	_msgHandler->execute(shared_from_this());
}

void BlueSession::onAcceptComplete()
{
	setMsgHandler(LoginHandler::getLoginHandler());
	Session::onAcceptComplete();
}

void BlueSession::recvPacketProc()
{
	auto rBufferPoint = _recvBuff->getReadableBuffer();
	auto recvBuffSize = _recvBuff->getContiguiousBytes();

	Packet packet(rBufferPoint, recvBuffSize);
	while (packet.next())
	{
		if (_msgHandler) _msgHandler->execute(shared_from_this(), packet);
		_recvBuff->remove(packet.getPacketLength());
	}
}


void BlueSession::SendPacket(short id_, google::protobuf::Message* msg_)
{
	char buffer[1024* 32] = { 0, };
	msg_->SerializeToArray(buffer, sizeof(buffer));


	BufferHelperPtr packet(new BufferHelper(msg_->ByteSize() + sizeof(Header)));

	auto header = packet->getHeader<Header>();
	header->_id = id_;
	header->_len = (unsigned short)packet->_len;

	auto body = packet->getBody<Header>();
	memcpy(body, buffer, msg_->ByteSize());
	send(packet);
}

}