#pragma once
#include <unordered_set>
#include "../BlueCore/Macro.h"
#include "../BlueCore/json11.hpp"
#include "../BlueCore/DateTime.h"
#include "../BlueCore/MemoryPool.h"

#include "cpp/common.pb.h"

using namespace json11;
namespace BLUE_BERRY
{
FOWARD_DECLARE(Session)
FOWARD_DECLARE(ChatRoom)

class User : public JsonValue
{
public:
	User();
	User(const MSG::UserData_& data_);
	User(const Json& data_);
	virtual ~User();

public:
	MSG::UserData_& getData()
	{
		return _data;
	}

	virtual Json::Type type() const
	{
		return Json::OBJECT;
	}

	virtual bool equals(const JsonValue * other) const
	{
		return false;
	}

	virtual bool less(const JsonValue * other) const
	{
		return false;
	}

	virtual void dump(std::string &out) const
	{
		out = to_json().dump();
	}

	Json to_json() const;

	// set get function
	void setSession(const SessionPtr& session_) { _session = session_; }
	SessionPtr getSession() { return _session; }

	void setSessionKey(const std::string& key_) { _sessionKey = key_; }
	std::string getSessionKey() { return _sessionKey; }

	int64_t getPingTime() { return _pingTime; }
	void setPingTime(const int64_t time_) { _pingTime = time_; }

	void joinRoom(ChatRoomPtr room_);
	void leaveRoom(ChatRoomPtr room_);
	void chatRoom(MSG::ChatData_& data_);

DECLARE_NEW_DELETE
public:
	// session key
	std::string _sessionKey;

	// user data
	MSG::UserData_ _data;

	// session
	SessionPtr _session;

	// ping time
	int64_t _pingTime;

	// join room
	std::unordered_set<ChatRoomPtr> _rooms;
};

DECLARE_SMART_PTR(User)

}