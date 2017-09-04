#pragma once
#include <unordered_set>
#include <mutex>
#include <memory>

#include "../BlueCore/Macro.h"
#include "../BlueCore/RedisClient.h"
#include "../BlueCore/MemoryPool.h"

#include "cpp/common.pb.h"
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

	void publishChat(MSG::ChatData_& chat_);
	void publishEnter(uint64_t uid_, const char* name_);
	void publishLeave(uint64_t uid_, const char* name_);

	void channelMsgProc(_RedisReply reply_);

	void broadcastPacket(short id_, google::protobuf::Message* msg_);

	void messageParser(json11::Json& json_);

DECLARE_NEW_DELETE
private:

	std::recursive_mutex _mtx;
	std::unordered_set<SessionPtr> _sessions;
	std::string _country;

	std::vector<MSG::ChatData_> _chatDatas;

	RedisClientPtr _redis;

};

DECLARE_SMART_PTR(ChatChannel)

}