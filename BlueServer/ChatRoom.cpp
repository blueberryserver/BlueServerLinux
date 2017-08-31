#include <vector>
#include "../BlueCore/Session.h"
#include "../BlueCore/json11.hpp"
#include "cpp/chat.pb.h"
#include "ChatRoom.h"

namespace BLUE_BERRY
{

uint64_t ChatRoom::_genRoomId = 0;
void ChatRoom::msgProc(_RedisReply reply_)
{
	LOG(L_INFO_, "Redis", "room", _name, "reply", reply_);

	// string -> json
	auto jsonData = reply_._array;
	if (jsonData.empty() || strcmp(jsonData[0]._string.c_str(), "message" ) != 0 ) return;


	std::string str;
	jsonData[2].dump(str);

	// string - >json
	std::string err;
	auto jsonChatData = json11::Json::parse(str, err);
	LOG(L_INFO_, "Redis", "message", jsonChatData);

	// json -> proto data
	MSG::ChatData_ data;
	data.set_uid((uint64_t)jsonChatData["uid"].number_value());
	data.set_name(jsonChatData["name"].string_value());
	data.set_groupname(jsonChatData["groupname"].string_value());

	data.set_language(jsonChatData["language"].string_value());
	data.set_chat(jsonChatData["chat"].string_value());
	data.set_regdate((uint64_t)jsonChatData["regdate"].number_value());
	_chatDatas.push_back(data);

	MSG::ChatNot notify;
	auto chat = notify.mutable_chats();
	notify.set_type(MSG::ChatType::CHAT_ROOM);
	chat->CopyFrom(data);

	// session broad casting
	broadcastPacket(MSG::CHAT_NOT, &notify);
}

void ChatRoom::broadcastPacket(short id_, google::protobuf::Message* msg_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	for (auto it : _sessions)
	{
		(*it).SendPacket(id_, msg_);
	}
}

ChatRoom::ChatRoom(const char* name_)
	:_rid(_genRoomId++), _name(name_)
{
	_redis->subscribe(std::vector<std::string>({ _name, }));
	_redis->setChannelMsgJob(makePostJob(this, &ChatRoom::msgProc));

}


ChatRoom::~ChatRoom()
{
	_redis->unsubscribe(std::vector<std::string>({ _name, }));
}


void ChatRoom::enter(const SessionPtr& session_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	_sessions.insert(session_);
}

void ChatRoom::leave(const SessionPtr& session_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	_sessions.erase(session_);
}

void ChatRoom::sendChat(MSG::ChatData_ & chat_)
{
	// json format
	Json::object jObj;
	jObj["uid"] = Json((double)chat_.uid());
	jObj["name"] = chat_.name();
	jObj["groupName"] = chat_.groupname();
	jObj["language"] = chat_.language();
	jObj["chat"] = chat_.chat();
	jObj["regDate"] = Json((double)chat_.regdate());

	std::string jsonDump;
	Json(jObj).dump(jsonDump);
	auto key = _redis->publish(_name.c_str(), jsonDump.c_str());
	auto publishFunc = LamdaToFuncObj([=](_RedisReply reply_) -> void {
		LOG(L_INFO_, "Redis", "publish", _name, "reply", reply_);
	});

	SyncJobManager::getSyncJobManager()->addJob(key, makePostJobStatic(publishFunc), nullptr);
}


}