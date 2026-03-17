#include "PageCache.hh"
#include "common_struct.hpp"
#include "global.hh"
#include "Utils.hpp"

#include <cstdint>
#include <tuple>
#include <algorithm>
#include <sys/mman.h>

oldking::Span* oldking::PageCache::newSpan(uint16_t k)
{
	mutex_.Lock();

	for(uint32_t i = k; i <= PC_BUCKET_NUM; i++)
	{
		if(!span_lists_[i].is_empty())
		{
			Span* ret = newSpan_(i, k);
			return ret;
		}
	}

	// mmap
	for(uint32_t i = PC_MMAP_PAGE_NUM / PC_SPAN_PAGE_MAX_NUM; i != 0; i--)
	{
		void* memory_ = (void*)mmap(
				NULL,
				PC_MMAP_PAGE_NUM * SP_PAGE_LEN,
				PROT_READ | PROT_WRITE,
				MAP_ANONYMOUS | MAP_PRIVATE,
				-1,
				0);
		Span* newspan = obj_pool_.get();
		InitSpan(newspan, memory_, PC_SPAN_PAGE_MAX_NUM);
		span_lists_[newspan->PageNum_].push_front(newspan);
		map_.newIndex(newspan);
	}

	Span* ret = newSpan_(PC_SPAN_PAGE_MAX_NUM, k);

	mutex_.Unlock();
	return ret;
}

oldking::Span* oldking::PageCache::newSpan_(uint32_t srcPageNum, uint32_t dstPageNum)
{
	Span* span_tmp = span_lists_[srcPageNum].pop_front();
	map_.delIndex(span_tmp);
	if(span_tmp->PageNum_ == dstPageNum)
		return span_tmp;
	Span* spanA;
	Span* spanB;
	SplitSpan(span_tmp, spanA, spanB, dstPageNum);
	span_lists_[spanB->PageNum_].push_front(spanB);
	map_.newIndex(spanB);
	return spanA;
}

void oldking::PageCache::ReleaseSpanToPageCache(oldking::Span* span)
{
	mutex_.Lock();
	
	// reset span 
	ResetSpan(span);

	// merge span 
	for(bool flag = true; flag; )
	{	
		flag = false;
		Span* leftspan = map_.PageIDToSpan(span->ID_ - 1);
		Span* rightspan = map_.PageIDToSpan(span->ID_ + 1);

		// How to lock?
		// 1. lock 
		// 2. check
		// 3. use 

		if(leftspan == nullptr && rightspan == nullptr)
			break;

		Span* newspanA = nullptr;
		if(leftspan != nullptr)
		{
			if(leftspan->PageNum_ + span->PageNum_ <= SP_PAGE_LEN)
			{
				map_.delIndex(leftspan);
				DelSpanfromSpanLists(leftspan);
				MergeSpan(span, leftspan, newspanA);
				span = newspanA;
				flag = true;
			}
		}
		
		Span* newspanB = nullptr;
		if(rightspan != nullptr)
		{
			if(rightspan->PageNum_ + span->PageNum_ <= SP_PAGE_LEN)
			{
				map_.delIndex(rightspan);
				DelSpanfromSpanLists(rightspan);
				MergeSpan(span, rightspan, newspanB);
				span = newspanB;
				flag = true;
			}
		}
	}

	// insert span
	span_lists_[span->PageNum_].push_front(span);
	map_.newIndex(span);
	
	mutex_.Unlock();
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

void oldking::PageCache::DelSpanfromSpanLists(Span* span)
{
	Span* prev = span->prevSpan_;
	Span* next = span->nextSpan_;

	prev->nextSpan_ = next;
	next->prevSpan_ = prev;

	span->prevSpan_ = nullptr;
	span->nextSpan_ = nullptr;
}

