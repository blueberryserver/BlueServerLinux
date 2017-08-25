#pragma once
#include "../BlueCore/MsgHandler.h"
#include "../BlueCore/Session.h"
#include "cpp/common.pb.h"
#include "DefaultHandler.h"


namespace BLUE_BERRY
{

class LoginHandler : public DefaultHandler
{
public:
	LoginHandler();

public:
	DECLARE_HANDLER(SessionPtr, LoginReq);
	DECLARE_HANDLER(SessionPtr, RegistReq);

	// db query proc function
	static void dbSelectUser(const SessionPtr session_, const std::string name_);
	static void dbInsertUser(SessionPtr session_, MSG::UserData_ data_);

	// redis query proc function
	static void redisSelectUser(SessionPtr session_, std::string name_);

public:
	DECLARE_MGR(LoginHandler)
};

EXTERN_MGR(LoginHandler)

}