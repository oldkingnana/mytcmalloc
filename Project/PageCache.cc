#include "PageCache.hh"
#include "common_struct.hpp"
#include "global.hh"
#include "Utils.hpp"

#include <cstdint>
#include <tuple>
#include <algorithm>

oldking::Span* oldking::PageCache::newSpan(uint16_t k)
{
	for(uint16_t i = k; i <= PC_BUCKET_NUM; i++)
	{
		if()
		{

		}
		else 
		{

		}
	}
}

void oldking::PageCache::ReleaseSpanToPageCache(oldking::Span* span)
{
	// reset span 
	ResetSpan(span);

	while(true)
	{
		
	}

	// span_lists_[].push_front(span);
}

void oldking::PageCache::SplitSpan(Span* srcSpan, Span*& dstSpanA, Span*& dstSpanB, uint32_t SpanAPageNum)
{
	if(srcSpan->PageNum_ == 1)
		return ;

	if(srcSpan->PageNum_ <= SpanAPageNum)
		return ;

	dstSpanB = obj_pool_.get();
	dstSpanA = srcSpan;

	uint32_t SpanBPageNum = dstSpanA->PageNum_ - SpanAPageNum;

	// init A span 
	InitSpan(dstSpanA, dstSpanA->PageBegin_, SpanAPageNum);
	InitSpan(dstSpanB, (char*)dstSpanA->PageBegin_ + (SP_PAGE_LEN * SpanAPageNum), SpanBPageNum);	
}
		
void oldking::PageCache::MergeSpan(Span* srcSpanA, Span* srcSpanB, Span*& dstSpan)
{
	if(srcSpanB->PageBegin_ > srcSpanA->PageBegin_)
		std::swap(srcSpanA, srcSpanB);

	srcSpanA->PageNum_ += srcSpanB->PageNum_;

	dstSpan = srcSpanA;
	ClearSpan(srcSpanB);
	obj_pool_.put_back(srcSpanB);
}

void oldking::PageCache::InitSpan(Span* span, void* PageBegin, uint32_t PageNum)
{
	span->ID_ = GetPageID(PageBegin);
	span->PageBegin_ = PageBegin;
	span->PageNum_ = PageNum;
}

void oldking::PageCache::ResetSpan(Span* span)
{
	span->prevSpan_ = nullptr;
	span->nextSpan_ = nullptr;
	span->objSize_ = 0;
	span->objNum_ = 0;
	span->useCount_ = 0;
	span->header_ = nullptr;
	span->isUse_ = false;
}

void oldking::PageCache::ClearSpan(Span* span)
{
	span->ID_ = 0;
	span->PageBegin_ = nullptr;
	span->PageNum_ = 0;
	ResetSpan(span);
}
