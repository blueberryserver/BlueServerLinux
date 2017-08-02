#pragma once
#include "../BlueCore/MsgHandler.h"
#include "../BlueCore/Session.h"

namespace BLUE_BERRY
{

class LoginHandler : public MsgHandler<Session>
{
public:
	LoginHandler();

	virtual bool execute(SessionPtr session_, unsigned short id_, char* buff_, unsigned short len_) override;

public:
	DECLARE_HANDLER(SessionPtr, LoginReq);
	DECLARE_HANDLER(SessionPtr, PingReq);

	static void SendPacket(SessionPtr session_, short id_, char* buffer_, short len_);
};

}