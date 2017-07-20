#pragma once
#include <boost/pool/pool.hpp>

#include "MemoryPool.h"
namespace BLUE_BERRY {
	// 64k ���� ũ��
#define DEFAULT_CIRCULAR_BUFFER_SIZE 65535
class CircularBuffer
{
private:

private:
	// ���� ���� ������
	char* _start;
	// ���� �� ������
	char* _end;
	// a���� ������
	char* _a;
	// a���� ũ��
	size_t _aSize;

	// b���� ������
	char* _b;
	// b���� ũ��
	size_t _bSize;

public:
	CircularBuffer(size_t size_ = DEFAULT_CIRCULAR_BUFFER_SIZE)
	{
		_start = static_cast<char*>( MemoryPool::getMemoryPool()->alloc(size_) );
		//_start = static_cast<char*>( malloc(size_));
		_end = _start + size_;

		// ó�� a�� ���
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
			// b���� ���� ���� ũ��
			return _a - _b - _bSize;
		}
		else
		{
			// b���� ��� ���� ���

			// a���� ���� ������ ũ�Ⱑ �̻�� ���� ���� ���� ���
			if ((_end - _a - _aSize) < (size_t)(_a - _start))
			{
				// �̻�� ���� ���ۿ� b ���� �Ҵ�
				// ���� a������ �Ҵ� ���� ����

				_b = _start;

				// b���� ���� ���� ũ��
				return _a - _b - _bSize;
			}
			else
			{
				// a ������ ����ϴ� ���
				//���� ���� ũ��
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
		// b �켱 Ȯ��
		if (_b != nullptr)
			return _b + _bSize;
		else
			return _a + _aSize;
	}

	// read
	char* getReadableBuffer()
	{
		// a �켱 Ȯ��
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