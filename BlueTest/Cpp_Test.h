#pragma once
#include <functional>
#include <future>
#include "../BlueCore/Callback.h"
#include "../BlueCore/ThreadUtil.h"


template<typename T>
void funcT(T t_)
{
	t_(10);
}

template<typename T>
struct memfun_type
{
	using type = void;
};

template<typename R, typename C, typename... A>
struct memfun_type<R(C::*)(A...) const>
{
	using type = std::function<R(A...)>;
};

template<typename F>
typename memfun_type<decltype(&F::operator())>::type
CapturedLamdaToFuncObj(F const &func)
{
	return func;
}


template<typename... A>
void makePostJobStatic( std::function<void(A...)> func_ )
{
	func_(10, 100);
}

TEST(Cpp, Lamda)
{
	auto lfunc = []( int a_, int b_ )
	{
		std::cout << "Lamda Test " << a_  << " " << b_ << std::endl; 
	};
	//funcT(lfunc);

	auto job = BLUE_BERRY::makePostJobStatic(CapturedLamdaToFuncObj(lfunc));
	BLUE_BERRY::executePostJob(job, 10, 200);

	auto job2 = BLUE_BERRY::makePostJobStatic(lfunc);
	BLUE_BERRY::executePostJob(job2, 10, 200);
}


int TestFunc(int i)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	return i * 100;
}

TEST(Cpp, Future)
{
	auto futureResult = std::async(std::launch::async, TestFunc, 100);
	auto result = futureResult.get();

	ASSERT_EQ(result, 10000);
}


void f(int a, int b, int c)
{
	c = a + b;
}
template<int N>
struct UnPack
{
	template<class _Fn, class... _ARGS, class... _TupleARGS, class... _Args>
	static void exec(_Fn func_, const std::tuple<_TupleARGS...>& tupleArgs_, _Args... args_)
	{
		UnPack<N - 1>::exec(func_, tupleArgs_, std::get<N - 1>(tupleArgs_), args_...);
	}
};

template<>
struct UnPack<0>
{
	template<class _Fn, class... _ARGS, class... _TupleARGS, class... _Args>
	static void exec(_Fn func_, const std::tuple<_TupleARGS...>& tupleArgs_, _Args... args_)
	{
		func_(args_...);
	}
};

template<class _Fn, class... _ARGS>
class AsyncJobStc
{
	typedef std::tuple<_ARGS...> args;

public:
	_Fn _func;
	args _args;
	const int _n = sizeof...(_ARGS);

	explicit AsyncJobStc(_Fn&& func_, _ARGS&&... args_)
		: _func(std::forward<_Fn>(func_)), _args(std::forward<_ARGS>(args_)...) {}

	virtual ~AsyncJobStc() {
	}

	void onExecute()
	{
		UnPack<sizeof...(_ARGS)>::exec(_func, _args);
	}
};


template<typename _Fn, typename ... _Args>
void funcMove(_Fn&& t, _Args&&... a)
{
	auto job = new AsyncJobStc<_Fn, _Args...>(std::forward<_Fn>(t), std::forward<_Args>(a)...);

	//t(std::forward<_Args>(a)...);
	job->onExecute();

}

template<class... _ARGS>
static void asyncJob(void(*func_)(_ARGS&&...), _ARGS&&... args_)
{
	//auto job = new AsyncJobStc<_ARGS...>(func_, args_...);
	//job->onExecute();
	func_(args_...);
}


TEST(Cpp, move)
{
	int r = { 0 };
	funcMove(f, 10, 20, r);

	int a = 10;
	int b = 20;
	int  c = 0;
	//funcMove(f, std::move(a), std::move(b), c );
	//asyncJob(f, std::forward<int>(a), std::forward<int>(b), std::forward<int>(c));
	//asyncJob(f, a, b, c);

	//asyncJob(f, a, b, c);

	int d = a + b + c;


}
std::string funcTest2(int a, int b, int c)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return std::string("AAAAAAAABBBBBBBVC");
}


TEST(Cpp, Task)
{
	std::packaged_task<std::string()> myTask([]() { funcTest2(10, 20, 30); });
	std::future<std::string> myFuture = myTask.get_future();
	std::async(std::launch::async, std::move(myTask));

	std::cout << myFuture.get() << "  :" << std::this_thread::get_id() << std::endl;
	std::hash<std::thread::id> h;
	std::cout << h(std::this_thread::get_id()) << std::endl;

	std::stringstream ss;
	ss << std::this_thread::get_id();
	uint64_t id = std::stoull(ss.str());
	std::cout << id << std::endl;

}

class Widget {
public:
	bool isValid() { return true; }
	bool isAchive() { return true; }
};

TEST(Cpp, UniquePtr)
{
	auto ptr = std::make_unique<Widget>();

	[](auto&& p) {
		p->isAchive();
	}(ptr);

	ptr->isAchive();
}