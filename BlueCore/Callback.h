#pragma once
#include "JobHelper.h"

namespace BLUE_BERRY
{

class Callback
{
public:
	virtual void execute() = 0;
};


template<typename _T>
class PostJob : public Callback
{
public:
	explicit PostJob(_T lamda_)
		: _lamda(lamda_) {}

	virtual void execute() override
	{
		_lamda();
	}

public:
	_T _lamda;
};

template<class _T>
static decltype(auto) makePostJob(_T lamda_)
{
	return std::make_shared<PostJob<_T>>(lamda_);
}

}
