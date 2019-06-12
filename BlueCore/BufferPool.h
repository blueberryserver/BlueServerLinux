#pragma once
#include <atomic>
#include <memory>
#include "Macro.h"
#include "MemoryPool.h"

namespace BLUE_BERRY {

class Buffer
{
public:
	size_t _size;
	char* _start;
	// base pos
	char* _base;
	std::atomic<long> _refCount;

public:
	explicit Buffer(size_t size_)
		: _size(size_)
	{
		_refCount.store(1);
		auto ptr = this; ++ptr;
		// save base pos
		_base = reinterpret_cast<char*>(ptr);
		_start = _base;
		// setting align address
		if ((__int64_t)_start % MEMORY_ALLOCATION_ALIGNMENT != 0)
		{
			_start += MEMORY_ALLOCATION_ALIGNMENT - ((__int64_t)_start % MEMORY_ALLOCATION_ALIGNMENT);
		}
	}

	~Buffer() = default;

	size_t getFreeSize() const
	{
		return _size - (_start - _base) - sizeof(*this);
	}
	void commit(size_t size_)
	{
		_start += size_;

		// memory address align
		if ((__int64_t)_start % MEMORY_ALLOCATION_ALIGNMENT != 0)
		{
			_start += MEMORY_ALLOCATION_ALIGNMENT - ((__int64_t)_start % MEMORY_ALLOCATION_ALIGNMENT);
		}
	}
	void clear()
	{
		_refCount.store(1);
		_start = _base;

		// memory address align
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

	// TLS
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

// buffer helper
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
		// check tls buffer 
		auto buffer = Buffer::getTLSBuffer();
		if (buffer == nullptr)
		{
			// aquire new buffer and set tls buff
			Buffer::setTLSBuffer(BufferPool::getBufferPool()->aquire());
			buffer = Buffer::getTLSBuffer();
		}

		// check require size
		if (buffer->getFreeSize() < _len)
		{
			// check now using
			if (buffer->_refCount.load() > 1)
			{
				// aquire new buffer and set tls buff
				Buffer::setTLSBuffer(BufferPool::getBufferPool()->aquire());
				buffer = Buffer::getTLSBuffer();
			}
			else
			{
				// nobody use buffer clear
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

	// extract header
	template<class HEADER>
	HEADER* getHeader()
	{
		_pos = _start + sizeof(HEADER);
		return reinterpret_cast<HEADER*>(_start);
	}

	// extract body
	template<class HEADER>
	char* getBody()
	{
		auto header = reinterpret_cast<HEADER*>(_start);
		header++;
		return reinterpret_cast<char*>(header);
	}

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