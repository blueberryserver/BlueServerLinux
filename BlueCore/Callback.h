#pragma once
#include "JobHelper.h"

namespace BLUE_BERRY
{

class Callback
{
public:
	virtual ~Callback() {}
	virtual void execute() {}
};


template<typename T>
class PostJob : public Callback
{
public:
	explicit PostJob(T lamda_)
		: _lamda(lamda_) {}
	virtual ~PostJob() = default;

	virtual void execute()
	{
		_lamda();
	}

public:
	T _lamda;
};

template<class T>
static decltype(auto) makePostJob(T lamda_)
{
	return new PostJob<T>(lamda_);
}

}
