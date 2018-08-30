#include <vector>
#include <google/protobuf/util/json_util.h>
#include "../BlueCore/Session.h"
#include "../BlueCore/json11.hpp"
#include "cpp/chat.pb.h"
#include "ChatRoom.h"

namespace BLUE_BERRY
{

uint64_t ChatRoom::_genRoomId = 0;
void ChatRoom::msgProc(_RedisReply reply_)
{
	LOG(L_INFO_, "Redis", "room", _name, "rid", (double)_rid, "reply", reply_);

	// string -> json
	auto jsonData = reply_._array;
	if (jsonData.empty() || strcmp(jsonData[0]._string.c_str(), "message" ) != 0 ) return;


	std::string str;
	jsonData[2].dump(str);

	// string - >json
	std::string err;
	auto jsonMessage = json11::Json::parse(str, err);
	//LOG(L_INFO_, "Redis", "message", jsonMessage);

	messageParser(jsonMessage);
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
	LOG(L_INFO_, " ", "room", _name, "rid", (double)_rid);
	_redis->setChannelMsgJob(makePostJob(this, &ChatRoom::msgProc));
	_redis->subscribe(std::vector<std::string>({ _name, }));

}


ChatRoom::~ChatRoom()
{
	LOG(L_INFO_, " ", "room", _name, "rid", (double)_rid);
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


void ChatRoom::publishEnter(uint64_t uid_, const char * name_)
{
	LOG(L_INFO_, " ", "room", _name, "uid", (double)uid_, "name", name_);
	// json format
	Json::object user_data;
	user_data["uid"] = Json((double)uid_);
	user_data["name"] = name_;

	Json::object data;
	data["type"] = "enter";
	data["userData"] = user_data;

	std::string jsonDump;
	Json(data).dump(jsonDump);
	auto key = _redis->publish(_name.c_str(), jsonDump.c_str());
	auto publishFunc = CapturedLamdaToFuncObj([=](_RedisReply reply_) -> void {
		LOG(L_INFO_, "Redis", "publish", _name, "reply", reply_);
	});

	SyncJobManager::getSyncJobManager()->addJob(key, makePostJobStatic(publishFunc), nullptr);
}

void ChatRoom::publishLeave(uint64_t uid_, const char * name_)
{
	LOG(L_INFO_, " ", "room", _name, "uid", (double)uid_, "name", name_);
	// json format
	Json::object user_data;
	user_data["uid"] = Json((double)uid_);
	user_data["name"] = name_;

	Json::object data;
	data["type"] = "leave";
	data["userData"] = user_data;


	std::string jsonDump;
	Json(data).dump(jsonDump);
	auto key = _redis->publish(_name.c_str(), jsonDump.c_str());
	auto publishFunc = CapturedLamdaToFuncObj([=](_RedisReply reply_) -> void {
		LOG(L_INFO_, "Redis", "publish", _name, "reply", reply_);
	});

	SyncJobManager::getSyncJobManager()->addJob(key, makePostJobStatic(publishFunc), nullptr);
}

void ChatRoom::publishChat(MSG::ChatData_ & chat_)
{
	std::string out;
	google::protobuf::util::MessageToJsonString(chat_, &out);

	std::string err;
	auto chatData = json11::Json::parse(out, err);

	Json::object data;
	data["type"] = "chat";
	data["chatData"] = chatData;

	std::string jsonDump;
	Json(data).dump(jsonDump);
	auto key = _redis->publish(_name.c_str(), jsonDump.c_str());
	auto publishFunc = CapturedLamdaToFuncObj([=](_RedisReply reply_) -> void {
		LOG(L_INFO_, "Redis", "publish", _name, "reply", reply_);
	});

	SyncJobManager::getSyncJobManager()->addJob(key, makePostJobStatic(publishFunc), nullptr);
}



void ChatRoom::messageParser(json11::Json & json_)
{
	auto type = json_["type"].string_value();
	if (type.compare("chat") == 0)
	{
		auto chatData = json_["chatData"];

		// chat
		MSG::ChatData_ data;
		google::protobuf::util::JsonStringToMessage(chatData.dump(), &data);
		_chatDatas.push_back(data);

		MSG::ChatNot notify;
		auto chat = notify.mutable_chats();
		notify.set_type(MSG::ChatType::CHAT_ROOM);
		chat->CopyFrom(data);

		// session broad casting
		broadcastPacket(MSG::CHAT_NOT, &notify);

	}
	else if (type.compare("enter") == 0)
	{
		// enter
		auto userData = json_["userData"];
		auto uid = (uint64_t)userData["uid"].number_value();
		auto name = userData["name"].string_value();

		MSG::EnterChatRoomNot notify;
		notify.set_uid(uid);
		notify.set_name(name);
		broadcastPacket(MSG::ENTERCHATROOM_NOT, &notify);
	}
	else
	{
		// leave
		auto userData = json_["userData"];
		auto uid = (uint64_t)userData["uid"].number_value();
		auto name = userData["name"].string_value();

		MSG::LeaveChatRoomNot notify;
		notify.set_uid(uid);
		notify.set_name(name);
		broadcastPacket(MSG::LEAVECHATROOM_NOT, &notify);
	}
}

}