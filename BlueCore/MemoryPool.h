#pragma once
#include <string.h>
#include <unordered_map>
#include <boost/pool/pool.hpp>

#include "Macro.h"
#include "ThreadUtil.h"
#include "LockFreeQueue.h"

namespace BLUE_BERRY
{
enum
{
	MEMORY_ALLOCATION_ALIGNMENT = 16,
};

// 메모리 해더(사이즈 정보 저장)
class MemoryHeader
{
public:
	size_t _size;
	MemoryHeader(size_t size_)
		: _size(size_) {}
};

template<class _Header = MemoryHeader>
inline void* allocMemoryHeader(_Header* header_, size_t size_)
{
	// 메모리 해더 생성자 호출
	new (header_)_Header(size_);
	// 해더 다음 데이터 메모리 주소 반환
	return ++header_;
}

template<class _Header = MemoryHeader>
inline _Header* freeMemoryHeader(void* mem_)
{
	_Header* header = (_Header*)mem_;

	// 데이터 메모리 주소에서 해더 메모리 주소로 이동
	--header;
	return header;
}

class MemoryPool
{
	enum
	{
		POOL_SIZE = 64,
		CHUNK_TABLE_SIZE = 1024,
	};

	struct ThreadPool
	{
		std::unordered_map<int64_t, boost::pool<>*> _pool;
		LockFreeQueue<void*, 65536> _restorePool;
		size_t _chunkSize;
	public:
		ThreadPool(size_t chunkSize_)
			: _chunkSize(chunkSize_)
		{
			_pool[getThreadId()] = new boost::pool<>(_chunkSize);
		}

		~ThreadPool()
		{
			for (auto it : _pool)
			{
				it.second->release_memory();
				delete it.second;
			}
		}

		void* malloc()
		{
			// check restore pool
			void* mem = nullptr;
			if (_restorePool.pop(mem) == true)
			{
				return mem;
			}

			if (_pool[getThreadId()] == nullptr)
			{
				_pool[getThreadId()] = new boost::pool<>(_chunkSize);
			}

			return _pool[getThreadId()]->ordered_malloc();
		}

		void free(void* mem_)
		{
			while(_restorePool.push(mem_) == false )
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			//if (_pool[getThreadId()] == nullptr)
			//{
			//	return;
			//}
			//
			//_pool[getThreadId()]->free(mem_);
		}

	};

public:
	MemoryPool(unsigned long threadCount_ = 0)
	{
		memset(_chunkTable, 0, sizeof(_chunkTable));

		int index = 1;
		for (int i = 0; i < POOL_SIZE; ++i)
		{
			int chunk_size = CHUNK_TABLE_SIZE * (4 * (i + 4));

			if (chunk_size % MEMORY_ALLOCATION_ALIGNMENT == 0)
			{
				_pool[i] = new ThreadPool(chunk_size);
				while (index < chunk_size / CHUNK_TABLE_SIZE)
				{
					_chunkTable[index++] = _pool[i];
				}
			}
			else
			{
				_pool[i] = nullptr;
			}
		}
		_chunkTable[0] = _chunkTable[1];
	}

	~MemoryPool()
	{
		for (int i = 0; i < POOL_SIZE; ++i)
		{
			delete _pool[i];
		}
	}

	void* alloc(size_t size_)
	{
		size_t chunkSize = size_ + sizeof(MemoryHeader);
		size_t index = (chunkSize / CHUNK_TABLE_SIZE);

		MemoryHeader* header = nullptr;
		if (index > CHUNK_TABLE_SIZE || _chunkTable[index] == nullptr)
		{
			header = static_cast<MemoryHeader*>(::malloc(chunkSize));
		}
		
		header = static_cast<MemoryHeader*>(_chunkTable[index]->malloc());

		if (header == nullptr)
		{
			return nullptr;
		}

		return allocMemoryHeader(header, chunkSize);
	}

	void free(void* mem_)
	{
		// 메모리 해더 획득
		MemoryHeader* header = (MemoryHeader*)mem_;
		--header;

		size_t chunkSize = header->_size;
		size_t index = (chunkSize / CHUNK_TABLE_SIZE);

		// 메모리 풀 테이블에 없음
		if (index > CHUNK_TABLE_SIZE || _chunkTable[index] == nullptr)
		{
			auto header = freeMemoryHeader(mem_);
			::free(header);
			return;
		}

		_chunkTable[index]->free(header);
	}

	DECLARE_MGR(MemoryPool);

private:
	ThreadPool* _chunkTable[CHUNK_TABLE_SIZE];
	ThreadPool* _pool[POOL_SIZE];
};

EXTERN_MGR(MemoryPool);

inline void* alloc(size_t size_)
{
	return ___MemoryPool->alloc(size_);
}

inline void free(void* mem_)
{
	___MemoryPool->free(mem_);
}

#define DECLARE_NEW_DELETE public: \
	void* operator new(size_t size_) \
	{ return alloc(size_); } \
	void operator delete(void* ptr_, size_t size_ ) \
	{ free(ptr_); }

}

