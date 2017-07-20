#pragma once
//#include <boost/lockfree/queue.hpp>
#include "concurrentqueue.h"
namespace BLUE_BERRY {

template<typename _T>
class LockFreeQueue
{
private:
	//boost::lockfree::queue<_T> _queue;
	moodycamel::ConcurrentQueue<_T> _queue;
	
public:

	LockFreeQueue()
		:_queue(600000)
	{}

	~LockFreeQueue()
	{}

	bool push(const _T& value_)
	{
		//return _queue.push(value_);
		return _queue.try_enqueue(value_);
	}

	bool pop(_T& value_)
	{
		//return _queue.pop(value_);
		return _queue.try_dequeue(value_);
	}

	bool empty()
	{
		//return _queue.empty();
		return _queue.size_approx() == 0;
	}

	bool popAll(std::vector<_T>& result_)
	{
		//_queue.try_dequeue_bulk(result_.begin(), 1000);

		//*
		_T value;
		while (_queue.try_dequeue(value))
		{
			result_.push_back(value);
		}
		/**/

		if (result_.empty() == false)
			return true;

		return false;
	}
};

}