#pragma once
#include "json11.hpp"

using namespace json11;
namespace BLUE_BERRY
{

enum REPLY_TYPE
{
	REPY_NIL		= 0x00,
	REPY_STRING		= 0x01,	// +
	REPY_ERROR		= 0x02,	// -
	REPY_INTEGER	= 0x04,	// :
	REPY_ARRAY		= 0x08,	// *
	REPY_BULKSTRING	= 0x10, // $
};

// redis server reply data
class _RedisReply : public JsonValue
{
public:
	_RedisReply() = default;
	virtual ~_RedisReply() {}
	// add move operator

	REPLY_TYPE _type;
	int64_t _integer;
	std::string _string;
	std::vector<_RedisReply> _array;

	virtual Json::Type type() const
	{
		if (_type == REPY_NIL) return Json::NUL;
		else if (_type == REPY_INTEGER) return Json::NUMBER;
		else if (_type == REPY_STRING) return Json::STRING;
		else if (_type == REPY_BULKSTRING) return Json::ARRAY;
		else if (_type == REPY_ARRAY) return Json::OBJECT;
		return Json::NUL;
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
		if (_type == REPY_NIL)
		{
			out += "nil";
		}
		else if (_type == REPY_INTEGER)
		{
			out += std::to_string(_integer);
		}
		else if (_type == REPY_STRING ||_type == REPY_ERROR)
		{
			out += _string;
		}
		else if (_type == REPY_BULKSTRING)
		{
			out += &_string[0];
		}
		else if (_type == REPY_ARRAY)
		{
			bool first = true;
			out += "[";
			for (auto it = _array.begin(); it != _array.end(); ++it)
			{
				if (!first) out += ", ";
				(*it).dump(out);
				first = false;
			}
			out += "]";
		}
	}

