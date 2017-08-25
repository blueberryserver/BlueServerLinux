#pragma once
#include "../BlueCore/MsgHandler.h"
#include "../BlueCore/Session.h"
#include "cpp/common.pb.h"
#include "DefaultHandler.h"


namespace BLUE_BERRY
{
class ChatHandler : public DefaultHandler
{
public:
	ChatHandler();

public:
	DECLARE_HANDLER(SessionPtr, ChatReq);
	DECLARE_HANDLER(SessionPtr, CreateChatRoomReq);
	DECLARE_HANDLER(SessionPtr, InviteChatRoomReq);
	DECLARE_HANDLER(SessionPtr, EnterChatRoomReq);
	DECLARE_HANDLER(SessionPtr, LeaveChatRoomReq);

public:
	DECLARE_MGR(ChatHandler)
};
EXTERN_MGR(ChatHandler)
}