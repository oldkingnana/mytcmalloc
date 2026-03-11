#pragma once 

#include <stdint.h>

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

	protected:
		void* header_;
	};
	

	class span : public FreeList
	{
	public:
		span(void* start_addr, uint32_t page_num, uint32_t obj_size)
		: ID_(0)
		, PageBegin_(static_cast<char*>(start_addr))
		, PageNum_(page_num)
		, prevSpan_(nullptr)
		, nextSpan_(nullptr)
		, objSize_(obj_size)
		, objNum_(0)
		, useCount_(0)
		, isUse_(false)
		{
			char* start = (char*)start_addr;
			uint32_t free_len = SP_PAGE_LEN * PageNum_;

			while(free_len >= obj_size) // cut
			{
				void* obj = start;
				start = (char*)obj + obj_size;
				free_len -= obj_size;
				push(obj);
				objNum_ += 1;
			}

			if(free_len != 0)
			{
				// warning
			}
		}

		~span()
		{}

		void* pop_list(uint32_t num)
		{
			if(num > objNum_ || num == 0)
			{
				return nullptr;
			}
			
			void* cur = header_;
			for(uint32_t count = 1; cur != nullptr; cur = *(void**)cur, count++)
			{	
				if(count == num)
					break;
			}
			void* ret = header_;
			header_ = *(void**)cur;

			useCount_ += num;
			objNum_ -= num;
			isUse_ = true;

			return ret;
		}
		
		bool push_list(void* header, uint32_t num)
		{
			if(num == 0)
				return false;

			void* cur = header;
			for(uint32_t count = 1; count != num; cur = *(void**)cur, count++)
			{}

				

			return {};
		}

	private:
		PageID ID_;
		char* PageBegin_;
		uint32_t PageNum_;

		span* prevSpan_;
		span* nextSpan_;
	
		uint32_t objSize_;
		uint32_t objNum_;

		uint16_t useCount_;

		bool isUse_;
	};

}

