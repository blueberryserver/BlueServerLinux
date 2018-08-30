#include <vector>
#include <google/protobuf/util/json_util.h>
#include "../BlueCore/Session.h"
#include "../BlueCore/json11.hpp"

#include "ChatChannel.h"
#include "cpp/chat.pb.h"

namespace BLUE_BERRY
{

void ChatChannel::channelMsgProc(_RedisReply reply_)
{
	LOG(L_INFO_, "Redis", "channel", _country, "reply", reply_);

	// string -> json
	auto jsonData = reply_._array;
	if (jsonData.empty() || strcmp(jsonData[0]._string.c_str(), "message" ) != 0 ) return;

	// json -> proto data
	std::string str;
	jsonData[2].dump(str);

	std::string err;
	auto jsonMessage = json11::Json::parse(str, err);
	//LOG(L_INFO_, "Redis", "message", jsonMessage);

	messageParser(jsonMessage);
}

ChatChannel::ChatChannel(const char * counrty_)
	: _country(counrty_)
{
	LOG(L_INFO_, " ", "channel", _country);
	_redis->setChannelMsgJob(makePostJob(this, &ChatChannel::channelMsgProc));
	_redis->subscribe(std::vector<std::string>({ _country, }));
	
}

ChatChannel::~ChatChannel()
{
	LOG(L_INFO_, " ", "channel", _country);
	_redis->unsubscribe(std::vector<std::string>({ _country, }));
}

void ChatChannel::enterChannel(const SessionPtr& session_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	_sessions.insert(session_);
}

void ChatChannel::leaveChannel(const SessionPtr& session_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	_sessions.erase(session_);
}

void ChatChannel::publishEnter(uint64_t uid_, const char * name_)
{
	// json format
	Json::object user_data;
	user_data["uid"] = Json((double)uid_);
	user_data["name"] = name_;

	Json::object data;
	data["type"] = "enter";
	data["userData"] = user_data;

	std::string jsonDump;
	Json(data).dump(jsonDump);
	auto key = _redis->publish(_country.c_str(), jsonDump.c_str());
	auto publishFunc = CapturedLamdaToFuncObj([=](_RedisReply reply_) -> void {
		LOG(L_INFO_, "Redis", "publish", _country, "reply", reply_);
	});

	SyncJobManager::getSyncJobManager()->addJob(key, makePostJobStatic(publishFunc), nullptr);
}

void ChatChannel::publishLeave(uint64_t uid_, const char * name_)
{
	// json format
	Json::object user_data;
	user_data["uid"] = Json((double)uid_);
	user_data["name"] = name_;

	Json::object data;
	data["type"] = "leave";
	data["userData"] = user_data;


	std::string jsonDump;
	Json(data).dump(jsonDump);
	auto key = _redis->publish(_country.c_str(), jsonDump.c_str());
	auto publishFunc = CapturedLamdaToFuncObj([=](_RedisReply reply_) -> void {
		LOG(L_INFO_, "Redis", "publish", _country, "reply", reply_);
	});

	SyncJobManager::getSyncJobManager()->addJob(key, makePostJobStatic(publishFunc), nullptr);
}

void ChatChannel::publishChat(MSG::ChatData_ & chat_)
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
	auto key = _redis->publish(_country.c_str(), jsonDump.c_str());
	auto publishFunc = CapturedLamdaToFuncObj([=](_RedisReply reply_) -> void {
		LOG(L_INFO_, "Redis", "publish", _country, "reply", reply_);
	});

	SyncJobManager::getSyncJobManager()->addJob(key, makePostJobStatic(publishFunc), nullptr);
}


void ChatChannel::broadcastPacket(short id_, google::protobuf::Message* msg_)
{
	std::lock_guard<std::recursive_mutex> guard(_mtx);
	for (auto it : _sessions)
	{
		(*it).SendPacket(id_, msg_);
	}
}

void ChatChannel::messageParser(json11::Json & json_)
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
		notify.set_type(MSG::ChatType::CHAT_CHANNEL);
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