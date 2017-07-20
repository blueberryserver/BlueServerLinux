#include "BufferPool.h"

namespace BLUE_BERRY {
DEFINE_TLS(Buffer)

void Buffer::incRefCount()
{
	_refCount.fetch_add(1);
}

void Buffer::decRefCount()
{
	if( _refCount.fetch_sub(1) == 2)
	{
		// 버퍼 반환
		BufferPool::getBufferPool()->release(this);
	}
}

DEFINE_MGR(BufferPool)
}