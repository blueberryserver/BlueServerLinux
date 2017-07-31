#pragma once
#include <vector>
#include <string>

#include "Logger.h"
#include "BufferPool.h"
#include "Session.h"
#include "RedisHelper.h"
#include "SyncJobHelper.h"


namespace BLUE_BERRY
{
template<typename T>
class RedisClient
{
public:
	typedef typename std::shared_ptr<T> TPtr;

	RedisClient(boost::asio::io_service& io_) 
	{
		_MSession = std::make_shared<T>(io_);
		_SSession = std::make_shared<T>(io_);
		_SubSession = std::make_shared<T>(io_);

		_MSession->setPacketProcHandler(std::bind(&RedisClient::recvPacketProc, this, std::placeholders::_1));
		_SSession->setPacketProcHandler(std::bind(&RedisClient::recvPacketProc, this, std::placeholders::_1));
		_SubSession->setPacketProcHandler(std::bind(&RedisClient::recvPacketProc, this, std::placeholders::_1));

	}
	~RedisClient()
	{}


public:
	bool connect(const char* addr_, short port_)
	{
		_MSession->connect(addr_, port_);
		_SSession->connect(addr_, port_);
		_SubSession->connect(addr_, port_);
		return true;
	}

	void close()
	{
		_MSession->disconnect();
		_SSession->disconnect();
		_SubSession->disconnect();
	}

	// buffer proc
	void recvPacketProc(CircularBuffer* buff_)
	{
		size_t recvBuffSize = buff_->getContiguiousBytes();

		while (recvBuffSize >= 1)
		{
			size_t len = 0;
			auto cmd = buff_->getReadableBuffer();

			_RedisReply reply;
			if (true == redisReplyParsing(reply, cmd, recvBuffSize, len))
			{
				// debuging reply log write
				if (reply._type == REPY_INTEGER)
				{
					LOG(L_DEBUG_, "replay", "result", reply._integer);
				}
				else if (reply._type == REPY_STRING || reply._type == REPY_ERROR || reply._type == REPY_BULKSTRING)
				{
					LOG(L_DEBUG_, "replay", "result", &reply._string[0]);
				}
				else if (reply._type == REPY_NIL)
				{
					LOG(L_DEBUG_, "replay", "result", "nil");
				}
				else if (reply._type == REPY_ARRAY)
				{
					LOG(L_DEBUG_, "replay", "array", reply._array);
				}
				if( _keys.empty() == false)
				{
					size_t key;
					if (_keys.pop(key) == true)
					{
						Callback* job;
						if (SyncJobManager::getSyncJobManager()->getPostJob(key, job) == true)
						{
							executePostJob(job, reply);
						}
					}
				}
			}

			recvBuffSize -= len;
			buff_->remove(len);
		}
	}


	// server command
	size_t select(int index_)
	{
		if (false == _MSession->isConnected()) return false;

		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$6\r\n";
		ss << "SELECT\r\n";
		ss << "$1\r\n";
		ss << index_ << "\r\n";

		return sendMsg(ss.str());
	}

	size_t auth(const char* pw_)
	{
		if (false == _MSession->isConnected()) return false;

		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$4\r\n";
		ss << "AUTH\r\n";
		ss << "$" << strlen(pw_) << "\r\n";
		ss << pw_ << "\r\n";

		return sendMsg(ss.str());

	}

