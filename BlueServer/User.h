#pragma once
#include <unordered_set>
#include <google/protobuf/util/json_util.h>
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

class User
{
public:
	User();
	User(const MSG::UserData_& data_);
	User(const Json& data_);
	virtual ~User();

public:
	MSG::UserData_* getData()
	{
		return &_data;
	}

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

template< typename T>
static const Json toJson( const T& t_)
{
	std::string out;
	google::protobuf::util::MessageToJsonString(t_, &out);

	std::string err;
	return json11::Json::parse(out, err);
}

}