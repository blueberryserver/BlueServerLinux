#pragma once
#include <boost/pool/pool.hpp>

#include "MemoryPool.h"
namespace BLUE_BERRY {
	// 64k 버퍼 크기
#define DEFAULT_CIRCULAR_BUFFER_SIZE 65535
class CircularBuffer
{
private:

private:
	// 버퍼 시작 포인터
	char* _start;
	// 버퍼 끝 포인터
	char* _end;
	// a지역 포인터
	char* _a;
	// a지역 크기
	size_t _aSize;

	// b지역 포인터
	char* _b;
	// b지역 크기
	size_t _bSize;

public:
	CircularBuffer(size_t size_ = DEFAULT_CIRCULAR_BUFFER_SIZE)
	{
		_start = static_cast<char*>( MemoryPool::getMemoryPool()->alloc(size_) );
		//_start = static_cast<char*>( malloc(size_));
		_end = _start + size_;

		// 처음 a만 사용
		_a = _start;
		_b = nullptr;
		_aSize = 0;
		_bSize = 0;
	}
	~CircularBuffer()
	{
		MemoryPool::getMemoryPool()->free(static_cast<void*>(_start));
		//free(_start);
	}

	size_t getFreeSize()
	{
		if (_b != nullptr)
		{
			// b지역 남은 버퍼 크기
			return _a - _b - _bSize;
		}
		else
		{
			// b지역 사용 안할 경우

			// a지역 남은 버퍼의 크기가 미사용 버퍼 보다 작을 경우
			if ((_end - _a - _aSize) < (size_t)(_a - _start))
			{
				// 미사용 중인 버퍼에 b 지역 할당
				// 이제 a지역은 할당 하지 않음

				_b = _start;

				// b지역 남은 버퍼 크기
				return _a - _b - _bSize;
			}
			else
			{
				// a 지역만 사용하는 경우
				//남은 버퍼 크기
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
		// b 우선 확인
		if (_b != nullptr)
			return _b + _bSize;
		else
			return _a + _aSize;
	}

	// read
	char* getReadableBuffer()
	{
		// a 우선 확인
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

}