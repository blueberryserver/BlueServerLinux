#pragma once
#include <unordered_set>
#include <mutex>
#include <memory>

#include "cpp/common.pb.h"

#include "../BlueCore/Macro.h"
#include "../BlueCore/RedisClient.h"

namespace BLUE_BERRY
{
FOWARD_DECLARE(Session)

class ChatChannel
{
public:
	ChatChannel( const char* counrty_);
	virtual ~ChatChannel();

	void enterChannel(const SessionPtr& session_);
	void leaveChannel(const SessionPtr& session_);

	void sendChat(MSG::ChatData_& chat_);

	void channelMsgProc(_RedisReply reply_);
private:

	std::recursive_mutex _mtx;
	std::unordered_set<SessionPtr> _sessions;
	std::string _country;

	std::vector<MSG::ChatData_> _chatDatas;

	RedisClientPtr _redis;

};

DECLARE_SMART_PTR(ChatChannel)

}