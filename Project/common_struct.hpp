#pragma once 

#include <stdint.h>
#include <assert.h>

#include "global.hh"

namespace oldking 
{
	class FreeList
	{
	public:
		FreeList()
		: header_(nullptr)
		{}

		~FreeList()
		{}

		void push(void* pointer) 
		{
			*((void**)pointer) = header_;
			header_ = pointer;
		}

		void* pop()
		{
			if(header_ == nullptr)
				return nullptr;
			void* obj = header_;
			header_ = *((void**)obj);
			return obj;
		}

		bool is_empty() { return header_ == nullptr; } 

	public:
		void* header_;
	};
	

	class Span
	{
	public:
		Span()
		: ID_(0)
		, PageBegin_(nullptr)
		, PageNum_(0)
		, prevSpan_(nullptr)
		, nextSpan_(nullptr)
		, objSize_(0)
		, objNum_(0)
		, useCount_(0)
		, isUse_(false)
		{}

		Span(void* start_addr, uint32_t page_num, uint32_t obj_size)
		: ID_(0)
		, PageBegin_(static_cast<char*>(start_addr))
		, PageNum_(page_num)
		, prevSpan_(nullptr)
		, nextSpan_(nullptr)
		, objSize_(obj_size)
		, objNum_(0)
		, useCount_(0)
		, FL_()
		, isUse_(false)
		{
			// no warning
			(void)ID_;
			(void)PageBegin_;

			(void)prevSpan_;
			(void)nextSpan_;
	
			(void)objSize_;
			
			init(start_addr, page_num, obj_size);
		}

		~Span()
		{}

		void init(void* start_addr, uint32_t page_num, uint32_t obj_size)
		{
			PageBegin_ = static_cast<char*>(start_addr);
			PageNum_ = page_num;
			objSize_ = obj_size;
		
			init_(start_addr, page_num, obj_size);
		}

		void* pop(uint32_t num)
		{
			if(num > objNum_ || num == 0)
				return nullptr;
		
			if(!is_init())
				return nullptr;

			void* cur = FL_.header_;
			for(uint32_t count = 1; cur != nullptr; cur = *(void**)cur, count++)
			{	
				if(count == num)
					break;
			}
			void* ret = FL_.header_;
			FL_.header_ = *(void**)cur;

			useCount_ += num;
			objNum_ -= num;
			isUse_ = true;

			return ret;
		}
		
		bool push(void* header, uint32_t num)
		{
			if(num == 0)
				return false;

			if(!is_init())
				return false;

			void* cur = header;
			for(uint32_t count = 1; count != num; cur = *(void**)cur, count++)
			{}

			*(void**)cur = FL_.header_;
			FL_.header_ = header;	

			return true;
		}

		bool is_init() { return FL_.header_ != nullptr; }

	private:
		void init_(void* start_addr, uint32_t page_num, uint32_t obj_size)
		{
			char* start = (char*)start_addr;
			uint32_t free_len = SP_PAGE_LEN * page_num;

			while(free_len >= obj_size) // cut
			{
				void* obj = start;
				start = (char*)obj + obj_size;
				free_len -= obj_size;
				FL_.push(obj);
				objNum_ += 1;
			}

			if(free_len != 0)
			{
				// warning
			}
		}

	public:
		PageID ID_;
		char* PageBegin_;
		uint32_t PageNum_;

		Span* prevSpan_;
		Span* nextSpan_;
	
		uint32_t objSize_;
		uint32_t objNum_;

		uint16_t useCount_;

		FreeList FL_;

		bool isUse_;
	};


	class SpanList
	{
	public:
		SpanList()
		: header_(nullptr)
		{
			header_ = new Span();
			header_->nextSpan_ = header_;
			header_->prevSpan_ = header_;
		}

		~SpanList()
		{}

		Span* begin() { return header_->nextSpan_; }

		Span* end() { return header_; }

		void push_back(Span* newSpan) // shared_ptr is better?
		{
			Span* prev = header_->prevSpan_;
			prev->nextSpan_ = newSpan;
			newSpan->prevSpan_ = prev;
			header_->prevSpan_ = newSpan;
			newSpan->nextSpan_ = header_;
		}

		void push_front(Span* newSpan)
		{
			Span* next = header_->nextSpan_;
			next->prevSpan_ = newSpan;
			newSpan->nextSpan_ = next;
			header_->nextSpan_ = newSpan;
			newSpan->prevSpan_ = header_;
		}

		Span* pop_back()
		{
			assert(header_->prevSpan_);
			Span* ret = header_->prevSpan_;
			erase(ret);	
			return ret;
		}

		Span* pop_front()
		{
			assert(header_->nextSpan_);
			Span* ret = header_->nextSpan_;
			erase(ret);	
			return ret;
		}

		void insert(Span* loacation, Span* newspan)
		{
			assert(newspan);
			assert(newspan != header_);

			Span* next = loacation;
			Span* prev = loacation->prevSpan_;

			newspan->nextSpan_ = next;
			newspan->prevSpan_ = prev;

			next->prevSpan_ = newspan;
			prev->nextSpan_ = newspan;
		}

		void erase(Span* span)
		{
			assert(span != header_);
			assert(span);

			Span* prev = span->prevSpan_;
			Span* next = span->nextSpan_;

			prev->nextSpan_ = next;
			next->prevSpan_ = prev;

			span->nextSpan_ = nullptr;
			span->prevSpan_ = nullptr;
		}

		bool is_empty() { return header_->nextSpan_ == nullptr; }

	private:
		Span* header_;
	};
}

