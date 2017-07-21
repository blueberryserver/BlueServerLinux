#pragma once
#include <atomic>
#include <memory>
#include "Macro.h"
#include "MemoryPool.h"

namespace BLUE_BERRY {

class Buffer
{
public:
	// �޸� ũ��
	size_t _size;
	// �޸� ���� �ּ�
	char* _start;
	// �ʱ� ���� ���� �ּ�
	char* _base;
	// ���� ī��Ʈ
	std::atomic<long> _refCount;

public:
	explicit Buffer(size_t size_)
		: _size(size_)
	{
		_refCount.store(1);
		auto ptr = this; ++ptr;
		// buffer ���� �ּ� ����
		_base = reinterpret_cast<char*>(ptr);
		_start = _base;
		// �޸� ���� ���� ���� �ּ� ����
		if ((__int64_t)_start % MEMORY_ALLOCATION_ALIGNMENT != 0)
		{
			_start += MEMORY_ALLOCATION_ALIGNMENT - ((__int64_t)_start % MEMORY_ALLOCATION_ALIGNMENT);
		}
	}

	~Buffer() {}

	size_t getFreeSize() const
	{
		return _size - (_start - _base) - sizeof(*this);
	}
	void commit(size_t size_)
	{
		_start += size_;

		// 16 ����Ʈ ����
		if ((__int64_t)_start % MEMORY_ALLOCATION_ALIGNMENT != 0)
		{
			_start += MEMORY_ALLOCATION_ALIGNMENT - ((__int64_t)_start % MEMORY_ALLOCATION_ALIGNMENT);
		}
	}
	void clear()
	{
		_refCount.store(1);
		_start = _base;
		// �޸� ���� ���� ���� �ּ� ����
		if ((__int64_t)_start % MEMORY_ALLOCATION_ALIGNMENT != 0)
		{
			_start += MEMORY_ALLOCATION_ALIGNMENT - ((__int64_t)_start % MEMORY_ALLOCATION_ALIGNMENT);
		}
	}

	void incRefCount()
	{
		_refCount.fetch_add(1);
	}

	void decRefCount();

	// ������ ���� ���丮��(TLS) �Լ�
	DECLARE_TLS(Buffer)
};
EXTERN_TLS(Buffer)

class BufferPool
{	
private:
	enum { BUFFER_SIZE = 64 * 1024 * 2, };	// 128k
	boost::pool<>* _pool;

public:
	BufferPool()
	{
		_pool = new boost::pool<>(BUFFER_SIZE);
	}
	virtual ~BufferPool()
	{
		_pool->release_memory();
		delete _pool;
	}

	Buffer* aquire()
	{
		auto header = reinterpret_cast<Buffer*>(_pool->malloc() );
		new(header) Buffer(BUFFER_SIZE);
		return header;
	}
	void release(Buffer* buffer_)
	{
		buffer_->clear();
		_pool->free(buffer_);
	}

	DECLARE_MGR(BufferPool)
};

// ���� ���� Ŭ����
class BufferHelper
{
private:
	Buffer* _buffer;

public:
	char* _start;
	size_t _len;
	char* _pos;

	explicit BufferHelper(size_t size_) : _start(nullptr), _len(size_), _pos(nullptr)
	{
		auto buffer = Buffer::getTLSBuffer();
		if (buffer == nullptr)
		{
			Buffer::setTLSBuffer(BufferPool::getBufferPool()->aquire());
			buffer = Buffer::getTLSBuffer();
		}

		// �ʿ��� ���� ũ�� Ȯ��
		if (buffer->getFreeSize() < _len)
		{
			if (buffer->_refCount.load() > 1)
			{

				Buffer::setTLSBuffer(BufferPool::getBufferPool()->aquire());
				buffer = Buffer::getTLSBuffer();
			}
			else
			{
				buffer->clear();
			}
		}

		_buffer = buffer;
		_buffer->incRefCount();

		_start = _buffer->_start;
		_buffer->commit(_len);
		_pos = _start;
	}
	~BufferHelper()
	{
		_buffer->decRefCount();
	}

	// ��Ŷ �ش� ����
	template<class HEADER>
	HEADER* getHeader()
	{
		_pos = _start + sizeof(HEADER);
		return reinterpret_cast<HEADER*>(_start);
	}

	// ��Ŷ �ٵ� ����
	template<class HEADER>
	char* getBody()
	{
		auto header = reinterpret_cast<HEADER*>(_start);
		header++;
		return reinterpret_cast<char*>(header);
	}

	// ���� ����
	template<class T>
	void write(T value_)
	{
		//_ASSERTDEBUG(_pos - _start + sizeof(T) <= _len);
		memcpy(_pos, &value_, sizeof(T));
		_pos += sizeof(T);
	}

	void write(char* value_, size_t len_)
	{
		//_ASSERTDEBUG(_pos - _start + len_ <= _len);
		memcpy(_pos, value_, len_);
		_pos += len_;
	}

	// ���� �б�
	template<class T>
	bool read(T& value_)
	{
		//_ASSERTDEBUG(_pos - _start >= sizeof(T));
		value_ = *(T*)(_start);
		_start += sizeof(T);
		return true;
	}

	bool read(char* value_, size_t len_)
	{
		//_ASSERTDEBUG(_pos - _start >= (int)len_);
		memcpy(value_, _start, len_);
		_start += len_;
		return true;
	}
};

DECLARE_SMART_PTR(BufferHelper)

}