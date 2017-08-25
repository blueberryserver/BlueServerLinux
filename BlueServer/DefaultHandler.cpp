#include "DefaultHandler.h"
#include "cpp/login.pb.h"

#include "../BlueCore/DateTime.h"
#include "../BlueCore/Packet.h"
#include "../BlueCore/Logger.h"


#include "UserManager.h"

namespace BLUE_BERRY
{

DEFINE_MGR(DefaultHandler)

DefaultHandler::DefaultHandler()
{

	REGIST_HANDLER(MSG::PING_REQ, PingReq);
	REGIST_HANDLER(MSG::CLOSED, Closed);
}


bool DefaultHandler::execute(SessionPtr session_, unsigned short id_, char * buff_, unsigned short len_)
{
	if (id_ >= MAX_MSG_ID || _table[id_] == NULL)
		return false;

	return _table[id_](session_, buff_, len_);
}

DEFINE_HANDLER(DefaultHandler, SessionPtr, PingReq)
{
	MSG::PingReq req;
	req.ParseFromArray(body_, len_);
	LOG(L_INFO_, "recv packet", "key", req.session_key(), "time", DateTime::getCurrentDateTime().formatLocal());

	// find user by session key
	auto user = UserManager::getUserManager()->find(req.session_key());
	if (user == nullptr)
	{
		MSG::PongAns ans;
		ans.set_err(MSG::ERR_SESSIONKEY_FAIL);
		session_->SendPacket(MSG::PONG_ANS, &ans);
		return true;
	}

	// update session key time out
	user->setPingTime(DateTime::GetTickCount() + 1000 * 1000 * 60);
	if (user->getSession() == nullptr || user->getSession() != session_)
	{
		user->setSession(session_);
	}

	MSG::PongAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	session_->SendPacket(MSG::PONG_ANS, &ans);
	return true;
}

DEFINE_HANDLER(DefaultHandler, SessionPtr, Closed)
{
	LOG(L_INFO_, " ");

	auto user = UserManager::getUserManager()->find(session_.get());
	if (user != nullptr)
	{
		//UserManager::getUserManager()->remove(session_.get());
		user->setSession(nullptr);
	}

	session_->disconnect();
	return true;
}


}