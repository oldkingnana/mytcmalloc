#include "alloc.hh"
#include "ThreadCache.hh"
#include <new>
#include "myexception.hpp"
#include "PageMap.hpp"

void* oldking::alloc(uint32_t size)
{
	if(oldking::pthreadcache == nullptr)
		oldking::pthreadcache = new oldking::ThreadCache(); // ?

	void* ret = oldking::pthreadcache->allocate(size);
	if(ret == nullptr)
	{
		oldking::my_exception ex("alloc -> bad_alloc");
		throw ex;
	}

	return ret;
}

void oldking::dealloc(void* obj)
{
	if(oldking::pthreadcache == nullptr)
	{
		oldking::my_exception ex("dealloc -> pthreadcache == nullpter");
		throw ex;
	}

	if(!oldking::pthreadcache->deallocate(obj, PageMap::GetIns().PointerToSpan(obj)->objSize_))
	{
		oldking::my_exception ex("dealloc -> bad dealloc");
		throw ex;
	}
}

