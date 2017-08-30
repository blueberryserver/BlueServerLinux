#include <vector>
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
	auto jsonChatData = json11::Json::parse(str, err);
	LOG(L_INFO_, "Redis", "message", jsonChatData);

	MSG::ChatData_ data;
	data.set_uid((uint64_t)jsonChatData["uid"].number_value());
	data.set_name(jsonChatData["name"].string_value());
	data.set_group_name(jsonChatData["group_name"].string_value());

	data.set_language(jsonChatData["language"].string_value());
	data.set_chat(jsonChatData["chat"].string_value());
	data.set_reg_date((uint64_t)jsonChatData["reg_date"].number_value());
	_chatDatas.push_back(data);

	MSG::ChatNot notify;
	auto chat = notify.mutable_chats();
	notify.set_type(MSG::ChatType::CHAT_CHANNEL);
	chat->CopyFrom(data);
	// session broad casting
	for (auto it : _sessions)
	{
		(*it).SendPacket(MSG::CHAT_NOT, &notify);
	}
}

ChatChannel::ChatChannel(const char * counrty_)
	: _country(counrty_)
{
	_redis->subscribe(std::vector<std::string>({ _country, }));
	_redis->setChannelMsgJob(makePostJob(this, &ChatChannel::channelMsgProc));
}

ChatChannel::~ChatChannel()
{
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

void ChatChannel::sendChat(MSG::ChatData_ & chat_)
{
	// json format
	Json::object jObj;
	jObj["uid"] = Json((double)chat_.uid());
	jObj["name"] = chat_.name();
	jObj["group_name"] = chat_.group_name();
	jObj["language"] = chat_.language();
	jObj["chat"] = chat_.chat();
	jObj["reg_date"] = Json((double)chat_.reg_date());

	std::string jsonDump;
	Json(jObj).dump(jsonDump);
	auto key = _redis->publish(_country.c_str(), jsonDump.c_str());
	auto publishFunc = LamdaToFuncObj([=](_RedisReply reply_) -> void {
		LOG(L_INFO_, "Redis", "publish", _country, "reply", reply_);
	});

	SyncJobManager::getSyncJobManager()->addJob(key, makePostJobStatic(publishFunc), nullptr);
}


}