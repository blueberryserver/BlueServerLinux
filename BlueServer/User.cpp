#include "User.h"
#include "../BlueCore/Logger.h"

namespace BLUE_BERRY
{


User::User()
{
}

User::User(const MSG::UserData_& data_)
	: _data(data_), _pingTime(DateTime::GetTickCount() + /*_minutes(1).count()*/ 1000* 1000*60)	// 60 second
{
	auto key = std::hash<std::string>()(to_json().dump());
	_sessionKey = std::to_string(key);

}

User::User(const Json& data_)
	: _pingTime(DateTime::GetTickCount() + /*_minutes(1).count()*/ 1000 * 1000 * 60)
{
	_data.set_uid( std::atoll(data_["uid"].string_value().c_str()) );
	_data.set_name( data_["name"].string_value() );
	_data.set_did( data_["did"].string_value() );

	_data.set_platform(static_cast<MSG::PlatForm>( std::atoi(data_["platform"].string_value().c_str())));
	_data.set_login_date(data_["login_date"].string_value());
	_data.set_logout_date(data_["logout_date"].string_value());
	_data.set_reg_date(data_["reg_date"].string_value());

	_data.set_vc1(std::atoi(data_["vc1"].string_value().c_str()));
	_data.set_vc2(std::atoi(data_["vc2"].string_value().c_str()));
	_data.set_vc3(std::atoi(data_["vc3"].string_value().c_str()));

	auto key = std::hash<std::string>()(data_.dump());
	_sessionKey = std::to_string(key);
}


User::~User()
{
	LOG(L_INFO_, " ");
}

MSG::UserData_ User::getData()
{
	return _data;
}

}