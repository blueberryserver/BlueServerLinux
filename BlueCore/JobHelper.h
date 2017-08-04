#pragma once
namespace BLUE_BERRY {

template<int N>
struct TupleUnpacker
{
	template<class _T, class... _ARGS, class... _TupleARGS, class... _Args>
	static void doExecute(_T* obj_, void(_T::*memfunc_)(_ARGS...), const std::tuple<_TupleARGS...>& tupleArgs_, _Args... args_)
	{
		TupleUnpacker<N - 1>::doExecute(obj_, memfunc_, tupleArgs_, std::get<N - 1>(tupleArgs_), args_...);
	}

	template<class... _ARGS, class... _TupleARGS, class... _Args>
	static void doExecute(void(*func_)(_ARGS...), const std::tuple<_TupleARGS...>& tupleArgs_, _Args... args_)
	{
		TupleUnpacker<N - 1>::doExecute(func_, tupleArgs_, std::get<N - 1>(tupleArgs_), args_...);
	}

	template<class... _ARGS, class... _TupleARGS, class... _Args>
	static void doExecute(std::function<void(_ARGS...)> func_, const std::tuple<_TupleARGS...>& tupleArgs_, _Args... args_)
	{
		TupleUnpacker<N - 1>::doExecute(func_, tupleArgs_, std::get<N - 1>(tupleArgs_), args_...);
	}
};

template<>
struct TupleUnpacker<0>
{
	template<class _T, class... _ARGS, class... _TupleARGS, class... _Args>
	static void doExecute(_T* obj_, void(_T::*memfunc_)(_ARGS...), const std::tuple<_TupleARGS...>& tupleArgs_, _Args... args_)
	{
		(obj_->*memfunc_)(args_...);
	}

	template<class... _ARGS, class... _TupleARGS, class... _Args>
	static void doExecute(void(*func_)(_ARGS...), const std::tuple<_TupleARGS...>& tupleArgs_, _Args... args_)
	{
		(*func_)(args_...);
	}

	template<class... _ARGS, class... _TupleARGS, class... _Args>
	static void doExecute(std::function<void(_ARGS...)> func_, const std::tuple<_TupleARGS...>& tupleArgs_, _Args... args_)
	{
		func_(args_...);
	}
};

template<class _T, class... _ARGS, class... _TupleARGS>
void doExecute(_T* obj_, void(_T::*memfunc_)(_ARGS...), const std::tuple<_TupleARGS...>& tupleArgs_)
{
	TupleUnpacker<sizeof...(_TupleARGS)>::doExecute(obj_, memfunc_, tupleArgs_);
}


template<class... _ARGS, class... _TupleARGS>
void doExecute( void(*func_)(_ARGS...), const std::tuple<_TupleARGS...>& tupleArgs_)
{
	TupleUnpacker<sizeof...(_TupleARGS)>::doExecute(func_, tupleArgs_);
}

template<class... _ARGS, class... _TupleARGS>
void doExecute(std::function<void(_ARGS...)> func_, const std::tuple<_TupleARGS...>& tupleArgs_)
{
	TupleUnpacker<sizeof...(_TupleARGS)>::doExecute(func_, tupleArgs_);
}



// lamda function to std::function
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




}
