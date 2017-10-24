#pragma once
#include <future>
#include "Callback.h"
namespace BLUE_BERRY
{

template<typename T>
class RedisConnection
{

public:
	enum REDIS_CONN_TYPE
	{
		MASTER_CONN = 1,
		SLAVE_CONN = 2,
		PUBSUB_CONN = 3,
	};

	typedef typename std::shared_ptr<T> TPtr;

	RedisConnection(boost::asio::io_service& io_)
	{
		_MSession = std::make_shared<T>(io_);
		_SSession = std::make_shared<T>(io_);
		_SubSession = std::make_shared<T>(io_);

		_MSession->setPacketProcHandler(std::bind(&RedisConnection::recvPacketProc, this, std::placeholders::_1));
		_SSession->setPacketProcHandler(std::bind(&RedisConnection::recvPacketProc, this, std::placeholders::_1));
		_SubSession->setPacketProcHandler(std::bind(&RedisConnection::recvPacketProcByPublish, this, std::placeholders::_1));

	}
	~RedisConnection()
	{}

private:
	TPtr _MSession;	// master session
	TPtr _SSession;	// slave session

	TPtr _SubSession;	// subscribe session
	Callback* _channelMsgJob;


	LockFreeQueue<size_t, 65536> _keys;
	std::unordered_map<size_t, std::promise<_RedisReply>> _promises;


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
	bool checkConnection(REDIS_CONN_TYPE type_ = MASTER_CONN)
	{
		if (type_ == MASTER_CONN)
		{
			return _MSession->isConnected();
		}
		else if (type_ == SLAVE_CONN)
		{
			return _SSession->isConnected();
		}
		else
		{
			return _SubSession->isConnected();
		}
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
					LOG(L_DEBUG_, "replay", "result", (int)reply._integer);
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

				if (_keys.empty() == false)
				{
					size_t key;
					if (_keys.pop(key) == true)
					{
						_promises[key].set_value(std::move(reply));
					}
					else
					{
						LOG(L_INFO_, "key pop fail");
					}
				}

			}

