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
		, num_(0)
		{}

		~FreeList()
		{}

		void push(void* pointer) 
		{
			*((void**)pointer) = header_;
			header_ = pointer;
			num_++;
		}

		void* pop()
		{
			if(header_ == nullptr)
				return nullptr;
			void* obj = header_;
			header_ = *((void**)obj);
			num_--;
			return obj;
		}

		void push_list(void* start, void* end, uint32_t num)
		{
			*(void**)end = header_;
			header_ = start;
			num_ += num;
		}

		bool pop_list(void*& start, void*& end, uint32_t num)
		{
			if(num_ < num)
				return false;
			
			void* cur = nullptr;
			for(uint16_t i = 0; i < num; i++)
			{
				if(cur == nullptr)
					cur = header_;
				else 
					cur = *(void**)cur;
			}
		
			start = header_;
			end = cur;

			header_ = *(void**)end;
			*(void**)end = nullptr;
			num_ -= num;

			return true;
		}

		bool is_empty() { return header_ == nullptr; } 

	public:
		void* header_;
		uint32_t num_;
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
		, header_()
		, state_(SpanState::NOT_INIT)
		{}

		~Span()
		{}

	public:
		// ======= init in PageCache ======
		PageID ID_;
		void* PageBegin_;
		uint32_t PageNum_;

		// ===== init in CentralCache =====
		Span* prevSpan_;
		Span* nextSpan_;
	
		uint32_t objSize_;
		uint32_t objNum_;

		uint32_t useCount_;

		void* header_;

		SpanState state_;
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
			assert(!is_empty());
			Span* ret = header_->prevSpan_;
			erase(ret);	
			return ret;
		}

		Span* pop_front()
		{
			assert(!is_empty());
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

