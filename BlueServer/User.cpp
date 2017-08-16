#include "User.h"

namespace BLUE_BERRY
{


User::User()
{
}

User::User(const MSG::UserData_ & data_)
	: _uId(data_.uid()), _name(data_.name()), _deviceId(data_.did())
{
}

User::User(const Json& data_)
{
	_uId = std::atoll(data_["uid"].string_value().c_str());
	_name = data_["name"].string_value();
	_deviceId = data_["did"].string_value();
}


User::~User()
{
}

MSG::UserData_ User::toProtoSerialize()
{
	auto data = MSG::UserData_();
	data.set_uid(_uId);
	data.set_name(_name);
	data.set_did(_deviceId);
	return data;
}

}