	Json to_json() const
	{
		std::string str;
		if (_type == REPY_NIL)
		{
			str += "nil";
		}
		else if (_type == REPY_INTEGER)
		{
			str += std::to_string(_integer);
		}
		else if (_type == REPY_STRING || _type == REPY_ERROR)
		{
			str += _string;
		}
		else if(_type == REPY_BULKSTRING)
		{
			str += &_string[0];
			
			//std::string err;
			//auto json = Json::parse(str, err);
			//return json;
		}
		else if (_type == REPY_ARRAY)
		{
			bool first = true;
			str += "[";
			for (auto it = _array.begin(); it != _array.end(); ++it)
			{
				if (!first) str += ", ";
				str += (*it).to_json().string_value();
				first = false;
			}
			str += "]";
		}
		return Json(str);

		//std::string err;
		//auto json = Json::parse(str, err, JsonParse::COMMENTS);
		//return json;
	}
};

// redis server reply data parser
static bool redisReplyParsing(_RedisReply& replay_, const char* buff_, size_t len_, size_t& parsingSize_)
{
	//auto replay = new _RedisReply();
	auto cmd = buff_[0];
	if (cmd == '+') replay_._type = REPY_STRING;
	else if (cmd == '-') replay_._type = REPY_ERROR;
	else if (cmd == ':') replay_._type = REPY_INTEGER;
	else if (cmd == '*') replay_._type = REPY_ARRAY;
	else if (cmd == '$') replay_._type = REPY_BULKSTRING;

	if (replay_._type == REPY_STRING || replay_._type == REPY_ERROR)
	{
		bool result = false;
		for (size_t i = 1; i < len_; ++i)
		{
			auto str = buff_[i];
			if (str == '\r')
			{
				auto strnext = buff_[i + 1];
				if (strnext == '\n')
				{
					i++;
					result = true;
					parsingSize_ = i + 1;
					break;
				}
			}
			else
			{
				replay_._string.push_back(str);
			}
		}

		if (result == true)
		{
			replay_._string.push_back('\0');
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (replay_._type == REPY_INTEGER)
	{
		bool result = false;
		std::string temp;
		for (size_t i = 1; i < len_; ++i)
		{
			auto str = buff_[i];
			if (str == '\r')
			{
				auto strnext = buff_[i + 1];
				if (strnext == '\n')
				{
					i++;
					result = true;
					parsingSize_ = i + 1;
					break;
				}
			}
			else
			{
				temp.push_back(str);
			}
		}

		if (result == true)
		{
			replay_._integer = std::stoll(temp);
			return true;
		}
		else
		{
			return false;
		}
	}
	else if (replay_._type == REPY_BULKSTRING)
	{
		//bool result = false;
		std::string strNum;
		size_t charCount = 0;
		bool isdollar = true;
		for (size_t i = 1; i < len_; ++i)
		{
			auto str = buff_[i];
			if (str == '\r')
			{
				auto strnext = buff_[i + 1];
				if (strnext == '\n' && charCount == replay_._string.size())
				{
					i++;
					isdollar = false;
					charCount = std::stoi(strNum);
					if (charCount == 0)
					{
						parsingSize_ = i + 1;
						replay_._type = REPY_NIL;
						replay_._string.clear();
						return true;
					}

					if (charCount == replay_._string.size())
					{
						parsingSize_ = i + 1;
						break;
					}
				}
				else
				{
					if (isdollar == false)
						replay_._string.push_back(str);
				}
			}
			else
			{
				if (isdollar == false)
					replay_._string.push_back(str);

				if (isdollar == true)
					strNum.push_back(str);
			}
		}

		if (charCount == replay_._string.size())
		{
			replay_._string.push_back('\0');
			return true;
		}
		else
		{
			parsingSize_ = len_;
			return false;
		}
	}
	else if (replay_._type == REPY_ARRAY)
	{
		//bool result = false;
		std::vector<char> temp;
		std::string starNum;
		std::string dollarNum;
		std::string colonNum;
		size_t arrayCount = 0;
		size_t charCount = 0;
		//int sectionNum = 0;
		bool isstar = true;
		bool isdollar = false;
		bool iscolon = false;
		for (size_t i = 1; i < len_; ++i)
		{
			auto str = buff_[i];
			auto preStr = buff_[i - 1];
			if (str == '$')
			{
				isdollar = true;
				temp.clear();
				temp.push_back(str);
			}
			else if (str == ':' && preStr == '\n')
			{				
				iscolon = true;
				temp.push_back(str);
			}
			else if (str == '\r')
			{
				auto strnext = buff_[i + 1];
				if (strnext == '\n')
				{
					i++;
					if (isstar == true)
					{
						arrayCount = std::stoi(starNum);
						isstar = false;
						if (replay_._array.size() == arrayCount)
						{
							parsingSize_ = i + 1;
							break;
						}
					}

					temp.push_back(str);
					temp.push_back(strnext);

					if (iscolon == true)
					{
						charCount = (int)(colonNum.size() + 3);
						iscolon = false;
						colonNum.clear();
					}

					if (temp.size() == charCount)
					{
						size_t tempSize = 0;
						_RedisReply tempReply;
						if( true == redisReplyParsing(tempReply, &temp[0], (int)temp.size(), tempSize))
							replay_._array.emplace_back(tempReply);
						temp.clear();

						if (replay_._array.size() == arrayCount)
						{
							parsingSize_ = i + 1;
							break;
						}
					}

					if (isdollar == true)
					{
						charCount = (int)(std::stoi(dollarNum) + dollarNum.size() + 5);
						isdollar = false;
						dollarNum.clear();
					}
				}
			}
			else
			{
				if (isstar == false)
					temp.push_back(str);

				if (isstar == true)
					starNum.push_back(str);

				if (isdollar == true)
					dollarNum.push_back(str);

				if (iscolon == true)
					colonNum.push_back(str);
			}
		}

		if (replay_._array.size() == arrayCount)
		{
			return true;
		}
		else if (arrayCount == 0 && replay_._array.size() > 0)
		{
			parsingSize_ = len_;
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
}


// redis server node info
struct _RedisNode
{
	enum ROLE_TYPE
	{
		ROLE_MASTER = 0,
		ROLE_SLAVE = 1,
	};

	uint32_t _dbIndex;
	std::string _address;
	uint32_t _port;
	std::string _pwd;

	uint32_t _poolSize;
	uint32_t _timeOut;
	ROLE_TYPE _role;

};

// hash function
template<typename T>
static int64_t hashFunc(T ty_, int64_t mask_= 0x0f)
{
	return std::hash<T>(ty_) & mask_;
}


// redis connector info
struct _RedisCon
{
	enum CON_TYPE
	{
		CON_CACHE = 0,		// mem cached
		CON_PUBSUB = 1,	// message queue

	};


	// network session
	CON_TYPE _type;
	std::string _address;
	uint32_t _port;
	std::string _pwd;

};


}