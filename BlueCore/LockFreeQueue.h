#pragma once
#include <boost/lockfree/queue.hpp>
namespace BLUE_BERRY {

template<typename _T>
class LockFreeQueue
{
private:
	boost::lockfree::queue<_T> _queue;
	
public:

	LockFreeQueue()
		:_queue(10000)
	{}

	~LockFreeQueue()
	{}

	bool push(const _T& value_)
	{
		return _queue.push(value_);
	}

	bool pop(_T& value_)
	{
		return _queue.pop(value_);
	}

	bool empty()
	{
		return _queue.empty();
	}

	bool popAll(std::vector<_T>& result_)
	{
		_T value;
		while (_queue.pop(value))
		{
			result_.push_back(value);
		}

		if (result_.empty() == false)
			return true;

		return false;
	}
};

}