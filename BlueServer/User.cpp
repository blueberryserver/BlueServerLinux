#include "User.h"
#include <google/protobuf/util/json_util.h>
#include "../BlueCore/Logger.h"
#include "../BlueCore/Session.h"
#include "../BlueCore/RedisClient.h"

#include "DBQueryUser.h"
#include "ChatRoom.h"
#include "cpp/chat.pb.h"

namespace BLUE_BERRY
{


User::User()
{
}

User::User(const MSG::UserData_& data_)
	: _data(data_), _pingTime(DateTime::GetTickCount() + std::chrono::duration_cast<_microseconds>(_minutes(2)).count())	// 60 second
{
	auto key = std::hash<std::string>()(to_json().dump());
	_sessionKey = std::to_string(key);

}

User::User(const Json& data_)
	: _pingTime(DateTime::GetTickCount() + std::chrono::duration_cast<_microseconds>(_minutes(2)).count())
{
	google::protobuf::util::JsonStringToMessage(data_.dump(), &_data);

	/*
	_data.set_uid( (uint64_t)data_["uid"].number_value());
	_data.set_name( data_["name"].string_value() );
	_data.set_did( data_["did"].string_value() );

	_data.set_platform(static_cast<MSG::PlatForm>( std::atoi(data_["platform"].string_value().c_str())));
	_data.set_logindate(data_["loginDate"].string_value());
	_data.set_logoutdate(data_["logoutDate"].string_value());
	_data.set_regdate(data_["regDate"].string_value());

	_data.set_vc1(std::atoi(data_["vc1"].string_value().c_str()));
	_data.set_vc2(std::atoi(data_["vc2"].string_value().c_str()));
	_data.set_vc3(std::atoi(data_["vc3"].string_value().c_str()));

	_data.set_groupname(data_["groupName"].string_value());
	_data.set_language(data_["language"].string_value());
	*/

	auto key = std::hash<std::string>()(data_.dump());
	_sessionKey = std::to_string(key);
}


User::~User()
{
	LOG(L_INFO_, " ");

	// check connected session
	if (_session != nullptr)
	{
		// connected destroy
		_session->disconnect();
	}

	// logout time
	auto now = DateTime::getCurrentDateTime().formatLocal();
	_data.set_logoutdate(now.c_str());


	// leave chat room
	for (auto it : _rooms)
	{
		it->leave(_session);
	}


	// cached data save
	RedisClientPtr client;
	std::string jsonStr;
	dump(jsonStr);
	auto keyHGetJon = client->hset("blue_server.UserData.json", std::to_string(_data.uid()).c_str(), jsonStr.c_str());
	auto hSetPostJobJson = LamdaToFuncObj([](_RedisReply reply_) -> void {
		LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.json", "reply", reply_);
	});
	SyncJobManager::getSyncJobManager()->addJob(keyHGetJon, makePostJobStatic(hSetPostJobJson), nullptr);


	// db save
	DBQueryUser query;
	query.setData(_data);
	if (query.updateData() == false)
	{
		// fail log
	}
}

void User::joinRoom(ChatRoomPtr room_)
{
	_rooms.insert(room_);

	MSG::EnterChatRoomNot notify;
	notify.set_uid(_data.uid());
	notify.set_name(_data.name());
	room_->broadcastPacket(MSG::ENTERCHATROOM_NOT, &notify);
}

void User::leaveRoom(ChatRoomPtr room_)
{
	_rooms.erase(room_);

	MSG::LeaveChatRoomNot notify;
	notify.set_uid(_data.uid());
	notify.set_name(_data.name());
	room_->broadcastPacket(MSG::LEAVECHATROOM_ANS, &notify);
}

Json User::to_json() const
{
	/*
	Json::object jObj;
	jObj["uid"] = Json((double)_data.uid());
	jObj["name"] = _data.name();
	jObj["did"] = _data.did();
	jObj["platform"] = std::to_string(_data.platform());

	jObj["login_date"] = _data.login_date();
	jObj["logout_date"] = _data.logout_date();
	jObj["reg_date"] = _data.reg_date();
	jObj["vc1"] = Json((int)_data.vc1());
	jObj["vc2"] = Json((int)_data.vc2());
	jObj["vc3"] = Json((int)_data.vc3());

	jObj["group_name"] = _data.group_name();
	jObj["language"] = _data.language();
	return Json(jObj);
	*/

	std::string out;
	google::protobuf::util::MessageToJsonString(_data, &out);
	//LOG(L_INFO_, "proto2json", "result", out);

	std::string err;
	return json11::Json::parse(out, err);	
}

}