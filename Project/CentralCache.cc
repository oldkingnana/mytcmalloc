#include "CentralCache.hh"
#include "Utils.hpp"
#include "common_struct.hpp"
#include "global.hh"
#include "PageCache.hh"
#include "PageMap.hpp"

#include <cmath>

uint32_t oldking::CentralCache::FetchRangeObj(void*& start, void*& end, uint32_t batch_num, uint32_t size_class)
{
	MutexList[SizeClass::table_pos(size_class)].Lock();
	
	oldking::Span* newspan = GetOneSpan(batch_num, size_class);

	start = newspan->header_;

	void* cur = nullptr;
	uint32_t count = 0;
	for(; count < newspan->objNum_ && count < batch_num; count++)
	{
		if(cur == nullptr)
			cur = start;
		else 
			cur = *(void**)cur;
	}

	end = cur;
	newspan->header_ = *(void**)cur;
	*(void**)end = nullptr;
	newspan->objNum_ -= count;
	newspan->useCount_ += count;

	MutexList[SizeClass::table_pos(size_class)].Unlock();

	return count;
}

void oldking::CentralCache::ReleaseListToSpans(void* start, uint32_t batch_num, uint32_t size_class)
{
	assert(start);
	assert(batch_num != 0);
	assert(size_class != 0);

	Span* span = nullptr;
	
	MutexList[SizeClass::table_pos(size_class)].Lock();

	void* cur_obj = start;
	for(uint32_t count = 0; count < batch_num; count++)
	{
		void* next = *(void**)cur_obj;

		if(span == nullptr || span->ID_ != PageMap::GetIns().PointerToPageID(cur_obj))
			span = PageMap::GetIns().PointerToSpan(cur_obj);
		
		InsertObj(cur_obj, span);

		// release span to PageCache
		if(span->useCount_ == 0)
		{
			FT_[SizeClass::table_pos(size_class)].erase(span);
			span->objNum_ = 0;
			span->objSize_ = 0;
			span->header_ = nullptr;

			PageCache::GetInstance().ReleaseSpanToPageCache(span);
			span = nullptr;
		}

		cur_obj = next;
	}
	
	MutexList[SizeClass::table_pos(size_class)].Unlock();
}

oldking::Span* oldking::CentralCache::GetOneSpan(uint32_t batch_num, uint32_t size_class)
{
	// find a spare Span 
	SpanList* SL = &FT_[SizeClass::table_pos(size_class)];
	
	Span* max_span = nullptr;
	for(auto it = SL->begin(); it != SL->end(); it = it->nextSpan_)
	{
		if(max_span == nullptr)
			max_span = it;

		if(max_span->objNum_ < it->objNum_)
			max_span = it;
	}
	
	// try to get a Span from FreeTable
	if(max_span != nullptr && max_span->objNum_ > 0)
		return max_span;
	// try to get a Span from PageCache
	else 
	{
		Span* newspan = oldking::PageCache::GetInstance().newSpan((batch_num * size_class + SP_PAGE_LEN - 1) >> SP_PAGE_SHIFT); // good design
		InitSpan(newspan, size_class);
		FT_[SizeClass::table_pos(size_class)].push_front(newspan);
		return newspan;
	}
}

void oldking::CentralCache::InsertObj(void* pointer, Span* span)
{
	*(void**)pointer = span->header_;
	span->header_ = pointer;

	span->useCount_ -= 1;
	span->objNum_ += 1;
}

void oldking::CentralCache::InitSpan(Span* span, uint32_t ObjSize)
{
	span->prevSpan_ = nullptr;
	span->nextSpan_ = nullptr;
	span->objSize_ = ObjSize;
	span->objNum_ = span->PageNum_ * SP_PAGE_LEN / ObjSize;
	span->useCount_ = 0;
	span->header_ = nullptr;
	span->state_ = SpanState::IN_CENTRAL_CACHE;

	// cut
	char* cur = (char*)span->PageBegin_;
	span->header_ = cur;
	for(uint32_t i = 0; i < span->objNum_ - 1; i++)
	{
		*(void**)cur = cur + ObjSize;
		cur += ObjSize;
	}
	*(void**)cur = nullptr;
}
