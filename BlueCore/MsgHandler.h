#pragma once
#include <functional>
#include <memory>
#include "Macro.h"

namespace BLUE_BERRY
{

template< typename T >
class MsgHandler
{
public:
	virtual ~MsgHandler() {}

	enum { MAX_MSG_ID = 0xffff - 1, };
	typedef std::function<bool(std::shared_ptr<T>, char*, unsigned short)> MSG_HANDLER_FUNCTION;

	MSG_HANDLER_FUNCTION _table[MAX_MSG_ID];
public:
	void addMsgHandler(unsigned short id_, MSG_HANDLER_FUNCTION func_)
	{
		_table[id_] = func_;
	}

	virtual bool execute(std::shared_ptr<T> session_, unsigned short id_, char* buff_, unsigned short  len_) { return false; }
};

#define REGIST_HANDLER(id___, func___) _table[id___] = func___
#define DECLARE_HANDLER(session___, function___) static bool function___(session___ session_, char* body_, unsigned short len_)
#define CC_NAMESPACE_NS(class___) ::class___
#define DEFINE_HANDLER(class___, session___, function___) bool class___ CC_NAMESPACE_NS(function___) (session___ session_, char* body_, unsigned short len_)

}
