#pragma once
#include "../BlueCore/Macro.h"
#include "../BlueCore/json11.hpp"

#include "cpp/common.pb.h"

using namespace json11;
namespace BLUE_BERRY
{

class User : public JsonValue
{
public:
	User();
	User(const MSG::UserData_& data_);
	virtual ~User();

public:
	MSG::UserData_ toProtoSerialize();

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
		jObj["uid"] = std::to_string(_uId);
		jObj["name"] = _name;
		jObj["did"] = _deviceId;

		return Json(jObj);
	}

public:
	// unique id
	uint64_t _uId;
	// user name
	std::string _name;
	// device unique id
	std::string _deviceId;
};

DECLARE_SMART_PTR(User)

}