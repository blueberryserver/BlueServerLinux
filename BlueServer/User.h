#pragma once
#include "../BlueCore/Macro.h"
#include "../BlueCore/json11.hpp"
#include "../BlueCore/DateTime.h"

#include "cpp/common.pb.h"

using namespace json11;
namespace BLUE_BERRY
{
FOWARD_DECLARE(Session)

class User : public JsonValue
{
public:
	User();
	User(const MSG::UserData_& data_);
	User(const Json& data_);
	virtual ~User();

public:
	MSG::UserData_ getData();

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

	Json to_json() const
	{
		Json::object jObj;
		jObj["uid"] = std::to_string(_data.uid());
		jObj["name"] = _data.name();
		jObj["did"] = _data.did();
		jObj["platform"] = std::to_string(_data.platform());

		jObj["login_date"] = _data.login_date();
		jObj["logout_date"] = _data.logout_date();
		jObj["reg_date"] = _data.reg_date();
		jObj["vc1"] = std::to_string(_data.vc1());
		jObj["vc2"] = std::to_string(_data.vc2());
		jObj["vc3"] = std::to_string(_data.vc3());

		return Json(jObj);
	}

	// set get function
	void setSession(const SessionPtr& session_) { _session = session_; }
	SessionPtr getSession() { return _session; }

	void setSessionKey(const std::string& key_) { _sessionKey = key_; }
	std::string getSessionKey() { return _sessionKey; }

	int64_t getPingTime() { return _pingTime; }
	void setPingTime(const int64_t time_) { _pingTime = time_; }
public:
	// session key
	std::string _sessionKey;

	// user data
	MSG::UserData_ _data;

	// session
	SessionPtr _session;

	// ping time
	int64_t _pingTime;
};

DECLARE_SMART_PTR(User)

}