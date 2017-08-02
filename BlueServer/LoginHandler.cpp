#include "LoginHandler.h"
#include "cpp/common.pb.h"
#include "cpp/login.pb.h"

#include "../BlueCore/DateTime.h"
#include "../BlueCore/Packet.h"
#include "../BlueCore/Logger.h"

#include "User.h"
namespace BLUE_BERRY
{

LoginHandler::LoginHandler()
{
	REGIST_HANDLER(MSG::LOGIN_REQ, LoginReq);
	REGIST_HANDLER(MSG::PING_REQ, PingReq);
}

bool LoginHandler::execute(SessionPtr session_, unsigned short id_, char * buff_, unsigned short len_)
{
	if (id_ >= MAX_MSG_ID || _table[id_] == NULL)
		return false;

	return _table[id_](session_, buff_, len_);
}


DEFINE_HANDLER(LoginHandler, SessionPtr, LoginReq)
{
	MSG::LoginReq req;
	req.ParseFromArray(body_, len_);

	LOG(L_INFO_, "LoginReq ", "id", req.id());


	MSG::LoginAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	auto userData = ans.mutable_data();

	userData->set_uid(10);
	userData->set_name("noom");
	userData->set_did("ios8");

	User user(*userData);
	auto jUser = user.to_json();
	LOG(L_INFO_, "User", "data", jUser);


	char buffer[1024] = { 0, };
	ans.SerializeToArray(buffer, sizeof(buffer));


	SendPacket(session_, MSG::LOGIN_ANS, buffer, ans.ByteSize());
	return true;
}


DEFINE_HANDLER(LoginHandler, SessionPtr, PingReq)
{
	MSG::PingReq req;
	req.ParseFromArray(body_, len_);

	LOG(L_INFO_, "PingReq ", "time", DateTime::getCurrentDateTime().format());

	MSG::PongAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	char buffer[1024] = { 0, };
	ans.SerializeToArray(buffer, sizeof(buffer));

	SendPacket(session_, MSG::PONG_ANS, buffer, ans.ByteSize());
	return true;
}


void LoginHandler::SendPacket(SessionPtr session_, short id_, char* buffer_, short len_)
{
	BufferHelperPtr packet(new BufferHelper(len_ + sizeof(Header)));

	auto header = packet->getHeader<Header>();
	header->_id = id_;
	header->_len = (unsigned short)packet->_len;

	auto body = packet->getBody<Header>();
	memcpy(body, buffer_, len_);

	session_->send(packet);
}
}