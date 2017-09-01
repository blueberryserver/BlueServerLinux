#pragma once
#include <unordered_set>
#include <mutex>
#include <memory>
#include "../BlueCore/Macro.h"
#include "cpp/common.pb.h"

#include "../BlueCore/RedisClient.h"
namespace BLUE_BERRY
{

class ChatRoom
{
public:
	ChatRoom(const char* name_);
	virtual ~ChatRoom();

	void enter(const SessionPtr& session_);
	void leave(const SessionPtr& session_);

	void publishChat(MSG::ChatData_& chat_);
	void publishEnter(uint64_t uid_, const char* name_);
	void publishLeave(uint64_t uid_, const char* name_);


	void msgProc(_RedisReply reply_);

	uint64_t getRId() { return _rid; }
	std::string& getName() { return _name; }
	std::string& getGreeting() { return _greeting; }

	void broadcastPacket(short id_, google::protobuf::Message* msg_);

	void messageParser(json11::Json & json_);

private:
	std::recursive_mutex _mtx;
	std::unordered_set<SessionPtr> _sessions;
	uint64_t _rid;
	std::string _name;
	std::string _greeting;

	std::vector<MSG::ChatData_> _chatDatas;

	RedisClientPtr _redis;
	static uint64_t _genRoomId;
};

DECLARE_SMART_PTR(ChatRoom)

}