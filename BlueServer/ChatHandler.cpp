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
	user->setPingTime(DateTime::GetTickCount() + duration_cast<microseconds>(minutes(2)).count());

	// create chat data
	MSG::ChatData_ data;
	data.set_uid(user->getData()->uid());
	data.set_name(user->getData()->name());
	data.set_groupname(user->getData()->groupname());
	data.set_language(user->getData()->language());
	data.set_chat(req.msg());
	data.set_regdate(DateTime::GetTickCountM());

	if (req.type() == MSG::CHAT_CHANNEL)
	{
		// find chat channel
		auto channel = ChatChannelManager::getChatChannelManager()->findChannel(user->getData()->groupname().c_str());
		if (channel == nullptr)
		{
			MSG::ChatAns ans;
			ans.set_err(MSG::ERR_ARGUMENT_FAIL);
			session_->SendPacket(MSG::CHAT_ANS, &ans);
			return true;
		}
		
		// broadcast chat channel
		channel->publishChat(data);
	}
	else if (req.type() == MSG::CHAT_ROOM)
	{
		user->chatRoom(data);
	}


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
	user->setPingTime(DateTime::GetTickCount() + duration_cast<microseconds>(minutes(2)).count());

	auto room = ChatRoomManager::getChatRoomManager()->findRoom(req.name());
	if (room == nullptr)
	{
		MSG::CreateChatRoomAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::CREATECHATROOM_ANS, &ans);
		return true;
	}

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

	auto user = UserManager::getUserManager()->find(session_.get());
	if (user == nullptr)
	{
		MSG::InviteChatRoomAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::INVITECHATROOM_ANS, &ans);
		return true;
	}

	auto room = ChatRoomManager::getChatRoomManager()->findRoom(req.rkey());
	if (room == nullptr)
	{
		MSG::InviteChatRoomAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::INVITECHATROOM_ANS, &ans);
		return true;
	}

	auto target = UserManager::getUserManager()->findByUid(req.targetuid());
	if (target == nullptr)
	{
		MSG::InviteChatRoomAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::INVITECHATROOM_ANS, &ans);
		return true;
	}

	// notify
	MSG::InviteChatRoomNot notify;
	notify.set_rid(req.rid());
	notify.set_rkey(req.rkey());
	notify.set_owneruid(user->getData()->uid());
	notify.set_ownername(user->getData()->name());
	target->getSession()->SendPacket(MSG::INVITECHATROOM_NOT, &notify);

	// answer
	MSG::InviteChatRoomAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	session_->SendPacket(MSG::INVITECHATROOM_ANS, &ans);
	return true;
}

DEFINE_HANDLER(ChatHandler, SessionPtr, EnterChatRoomReq)
{
	MSG::EnterChatRoomReq req;
	req.ParseFromArray(body_, len_);

	auto user = UserManager::getUserManager()->find(session_.get());
	if (user == nullptr)
	{
		MSG::EnterChatRoomAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::ENTERCHATROOM_ANS, &ans);
		return true;
	}

	auto room = ChatRoomManager::getChatRoomManager()->findRoom(req.rkey());
	if (room == nullptr)
	{
		MSG::EnterChatRoomAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::ENTERCHATROOM_ANS, &ans);
		return true;
	}

	// join room
	user->joinRoom(room);

	//
	MSG::EnterChatRoomAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	session_->SendPacket(MSG::ENTERCHATROOM_ANS, &ans);
	return true;
}

DEFINE_HANDLER(ChatHandler, SessionPtr, LeaveChatRoomReq)
{
	MSG::LeaveChatRoomReq req;
	req.ParseFromArray(body_, len_);

	auto user = UserManager::getUserManager()->find(session_.get());
	if (user == nullptr)
	{
		MSG::LeaveChatRoomAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::LEAVECHATROOM_ANS, &ans);
		return true;
	}

	auto room = ChatRoomManager::getChatRoomManager()->findRoom(req.rkey());
	if (room == nullptr)
	{
		MSG::LeaveChatRoomAns ans;
		ans.set_err(MSG::ERR_ARGUMENT_FAIL);
		session_->SendPacket(MSG::LEAVECHATROOM_ANS, &ans);
		return true;
	}

	// leave room
	user->leaveRoom(room);

	MSG::LeaveChatRoomAns ans;
	ans.set_err(MSG::ERR_SUCCESS);
	session_->SendPacket(MSG::LEAVECHATROOM_ANS, &ans);
	return true;
}

}