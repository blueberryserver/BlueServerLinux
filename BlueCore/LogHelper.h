#pragma once
#include "json11.hpp"
using namespace json11;

namespace BLUE_BERRY 
{
	enum _LogLevel
	{
		L_TRACE_ = 0x01,			// 모든 로그..
		L_DEBUG_ = 0x02,			// 디버깅
		L_INFO_ = 0x04,				// 정보, 알림
		L_ERROR_ = 0x08,			// 실서버 에러

		L_ALL_ = L_TRACE_ | L_DEBUG_ | L_INFO_ | L_ERROR_
	};

	enum _LogType
	{
		T_CONSOLE_ = 0x01,
		T_FILE_ = 0x02,
		T_NOSQL_ = 0x04,
		T_CF_ = T_CONSOLE_ | T_FILE_,
		T_ALL_ = T_CONSOLE_ | T_FILE_ | T_NOSQL_
	};

	class LogData;
	class LogWriter
	{
	public:
		virtual ~LogWriter() {}
		virtual void write(const LogData* data_) = 0;
	};


	inline static std::string ToStrLogLevel(_LogLevel level_)
	{
		switch (level_)
		{
		case L_TRACE_:
			return "Trace";
		case L_DEBUG_:
			return "Debug";
		case L_INFO_:
			return "Info ";
		case L_ERROR_:
			return "Error";
		default:
			return "";
		}
	}


	class LogData
	{
	public:
		struct Param
		{
			std::string _type;
			std::string _value;
			std::string _comment;
		};
		typedef std::vector<Param> ParamVec;
		typedef Json::array objects;

	public:
		// user
		int _id;
		int _no;
		int _line;
		int64_t _thread;
		//int _symbol;
		_LogLevel _level;
		//int _group;
		time_t _time;
		std::string _func;
		std::string _file;
		std::string _desc;
		ParamVec _params;
		objects _objects;
	public:
		LogData( int id_, int no_, int line_, int64_t thread_, _LogLevel level_, const std::string& func_,  const std::string& file_, const std::string& desc_)
			: _id(id_), _no(no_), _line(line_), _thread(thread_), _level(level_), _func(func_), _file(file_), _desc(desc_)
		{
			time(&_time);
		}
	};


	template <typename T>
	struct TypeFormat
	{
		static const char* GetType()
		{
			return typeid(T).name();
		}

		//static const std::string ToString(const char* v_)
		//{
		//	return std::string(v_);
		//}
		//static const std::string ToString(const std::string& v_)
		//{
		//	return v_;
		//}
		//static const std::string ToString(const char v_)
		//{
		//	return std::to_string(v_);
		//}
		//static const std::string ToString(const int& v_)
		//{
		//	return std::to_string(v_);
		//}
		//static const std::string ToString(const long& v_)
		//{
		//	return std::to_string(v_);
		//}
		//static const std::string ToString(const double& v_)
		//{
		//	return std::to_string(v_);
		//}
		//static const std::string ToString(const float& v_)
		//{
		//	return std::to_string(v_);
		//}
		template<typename std::enable_if_t<!std::is_base_of<std::string, std::decay_t<T>>::value>::type>
		static const std::string ToString(T&& v_)
		{
			return std::to_string(v_);
		}

		template<typename std::enable_if_t<std::is_base_of<std::string, std::decay_t<T>>::value>::type>
		static const std::string ToString(T&& v_)
		{
			return v_;
		}

		template<typename std::enable_if_t<std::is_same<char*, std::decay_t<T>>::value>::type>
		static const std::string ToString(T&& v_)
		{
			return std::string(v_);
		}
	};


	template <typename T>
	void AddParam(LogData::ParamVec& params_, const std::string& comment_, const T& v_)
	{
		LogData::Param param;
		param._type = TypeFormat<T>::GetType();
		param._value = TypeFormat<T>::ToString(v_);
		param._comment = comment_;
		params_.push_back(param);
	}

	template <typename T, typename ... ARGS>
	void AddParam(LogData::ParamVec& params_, const std::string& comment_, const T& v_, ARGS&... args_)
	{
		LogData::Param param;
		param._type = TypeFormat<T>::GetType();
		param._value = TypeFormat<T>::ToString(v_);
		param._comment = comment_;
		params_.push_back(param);
		AddParam(params_, args_...);
	}

	template <typename T, typename F>
	void AddParam(LogData::ParamVec& params_, const std::pair<T, F>& p_)
	{
		LogData::Param param;
		param._type = TypeFormat<F>::GetType();
		param._value = TypeFormat<F>::ToString(p_.second);
		param._comment = TypeFormat<T>::ToString(p_.first);
		params_.push_back(param);
	}

	template <typename T, typename F, typename ... ARGS>
	void AddParam(LogData::ParamVec& params_, const std::pair<T, F>& p_, ARGS&... args_)
	{
		LogData::Param param;
		param._type = TypeFormat<F>::GetType();
		param._value = TypeFormat<F>::ToString(p_.second);
		param._comment = TypeFormat<T>::ToString(p_.first);
		params_.push_back(param);
		AddParam(params_, args_...);
	}

	template <typename... ARGS>
	void AddParams(LogData::ParamVec& params_, ARGS&&... args_)
	{
		AddParam(params_, args_...);
	}

	template<typename T, typename F>
	void AddJsonObject(Json::array& objects_, const T& tag_, const F& value_)
	{
		Json temp = Json::object{ { tag_, value_ } };
		objects_.emplace_back(temp);
	}

	template<typename T, typename F, typename ...ARGS>
	void AddJsonObject(Json::array& objects_, const T& tag_, const F& value_, ARGS&&... args_)
	{
		Json temp = Json::object{ {tag_, value_} };
		objects_.emplace_back(temp);
		AddJsonObject(objects_, args_...);
	}

	/*
	template<typename T>
	void AddJsonObject(Json::array& objects_, const T& tag_)
	{
		// compler error
		static_assert(false, "argument pair fail!!!");
	}
	*/

}