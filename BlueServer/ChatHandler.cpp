#include "ChatHandler.h"
#include "cpp/login.pb.h"
#include "cpp/chat.pb.h"

#include "../BlueCore/DateTime.h"
#include "../BlueCore/Packet.h"
#include "../BlueCore/Logger.h"


#include "UserManager.h"

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
	return true;
}

DEFINE_HANDLER(ChatHandler, SessionPtr, CreateChatRoomReq)
{
	MSG::CreateChatRoomReq req;
	req.ParseFromArray(body_, len_);
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
	return true;
}

DEFINE_HANDLER(ChatHandler, SessionPtr, LeaveChatRoomReq)
{
	MSG::LeaveChatRoomReq req;
	req.ParseFromArray(body_, len_);
	return true;
}

}