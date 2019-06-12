#include "BufferPool.h"

namespace BLUE_BERRY {
DEFINE_TLS(Buffer)
void Buffer::decRefCount()
{
	if (_refCount.fetch_sub(1) == 1)
	{
		BufferPool::getBufferPool()->release(this);
	}
}

DEFINE_MGR(BufferPool)
}