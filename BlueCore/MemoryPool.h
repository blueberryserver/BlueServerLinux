#pragma once
#include <string.h>
#include <boost/pool/pool.hpp>

#include "Macro.h"
namespace BLUE_BERRY
{
enum
{
	MEMORY_ALLOCATION_ALIGNMENT = 16,
};

// �޸� �ش�(������ ���� ����)
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
	// �޸� �ش� ������ ȣ��
	new (header_)_Header(size_);
	// �ش� ���� ������ �޸� �ּ� ��ȯ
	return ++header_;
}

template<class _Header = MemoryHeader>
inline _Header* freeMemoryHeader(void* mem_)
{
	_Header* header = (_Header*)mem_;

	// ������ �޸� �ּҿ��� �ش� �޸� �ּҷ� �̵�
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
public:
	MemoryPool()
	{
		memset(_chunkTable, 0, sizeof(_chunkTable));

		int index = 1;
		for (int i = 0; i < POOL_SIZE; ++i)
		{
			int chunk_size = CHUNK_TABLE_SIZE * (4 * (i + 4));

			if (chunk_size % MEMORY_ALLOCATION_ALIGNMENT == 0)
			{
				_pool[i] = new boost::pool<>(chunk_size);
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

		return allocMemoryHeader(header, chunkSize);
	}

	void free(void* mem_)
	{
		// �޸� �ش� ȹ��
		MemoryHeader* header = (MemoryHeader*)mem_;
		--header;

		size_t chunkSize = header->_size;
		size_t index = (chunkSize / CHUNK_TABLE_SIZE);

		// �޸� Ǯ ���̺� ����
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
	boost::pool<>* _chunkTable[CHUNK_TABLE_SIZE];
	boost::pool<>* _pool[POOL_SIZE];
};

EXTERN_MGR(MemoryPool);

}

