#pragma once
#include "../BlueCore/MsgHandler.h"
#include "../BlueCore/Session.h"
#include "cpp/common.pb.h"


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
	DECLARE_HANDLER(SessionPtr, RegistReq);

	DECLARE_HANDLER(SessionPtr, Closed);

	// db query proc function
	static void dbSelectUser(const SessionPtr session_, const std::string name_);
	static void dbInsertUser(SessionPtr session_, MSG::UserData_ data_);

	// redis query proc function
	static void redisSelectUser(SessionPtr session_, std::string name_);
};

}