	size_t zadd(const char* key_, int score_, const char* value_)
	{
		std::stringstream ss;
		ss << "*4\r\n";
		ss << "$4\r\n";
		ss << "ZADD\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// score
		auto strScore = std::to_string(score_);
		ss << "$" << strScore.length() << "\r\n";
		ss << strScore << "\r\n";

		// value
		ss << "$" << strlen(value_) << "\r\n";
		ss << value_ << "\r\n";

		return sendMsg(ss.str());

	}
	size_t zrem(const char* key_, const char* value_)
	{
		std::stringstream ss;
		ss << "*3\r\n";
		ss << "$4\r\n";
		ss << "ZREM\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// value
		ss << "$" << strlen(value_) << "\r\n";
		ss << value_ << "\r\n";

		return sendMsg(ss.str());

	}
	size_t zcard(const char* key_)
	{
		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$5\r\n";
		ss << "ZCARD\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		return sendMsg(ss.str());

	}
	size_t zincrby(const char* key_, int score_, const char* value_)
	{
		std::stringstream ss;
		ss << "*4\r\n";
		ss << "$7\r\n";
		ss << "ZINCRBY\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// score
		auto strScore = std::to_string(score_);
		ss << "$" << strScore.length() << "\r\n";
		ss << strScore.c_str() << "\r\n";

		// value
		ss << "$" << strlen(value_) << "\r\n";
		ss << value_ << "\r\n";

		return sendMsg(ss.str());

	}
	size_t zrange(const char* key_, int start_, int end_)
	{
		std::stringstream ss;
		ss << "*4\r\n";
		ss << "$6\r\n";
		ss << "ZRANGE\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// start
		auto strStart = std::to_string(start_);
		ss << "$" << strStart.length() << "\r\n";
		ss << strStart.c_str() << "\r\n";

		// end
		auto strEnd = std::to_string(end_);
		ss << "$" << strEnd.length() << "\r\n";
		ss << strEnd.c_str() << "\r\n";

		return sendMsg(ss.str());

	}
	size_t zrevrange(const char* key_, int start_, int end_)
	{
		std::stringstream ss;
		ss << "*4\r\n";
		ss << "$9\r\n";
		ss << "ZREVRANGE\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// start
		auto strStart = std::to_string(start_);
		ss << "$" << strStart.length() << "\r\n";
		ss << strStart.c_str() << "\r\n";

		// end
		auto strEnd = std::to_string(end_);
		ss << "$" << strEnd.length() << "\r\n";
		ss << strEnd.c_str() << "\r\n";

		return sendMsg(ss.str());

	}
	size_t zrank(const char* key_, const char* value_)
	{
		std::stringstream ss;
		ss << "*3\r\n";
		ss << "$5\r\n";
		ss << "ZRANK\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// value
		ss << "$" << strlen(value_) << "\r\n";
		ss << value_ << "\r\n";

		return sendMsg(ss.str());

	}
	size_t zrevrank(const char* key_, const char* value_)
	{
		std::stringstream ss;
		ss << "*3\r\n";
		ss << "$8\r\n";
		ss << "ZREVRANK\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// value
		ss << "$" << strlen(value_) << "\r\n";
		ss << value_ << "\r\n";

		return sendMsg(ss.str());

	}
	size_t zscore(const char* key_, const char* value_)
	{
		std::stringstream ss;
		ss << "*3\r\n";
		ss << "$6\r\n";
		ss << "ZSCORE\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// value
		ss << "$" << strlen(value_) << "\r\n";
		ss << value_ << "\r\n";

		return sendMsg(ss.str());

	}



	size_t hset(const char* key_, const char* field_, const char* value_)
	{
		std::stringstream ss;
		ss << "*4\r\n";
		ss << "$4\r\n";
		ss << "HSET\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// field
		ss << "$" << strlen(field_) << "\r\n";
		ss << field_ << "\r\n";

		// value
		ss << "$" << strlen(value_) << "\r\n";
		ss << value_ << "\r\n";

		return sendMsg(ss.str());

	}
	size_t hset(const char* key_, const char* field_, std::vector<char>& value_)
	{
		std::stringstream ss;
		ss << "*4\r\n";
		ss << "$4\r\n";
		ss << "HSET\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// field
		ss << "$" << strlen(field_) << "\r\n";
		ss << field_ << "\r\n";

		// value
		ss << "$" << value_.size() << "\r\n";
		ss.write(&value_[0], value_.size());
		ss << "\r\n";

		return sendMsg(ss.str());

	}
	size_t hsetnx(const char* key_, const char* field_, const char* value_)
	{
		std::stringstream ss;
		ss << "*4\r\n";
		ss << "$6\r\n";
		ss << "HSETNX\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// field
		ss << "$" << strlen(field_) << "\r\n";
		ss << field_ << "\r\n";

		// value
		ss << "$" << strlen(value_) << "\r\n";
		ss << value_ << "\r\n";

		return sendMsg(ss.str());

	}
	size_t hsetnx(const char* key_, const char* field_, std::vector<char>& value_)
	{
		std::stringstream ss;
		ss << "*4\r\n";
		ss << "$6\r\n";
		ss << "HSETNX\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// field
		ss << "$" << strlen(field_) << "\r\n";
		ss << field_ << "\r\n";

		// value
		ss << "$" << value_.size() << "\r\n";
		ss.write(&value_[0], value_.size());
		ss << "\r\n";

		return sendMsg(ss.str());

	}
	// exist field
	size_t hexists(const char* key_, const char* field_)
	{
		std::stringstream ss;
		ss << "*3\r\n";
		ss << "$7\r\n";
		ss << "HEXISTS\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// field
		ss << "$" << strlen(field_) << "\r\n";
		ss << field_ << "\r\n";

		return sendMsg(ss.str());

	}
	// get value
	size_t hget(const char* key_, const char* field_)
	{
		std::stringstream ss;
		ss << "*3\r\n";
		ss << "$4\r\n";
		ss << "HGET\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// field
		ss << "$" << strlen(field_) << "\r\n";
		ss << field_ << "\r\n";

		return sendMsg(ss.str());

	}
	// delete field
	size_t hdel(const char* key_, const char* field_)
	{
		std::stringstream ss;
		ss << "*3\r\n";
		ss << "$4\r\n";
		ss << "HDEL\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// field
		ss << "$" << strlen(field_) << "\r\n";
		ss << field_ << "\r\n";

		return sendMsg(ss.str());

	}
	size_t hgetall(const char* key_)
	{
		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$7\r\n";
		ss << "HGETALL\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		return sendMsg(ss.str());

	}
	size_t hvals(const char* key_)
	{
		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$5\r\n";
		ss << "HVALS\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		return sendMsg(ss.str());

	}




