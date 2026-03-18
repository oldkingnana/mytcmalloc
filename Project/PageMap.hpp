#pragma once 

#include "global.hh"
#include "common_struct.hpp"

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
			return (uint64_t)pointer >> 12;	
		}

		Span* PointerToSpan(void* pointer)
		{
			if(map_.find(PointerToPageID(pointer)) != map_.end())
				return map_[PointerToPageID(pointer)];
			else 
				return nullptr;
		}

		Span* PageIDToSpan(PageID id)
		{
			if(map_.find(id) != map_.end())
				return map_[id];
			else 
				return nullptr;
		}

		void newIndex(Span* span)
		{
			auto pagenum = span->PageNum_;
			for(uint32_t i = 0; i < pagenum; i++)
				map_.emplace(span->ID_ + i, span);
		}

		void delIndex(void* pointer)
		{
			delIndex(PointerToPageID(pointer));
		}
		
		void delIndex(Span* span)
		{
			delIndex(span->ID_);
		}

		void delIndex(PageID id)
		{
			auto beginid = map_[id]->ID_;
			auto pagenum = map_[id]->PageNum_;

			for(uint32_t i = 0; i < pagenum; i++)
				map_.erase(beginid + i);
		}
	private:
		std::unordered_map<PageID, Span*> map_;
	};
}




