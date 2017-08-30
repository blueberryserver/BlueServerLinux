#include "ChatHandler.h"
#include "cpp/login.pb.h"
#include "cpp/chat.pb.h"

#include "../BlueCore/DateTime.h"
#include "../BlueCore/Packet.h"
#include "../BlueCore/Logger.h"
#include "../BlueCore/SyncJobHelper.h"
#include "../BlueCore/RedisClient.h"


#include "UserManager.h"
#include "ChatChannelManager.h"
#include "ChatRoomManager.h"

namespace BLUE_BERRY
{

DEFINE_MGR(ChatHandler)

ChatHandler::ChatHandler() : DefaultHandler()
{
	REGIST_HANDLER(MSG::CHAT_REQ, ChatReq);
	REGIST_HANDLER(MSG::CREATECHATROOM_REQ, CreateChatRoomReq);
	REGIST_HANDLER(MSG::INVITECHATROOM_REQ, InviteChatRoomReq);
	REGIST_HANDLER(MSG::ENTERCHATROOM_REQ, EnterChatRoomReq);
	REGIST_HANDLER(MSG::LEAVECHATROOM_REQ, LeaveChatRoomReq);

}

DEFINE_HANDLER(ChatHandler, SessionPtr, ChatReq)
{
	MSG::ChatReq req;
	req.ParseFromArray(body_, len_);

	auto user = UserManager::getUserManager()->find(session_.get());
	if (user == nullptr)
	{
		MSG::ChatAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::CHAT_ANS, &ans);
		return true;
	}

	// update time out
	user->setPingTime(DateTime::GetTickCount() + std::chrono::duration_cast<_microseconds>(_minutes(2)).count());

	// find chat channel
	auto channel = ChatChannelManager::getChatChannelManager()->findChannel(user->getData().group_name().c_str());
	if (channel == nullptr)
	{
		MSG::ChatAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::CHAT_ANS, &ans);
		return true;
	}

	// create chat data
	MSG::ChatData_ data;
	data.set_uid(user->getData().uid());
	data.set_name(user->getData().name());
	data.set_group_name(user->getData().group_name());
	data.set_language(user->getData().language());
	data.set_chat(req.msg());
	data.set_reg_date(DateTime::GetTickCountM());

	// broadcast chat channel
	channel->sendChat(data);


	// answer
	MSG::ChatAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	session_->SendPacket(MSG::CHAT_ANS, &ans);
	return true;
}

DEFINE_HANDLER(ChatHandler, SessionPtr, CreateChatRoomReq)
{
	MSG::CreateChatRoomReq req;
	req.ParseFromArray(body_, len_);

	auto user = UserManager::getUserManager()->find(session_.get());
	if (user == nullptr)
	{
		MSG::CreateChatRoomAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::CREATECHATROOM_ANS, &ans);
		return true;
	}

	// update time out
	user->setPingTime(DateTime::GetTickCount() + std::chrono::duration_cast<_microseconds>(_minutes(2)).count());

	auto room = ChatRoomManager::getChatRoomManager()->findRoom(req.name());
	if (room == nullptr)
	{
		MSG::CreateChatRoomAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::CREATECHATROOM_ANS, &ans);
		return true;
	}

	// enter room
	room->enter(session_);

	// join room
	user->joinRoom(room);

	MSG::CreateChatRoomAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	ans.set_rkey(room->getName());
	ans.set_rid(room->getRId());
	session_->SendPacket(MSG::CREATECHATROOM_ANS, &ans);
	return true;
}

DEFINE_HANDLER(ChatHandler, SessionPtr, InviteChatRoomReq)
{
	MSG::InviteChatRoomReq req;
	req.ParseFromArray(body_, len_);
	return true;
}

DEFINE_HANDLER(ChatHandler, SessionPtr, EnterChatRoomReq)
{
	MSG::EnterChatRoomReq req;
	req.ParseFromArray(body_, len_);

	auto user = UserManager::getUserManager()->find(session_.get());
	if (user == nullptr)
	{
		MSG::CreateChatRoomAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::CREATECHATROOM_ANS, &ans);
		return true;
	}
	return true;
}

DEFINE_HANDLER(ChatHandler, SessionPtr, LeaveChatRoomReq)
{
	MSG::LeaveChatRoomReq req;
	req.ParseFromArray(body_, len_);
	return true;
}

}