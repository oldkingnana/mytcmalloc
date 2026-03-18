#include "PageCache.hh"
#include "common_struct.hpp"

void testfunc1()
{
	oldking::Span* newspan = oldking::PageCache::GetInstance().newSpan(10);
	assert(newspan->ID_);
	assert(newspan->PageBegin_);
	assert(newspan->PageNum_ == 10);
	assert(newspan->prevSpan_ == nullptr);
	assert(newspan->nextSpan_ == nullptr);
}

int main()
{
	
	return 0;
}


