#include "DefaultHandler.h"
#include "cpp/login.pb.h"

#include "../BlueCore/DateTime.h"
#include "../BlueCore/Packet.h"
#include "../BlueCore/Logger.h"


#include "UserManager.h"
#include "ChatChannelManager.h"

namespace BLUE_BERRY
{

DEFINE_MGR(DefaultHandler)

DefaultHandler::DefaultHandler()
{
	REGIST_HANDLER(MSG::PING_REQ, PingReq);
	REGIST_HANDLER(MSG::CLOSED, Closed);
}


bool DefaultHandler::execute(SessionPtr session_, const Packet& packet_)
{
	auto id = packet_.getId();
	if ( id >= MAX_MSG_ID || _table[id] == NULL)
		return false;

	return _table[id](session_, packet_.getData(), packet_.getDataLength());
}

DEFINE_HANDLER(DefaultHandler, SessionPtr, PingReq)
{
	MSG_PARSING(MSG::PingReq, body_, len_);
	LOG(L_INFO_, "recv packet", "key", req.sessionkey(), "time", DateTime::getCurrentDateTime().formatLocal());

	// find user by session key
	auto user = UserManager::getUserManager()->find(req.sessionkey());
	if (user == nullptr)
	{
		MSG::PongAns ans;
		ans.set_err(MSG::ERR_SESSIONKEY_FAIL);
		session_->SendPacket(MSG::PONG_ANS, &ans);
		return true;
	}

	// update session key time out
	user->setPingTime(DateTime::GetTickCount() + duration_cast<microseconds>(minutes(2)).count());
	if (user->getSession() == nullptr || user->getSession() != session_)
	{
		user->setSession(session_);
		auto channel = ChatChannelManager::getChatChannelManager()->findChannel(user->getData()->groupname().c_str());
		channel->enterChannel(session_);
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
		auto channel = ChatChannelManager::getChatChannelManager()->findChannel(user->getData()->groupname().c_str());
		channel->leaveChannel(session_);

		//UserManager::getUserManager()->remove(session_.get());
		user->setSession(nullptr);
	}

	session_->disconnect();
	return true;
}


}