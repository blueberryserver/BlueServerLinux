#pragma once
#include <boost/pool/pool.hpp>

#include "MemoryPool.h"
namespace BLUE_BERRY {
// buffer size 64k 
#define DEFAULT_CIRCULAR_BUFFER_SIZE 65535
class CircularBuffer
{
private:

private:
	// start pos
	char* _start;
	// end pos
	char* _end;
	// A region pos
	char* _a;
	// A region size
	size_t _aSize;

	// B region pos
	char* _b;
	// B region size
	size_t _bSize;

public:
	CircularBuffer(size_t size_ = DEFAULT_CIRCULAR_BUFFER_SIZE)
	{
		_start = static_cast<char*>( MemoryPool::getMemoryPool()->alloc(size_) );
		_end = _start + size_;

		// first use A region 
		_a = _start;
		_b = nullptr;
		_aSize = 0;
		_bSize = 0;
	}
	~CircularBuffer()
	{
		MemoryPool::getMemoryPool()->free(static_cast<void*>(_start));
	}

	size_t getFreeSize()
	{
		if (_b != nullptr)
		{
			// B region rest size
			return _a - _b - _bSize;
		}
		else
		{
			// A region rest buffer size < not used size(B region)
			if ((_end - _a - _aSize) < (size_t)(_a - _start))
			{
				// B region assign
				_b = _start;

				// B region rest size
				return _a - _b - _bSize;
			}
			else
			{
				//A region rest size
				return _end - _a - _aSize;
			}
		}
	}

	size_t getStoredSize() const
	{
		return _aSize + _bSize;
	}

	size_t getContiguiousBytes()
	{
		if (_aSize > 0)
			return _aSize;
		else
			return _bSize;
	}

	// write
	char* getWritableBuffer()
	{
		// first B region check
		if (_b != nullptr)
			return _b + _bSize;
		else
			return _a + _aSize;
	}

	// read
	char* getReadableBuffer()
	{
		// first A region check
		if (_aSize > 0)
			return _a;
		else
			return _b;
	}

	// buffer write complete( recv complete )
	void commit(size_t bytes_)
	{
		if (_b != nullptr)
			_bSize += bytes_;
		else
			_aSize += bytes_;
	}

	// buffer read complete( send complete )
	void remove(size_t bytes_)
	{
		size_t bytes = bytes_;

		if (_aSize > 0)
		{
			size_t aRemove = bytes > _aSize ? _aSize : bytes;
			_aSize -= aRemove;
			_a += aRemove;
			bytes -= aRemove;
		}

		if (bytes > 0 && _bSize > 0)
		{
			size_t bRemove = bytes > _bSize ? _bSize : bytes;
			_bSize -= bRemove;
			_b += bRemove;
			bytes -= bRemove;
		}

		if (_aSize == 0)
		{
			if (_bSize > 0)
			{
				if (_b != _start)
					memmove(_start, _b, _bSize);

				_a = _start;
				_aSize = _bSize;
				_b = nullptr;
				_bSize = 0;
			}
			else
			{
				_b = nullptr;
				_bSize = 0;
				_a = _start;
				_aSize = 0;
			}
		}
	}
};

DECLARE_SMART_PTR(CircularBuffer);
}