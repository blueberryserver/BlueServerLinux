#pragma once
#include <functional>
#include <future>
#include "../BlueCore/Callback.h"


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
LamdaToFuncObj(F const &func)
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

	auto job = BLUE_BERRY::makePostJobStatic(LamdaToFuncObj(lfunc));
	BLUE_BERRY::executePostJob(job, 10, 200);
}


int TestFunc(int i)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	return i * 100;
}

TEST(Cpp, Future)
{
	auto futureResult = std::async(std::launch::async, TestFunc, 100);
	auto result = futureResult.get();

	ASSERT_EQ(result, 10000);
}