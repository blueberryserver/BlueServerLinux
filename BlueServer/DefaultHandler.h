
#pragma once
#include "../BlueCore/MsgHandler.h"
#include "../BlueCore/Session.h"
#include "cpp/common.pb.h"


namespace BLUE_BERRY
{
class DefaultHandler : public MsgHandler<Session, Packet>
{
public:
	DefaultHandler();

	virtual bool execute(SessionPtr session_, const Packet& packet_) override;

public:

	DECLARE_HANDLER(SessionPtr, PingReq);
	DECLARE_HANDLER(SessionPtr, Closed);
public:
	DECLARE_MGR(DefaultHandler)
};
EXTERN_MGR(DefaultHandler)

}