	// expire key
	size_t expire(const char* key_, int timeOut_)
	{
		std::stringstream ss;
		ss << "*3\r\n";
		ss << "$6\r\n";
		ss << "EXPIRE\r\n";

		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		// start
		auto strStart = std::to_string(timeOut_);
		ss << "$" << strStart.length() << "\r\n";
		ss << strStart.c_str() << "\r\n";

		return sendMsg(ss.str());

	}

	size_t persist(const char* key_)
	{
		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$7\r\n";
		ss << "PERSIST\r\n";

		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		return sendMsg(ss.str());

	}
	// del key
	size_t del(const char* key_)
	{
		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$3\r\n";
		ss << "DEL\r\n";

		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		return sendMsg(ss.str());

	}

	size_t publish(const char* channel_, const char* data_)
	{
		std::stringstream ss;

		ss << "*3\r\n";
		ss << "$7\r\n";
		ss << "PUBLISH\r\n";

		ss << "$" << strlen(channel_) << "\r\n";
		ss << channel_ << "\r\n";

		ss << "$" << strlen(data_) << "\r\n";
		ss << data_ << "\r\n";

		return sendMsg(ss.str(), false);

	}

	size_t subscribe(std::vector<std::string>& channels_)
	{
		std::stringstream ss;
		for (int i = 0; i < channels_.size(); i++)
		{
			ss << "*3\r\n";
			ss << "$9\r\n";
			ss << "SUBSCRIBE\r\n";
			ss << "$" << channels_[i].length() << "\r\n";
			ss << channels_[i] << "\r\n";

			ss << "$" << std::to_string(i + 1).length() << "\r\n";
			ss << std::to_string(i + 1) << "\r\n";
		}

		return sendMsg(ss.str());
	}

	size_t unsubscribe(std::vector<std::string>& channels_)
	{
		std::stringstream ss;
		for (int i = 0; i < channels_.size(); i++)
		{
			ss << "*3\r\n";
			ss << "$9\r\n";
			ss << "UNSUBSCRIBE\r\n";
			ss << "$" << channels_[i].length() << "\r\n";
			ss << channels_[i] << "\r\n";

			ss << "$" << std::to_string(i + 1).length() << "\r\n";
			ss << std::to_string(i + 1) << "\r\n";
		}

		return sendMsg(ss.str());
	}

	size_t psubscribe(std::vector<std::string>& patterns_)
	{
		std::stringstream ss;
		for (int i = 0; i < patterns_.size(); i++)
		{
			ss << "*3\r\n";
			ss << "$10\r\n";
			ss << "PSUBSCRIBE\r\n";
			ss << "$" << patterns_[i].length() << "\r\n";
			ss << patterns_[i] << "\r\n";

			ss << "$" << std::to_string(i + 1).length() << "\r\n";
			ss << std::to_string(i + 1) << "\r\n";
		}
		return sendMsg(ss.str());
	}

	size_t punsubscribe(std::vector<std::string>& patterns_)
	{
		std::stringstream ss;
		for (int i = 0; i < patterns_.size(); i++)
		{
			ss << "*3\r\n";
			ss << "$12\r\n";
			ss << "PUNSUBSCRIBE\r\n";
			ss << "$" << patterns_[i].length() << "\r\n";
			ss << patterns_[i] << "\r\n";

			ss << "$" << std::to_string(i + 1).length() << "\r\n";
			ss << std::to_string(i + 1) << "\r\n";
		}
		return sendMsg(ss.str());
	}


private:
	size_t sendMsg(const std::string& msg_, bool sendMaster_ = true)
	{
		auto key = std::hash<std::string>{}(msg_);
		_keys.push(key);
		LOG(L_INFO_, "redis", "cmd", msg_, "key", (int)key);

		BufferHelperPtr packet(new BufferHelper(msg_.length()));
		packet->write(const_cast<char*>(msg_.c_str()), static_cast<short>(msg_.length()));

		if (sendMaster_ == true)
		{
			_MSession->send(packet);
			return key;
		}

		_SSession->send(packet);
		return key;
	}

	size_t sendSubMsg(const std::string& msg_)
	{
		auto key = std::hash<std::string>{}(msg_);
		_keys.push(key);
		LOG(L_INFO_, "redis", "cmd", msg_, "key", (int)key);

		BufferHelperPtr packet(new BufferHelper(msg_.length()));
		packet->write(const_cast<char*>(msg_.c_str()), static_cast<short>(msg_.length()));
		_SubSession->send(packet);
		return key;
	}


private:
	TPtr _MSession;	// master session
	TPtr _SSession;	// slave session

	TPtr _SubSession;	// subscribe session

	// redis request keys
	LockFreeQueue<size_t> _keys;
};

}