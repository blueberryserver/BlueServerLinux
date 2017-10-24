#include "User.h"
#include "../BlueCore/Logger.h"
#include "../BlueCore/Session.h"
#include "../BlueCore/RedisClient.h"

#include "DBQueryUser.h"
#include "DBQueryChar.h"
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
	auto key = std::hash<std::string>()(toJson(data_).dump());
	_sessionKey = std::to_string(key);

}

User::User(const Json& data_)
	: _pingTime(DateTime::GetTickCount() + std::chrono::duration_cast<_microseconds>(_minutes(2)).count())
{
	google::protobuf::util::JsonStringToMessage(data_.dump(), &_data);

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
		it->publishLeave(_data.uid(), _data.name().c_str());
	}


	// cached data save
	RedisClientPtr client;
	std::future<_RedisReply> futureReply;
	std::string jsonStr;
	toJson(_data).dump(jsonStr);

	if (true == client->hset("blue_server.UserData.json", std::to_string(_data.uid()).c_str(), jsonStr.c_str(), std::ref(futureReply)))
	{
		auto reply = futureReply.get();
		LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.json", "reply", reply);
	}

	//auto hSetPostJobJson = LamdaToFuncObj([](_RedisReply reply_) -> void {
	//	LOG(L_INFO_, "Redis", "hset", "blue_server.UserData.json", "reply", reply_);
	//});
	//SyncJobManager::getSyncJobManager()->addJob(keyHGetJon, makePostJobStatic(hSetPostJobJson), nullptr);

	// db save
	{
		DBQueryUser query;
		query.setData(_data);
		if (query.updateData() == false)
		{
			// fail log
			LOG(L_ERROR_, "UserData update fail" );
		}
	}

	// character
	{
		DBQueryChar query;
		std::vector<MSG::CharData_> chars;
		for (auto i = 0; i < _data.chars_size(); i++)
		{
			auto charData = _data.chars(i);
			chars.push_back(charData);
		}
		query.setData(chars);
		if (query.updateData() == false)
		{
			// fail log
			LOG(L_ERROR_, "CharData update fail");
		}
	}
}

void User::joinRoom(ChatRoomPtr room_)
{
	_rooms.insert(room_);
	room_->enter(_session);
	room_->publishEnter(_data.uid(), _data.name().c_str());
}

void User::leaveRoom(ChatRoomPtr room_)
{
	_rooms.erase(room_);
	room_->leave(_session);
	room_->publishLeave(_data.uid(), _data.name().c_str());
}

void User::chatRoom(MSG::ChatData_& data_)
{
	for (auto it : _rooms)
	{
		it->publishChat(data_);
	}
}

}