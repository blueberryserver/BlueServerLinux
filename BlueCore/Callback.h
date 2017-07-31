#pragma once

namespace BLUE_BERRY
{

class Callback
{
public:
	virtual void execute(void* param_) {}
};


template<typename T, typename... A>
class PostJob : public Callback
{
	typedef void(T::*MemberFunc)(A...);
	typedef std::tuple<A...> TupleArgs;

public:
	explicit PostJob(T* t_, MemberFunc func_)
		: _t(t_), _memFunc(func_) {}

	virtual void execute(void* param_)
	{
		doExecute(_t, _memFunc, *reinterpret_cast<TupleArgs*>(param_));
	}

public:
	T* _t;
	MemberFunc _memFunc;
	TupleArgs _args;
};

template<class T, class... A>
static decltype(auto) makePostJob(T* t_, void (T::*func_)(A...))
{
	return new PostJob<T, A...>(static_cast<T*>(t_), func_);
}

template<typename... A>
class PostJobStatic : public Callback
{
	typedef void(*Func)(A...);
	typedef std::tuple<A...> TupleArgs;

public:
	explicit PostJobStatic(Func func_)
		: _func(func_) {}

	virtual void execute(void* param_)
	{
		doExecute(_func, *reinterpret_cast<TupleArgs*>(param_));
	}

public:
	Func _func;
	TupleArgs _args;
};

template<class... A>
static decltype(auto) makePostJobStatic(void (*func_)(A...))
{
	return new PostJobStatic<A...>(func_);
}



template<typename T, typename... A>
static void executePostJob(T* t_, A... a_)
{
	std::tuple<A...> args(std::forward<A>(a_)...);
	t_->execute(reinterpret_cast<void*>(&args));
}



}
