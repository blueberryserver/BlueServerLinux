#pragma once
//#include <boost/lockfree/queue.hpp>
#include "concurrentqueue.h"
namespace BLUE_BERRY {

template<typename _T, size_t _Size = 128>
class LockFreeQueue
{
private:
	//boost::lockfree::queue<_T> _queue;
	moodycamel::ConcurrentQueue<_T> _queue;
	
public:

	LockFreeQueue()
		:_queue(_Size)
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
		_T list[1000];
		auto count = _queue.try_dequeue_bulk(list, 1000);
		result_.assign(list, list + count);

		if (result_.empty() == false)
			return true;

		return false;
	}
};

}