			recvBuffSize -= len;
			buff_->remove(len);
		}
	}

	void setChannelMsgJob(Callback* job_) { _channelMsgJob = job_; }
	void recvPacketProcByPublish(CircularBuffer* buff_)
	{
		size_t recvBuffSize = buff_->getContiguiousBytes();

		while (recvBuffSize >= 1)
		{
			size_t len = 0;
			auto cmd = buff_->getReadableBuffer();
			LOG(L_DEBUG_, "replay", "origin str", std::string(cmd, recvBuffSize));

			_RedisReply reply;
			if (true == redisReplyParsing(reply, cmd, recvBuffSize, len))
			{
				// debuging reply log write
				if (reply._type == REPY_INTEGER)
				{
					LOG(L_DEBUG_, "replay", "result", (int)reply._integer);
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
					LOG(L_DEBUG_, "replay", "array", reply.to_json());
				}

				executePostJobNoDelete(_channelMsgJob, reply);
			}

			recvBuffSize -= len;
			buff_->remove(len);
		}
	}

	// server command
	bool select(int index_, std::future<_RedisReply>& result_)
	{
		if (false == _MSession->isConnected()) return false;

		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$6\r\n";
		ss << "SELECT\r\n";
		ss << "$1\r\n";
		ss << index_ << "\r\n";

		auto key = sendMsg(ss.str());

		result_ = _promises[key].get_future();
		return true;
	}

	bool auth(const char* pw_, std::future<_RedisReply>& result_)
	{
		if (false == _MSession->isConnected()) return false;

		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$4\r\n";
		ss << "AUTH\r\n";
		ss << "$" << strlen(pw_) << "\r\n";
		ss << pw_ << "\r\n";

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;
	}


	bool zadd(const char* key_, int score_, const char* value_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool zrem(const char* key_, const char* value_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool zcard(const char* key_, std::future<_RedisReply>& result_)
	{
		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$5\r\n";
		ss << "ZCARD\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool zincrby(const char* key_, int score_, const char* value_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool zrange(const char* key_, int start_, int end_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool zrevrange(const char* key_, int start_, int end_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool zrank(const char* key_, const char* value_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool zrevrank(const char* key_, const char* value_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool zscore(const char* key_, const char* value_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}


	bool hset(const char* key_, const char* field_, const char* value_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool hset(const char* key_, const char* field_, std::vector<char>& value_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool hsetnx(const char* key_, const char* field_, const char* value_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool hsetnx(const char* key_, const char* field_, std::vector<char>& value_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	// exist field
	bool hexists(const char* key_, const char* field_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	// get value
	bool hget(const char* key_, const char* field_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	// delete field
	bool hdel(const char* key_, const char* field_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool hgetall(const char* key_, std::future<_RedisReply>& result_)
	{
		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$7\r\n";
		ss << "HGETALL\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool hvals(const char* key_, std::future<_RedisReply>& result_)
	{
		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$5\r\n";
		ss << "HVALS\r\n";
		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}


	// expire key
	bool expire(const char* key_, int timeOut_, std::future<_RedisReply>& result_)
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

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	bool persist(const char* key_, std::future<_RedisReply>& result_)
	{
		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$7\r\n";
		ss << "PERSIST\r\n";

		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

	}
	// del key
	bool del(const char* key_, std::future<_RedisReply>& result_)
	{
		std::stringstream ss;
		ss << "*2\r\n";
		ss << "$3\r\n";
		ss << "DEL\r\n";

		// key
		ss << "$" << strlen(key_) << "\r\n";
		ss << key_ << "\r\n";

		auto key = sendMsg(ss.str());
		result_ = _promises[key].get_future();
		return true;

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

		return sendMsg(ss.str());
	}
	size_t subscribe(const std::vector<std::string>& channels_)
	{
		std::stringstream ss;
		for (size_t i = 0; i < channels_.size(); i++)
		{
			ss << "*2\r\n";
			ss << "$9\r\n";
			ss << "SUBSCRIBE\r\n";
			ss << "$" << channels_[i].length() << "\r\n";
			ss << channels_[i] << "\r\n";

			//ss << "$" << std::to_string(i + 1).length() << "\r\n";
			//ss << std::to_string(i + 1) << "\r\n";
		}

		return sendMsg(ss.str(), PUBSUB_CONN);
	}
	size_t unsubscribe(const std::vector<std::string>& channels_)
	{
		std::stringstream ss;
		for (size_t i = 0; i < channels_.size(); i++)
		{
			ss << "*3\r\n";
			ss << "$9\r\n";
			ss << "UNSUBSCRIBE\r\n";
			ss << "$" << channels_[i].length() << "\r\n";
			ss << channels_[i] << "\r\n";

			//ss << ":" << std::to_string(i + 1).length() << "\r\n";
			ss << ":" << std::to_string(i + 1) << "\r\n";
		}

		return sendMsg(ss.str(), PUBSUB_CONN);
	}
	size_t psubscribe(const std::vector<std::string>& patterns_)
	{
		std::stringstream ss;
		for (size_t i = 0; i < patterns_.size(); i++)
		{
			ss << "*3\r\n";
			ss << "$10\r\n";
			ss << "PSUBSCRIBE\r\n";
			ss << "$" << patterns_[i].length() << "\r\n";
			ss << patterns_[i] << "\r\n";

			//ss << ":" << std::to_string(i + 1).length() << "\r\n";
			ss << ":" << std::to_string(i + 1) << "\r\n";
		}
		return sendMsg(ss.str(), PUBSUB_CONN);
	}
	size_t punsubscribe(const std::vector<std::string>& patterns_)
	{
		std::stringstream ss;
		for (size_t i = 0; i < patterns_.size(); i++)
		{
			ss << "*3\r\n";
			ss << "$12\r\n";
			ss << "PUNSUBSCRIBE\r\n";
			ss << "$" << patterns_[i].length() << "\r\n";
			ss << patterns_[i] << "\r\n";

			//ss << ":" << std::to_string(i + 1).length() << "\r\n";
			ss << ":" << std::to_string(i + 1) << "\r\n";
		}
		return sendMsg(ss.str(), PUBSUB_CONN);
	}

	size_t sendMsg(const std::string& msg_, REDIS_CONN_TYPE type_ = MASTER_CONN)
	{
		auto key = std::hash<std::string>{}(msg_);
		_keys.push(key);
		//LOG(L_INFO_, "redis", "cmd", msg_, "key", (int)key);

		BufferHelperPtr packet(new BufferHelper(msg_.length()));
		packet->write(const_cast<char*>(msg_.c_str()), static_cast<short>(msg_.length()));

		if (type_ == MASTER_CONN)
		{
			_MSession->send(packet);
		}
		else if (type_ == SLAVE_CONN)
		{
			_SSession->send(packet);
		}
		else
		{
			_SubSession->send(packet);
		}
		return key;
	}

};

}