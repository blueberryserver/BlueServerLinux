#pragma once
#include <string>
#include <vector>
namespace BLUE_BERRY
{

struct Schema{
	enum DataType {
		_tinyint,
		_smallint,
		_int,
		_bigint,
		_double,
		_float,
		_datetime,
		_varchar,
		_blob,
		_text,
	};

	std::string _name;
	DataType _dataType;
	std::string _property;


};
typedef std::vector<Schema> SCHEMA;

template< typename T >
class BlueORM
{
public:

	BlueORM()
	{
	}

	virtual ~BlueORM()
	{
	}

private:
	T _data;

	// db connect info

	// schema info
	SCHEMA _schema;
	/*
	`uid` bigint(20) unsigned NOT NULL AUTO_INCREMENT,
	`name` varchar(64) NOT NULL,
	`did` varchar(64) NOT NULL,
	`platform` int(11) NOT NULL,
	`loginDate` datetime DEFAULT NULL,
	`logoutDate` datetime DEFAULT NULL,
	`regDate` datetime DEFAULT NULL,
	`vc1` int(11) NOT NULL,
	`vc2` int(11) NOT NULL,
	`vc3` int(11) NOT NULL,
	`groupName` varchar(64) DEFAULT ' ',
	`language` varchar(64) DEFAULT ' ',
	*/
};

}