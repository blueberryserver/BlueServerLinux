#pragma once
#include <atomic>
#include <memory>
#include "Macro.h"
#include "MemoryPool.h"

namespace BLUE_BERRY {

class Buffer
{
public:
	// 메모리 크기
	size_t _size;
	// 메모리 시작 주소
	char* _start;
	// 초기 버퍼 시작 주소
	char* _base;
	// 참조 카운트
	std::atomic<long> _refCount;

public:
	explicit Buffer(size_t size_)
		: _size(size_)
	{
		_refCount.store(1);
		auto ptr = this; ++ptr;
		// buffer 시작 주소 저장
		_base = reinterpret_cast<char*>(ptr);
		_start = _base;
		// 메모리 정렬 기준 시작 주소 설정
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

		// 16 바이트 정렬
		if ((__int64_t)_start % MEMORY_ALLOCATION_ALIGNMENT != 0)
		{
			_start += MEMORY_ALLOCATION_ALIGNMENT - ((__int64_t)_start % MEMORY_ALLOCATION_ALIGNMENT);
		}
	}
	void clear()
	{
		_refCount.store(1);
		_start = _base;
		// 메모리 정렬 기준 시작 주소 설정
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

	// 스레드 로컬 스토리지(TLS) 함수
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

// 버퍼 보조 클래스
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

		// 필요한 버퍼 크기 확인
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

	// 패킷 해더 추출
	template<class HEADER>
	HEADER* getHeader()
	{
		_pos = _start + sizeof(HEADER);
		return reinterpret_cast<HEADER*>(_start);
	}

	// 패킷 바디 추출
	template<class HEADER>
	char* getBody()
	{
		auto header = reinterpret_cast<HEADER*>(_start);
		header++;
		return reinterpret_cast<char*>(header);
	}

	// 버퍼 쓰기
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

	// 버퍼 읽기
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