#include "CentralCache.hh"
#include "Utils.hpp"
#include "common_struct.hpp"
#include "global.hh"
#include "PageCache.hh"

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

	uint32_t count = 0;
	void* cur_obj = nullptr;
	void* next = nullptr;
	
	Span* span = nullptr;
	
	MutexList[SizeClass::table_pos(size_class)].Lock();

	for(; count < batch_num; count++)
	{
		if(cur_obj == nullptr)
		{
			cur_obj = start;
			next = *(void**)start;
		}
		else
		{
			cur_obj = next;
			next = *(void**)next;
		}

		if(span == nullptr)
			span = FindSpan(cur_obj, size_class);
		
		if(span->ID_ == PageIDSpanMap::PointerToPageID(cur_obj))
			InsertObj(cur_obj, span);
		else 
		{
			span = FindSpan(cur_obj, size_class);
			InsertObj(cur_obj, span);
		}
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

		if(max_span->objSize_ > it->objSize_)
			max_span = it;
	}
	
	// try to get a Span from FreeTable
	if(max_span->objSize_ >= batch_num / 3)
		return max_span;
	// try to get a Span from PageCache
	else 
	{
		Span* newspan = oldking::PageCache::GetInstance().newSpan(std::ceil(batch_num * size_class / SP_PAGE_LEN));
		InitSpan(newspan, size_class);
		FT_[SizeClass::table_pos(size_class)].push_front(newspan);
		return {};
	}
}
		
oldking::Span* oldking::CentralCache::FindSpan(void* pointer, uint32_t size_class)
{
	auto pos = SizeClass::table_pos(size_class);
	Span* cur_span = FT_[pos].begin();
	while(cur_span != FT_[pos].end())
	{
		if(cur_span->ID_ == PageIDSpanMap::PointerToPageID(pointer))
			break;
	}
	return cur_span;
}

void oldking::CentralCache::InsertObj(void* pointer, Span* span)
{
	void* header = span->header_;

	*(void**)pointer = header;
	header = pointer;

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
	span->isUse_ = false;

	// cut
	char* cur = nullptr;
	for(uint32_t i = 0; i < span->objNum_; i++)
	{
		if(cur == nullptr)
			cur = (char*)span->PageBegin_;
		else 
			cur = cur + ObjSize;
		*(void**)cur = cur + ObjSize;
	}
	*(void**)cur = nullptr;
	span->header_ = span->PageBegin_;
}
