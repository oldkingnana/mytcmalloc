#pragma once 

#include "global.hh"
#include "common_struct.hpp"
#include "mutex.hpp" 

#include <unordered_map>

namespace oldking
{
	class PageMap 
	{
	private:
		PageMap()
		{}

		~PageMap()
		{}

	public:
		static PageMap& GetIns()
		{
			static PageMap pagemap_ins;
			return pagemap_ins;
		}

		static PageID PointerToPageID(void* pointer)
		{
			return (uint64_t)pointer >> SP_PAGE_SHIFT;	
		}

		Span* PointerToSpan(void* pointer)
		{
			assert(pointer);
			mutex_.Lock();
			Span* ret = nullptr;
			if(map_.find(PointerToPageID(pointer)) != map_.end())
				ret = map_[PointerToPageID(pointer)];
			mutex_.Unlock();
			return ret;
		}

		Span* PageIDToSpan(PageID id)
		{
			mutex_.Lock();
			Span* ret = nullptr;
			if(map_.find(id) != map_.end())
				ret = map_[id];
			mutex_.Unlock();
			return ret;
		}

		void newIndex(Span* span)
		{
			assert(span);
			mutex_.Lock();
			auto pagenum = span->PageNum_;
			for(uint32_t i = 0; i < pagenum; i++)
				map_[span->ID_ + i] = span;
			mutex_.Unlock();
		}

		void delIndex(void* pointer)
		{
			delIndex(PointerToPageID(pointer));
		}
		
		void delIndex(Span* span)
		{
			assert(span);
			delIndex(span->ID_);
		}

		void delIndex(PageID id)
		{
			mutex_.Lock();
			if(map_.find(id) == map_.end())
				return ;
			auto beginid = map_[id]->ID_;
			auto pagenum = map_[id]->PageNum_;

			for(uint32_t i = 0; i < pagenum; i++)
				map_.erase(beginid + i);
			mutex_.Unlock();
		}
	private:
		std::unordered_map<PageID, Span*> map_;
		mymutex mutex_;	
	};
}




