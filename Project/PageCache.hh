#pragma once 

#include "common_struct.hpp"
#include "ObjectPool.hpp"
#include "global.hh"
#include "mutex.hpp"
#include "Utils.hpp"

#include <stdint.h>

namespace oldking
{
	class PageCache 
	{
	private:
		PageCache()
		{}

		~PageCache()
		{}

		PageCache(const PageCache& other) = delete;
		PageCache& operator=(const PageCache& other) = delete;

	public:
		static PageCache& GetInstance()
		{
			static PageCache pagecache_instance;
			return pagecache_instance;
		}

		Span* newSpan(uint16_t k);

		void ReleaseSpanToPageCache(Span* span);
		static PageID GetPageID(void* pointer);

	private:
		void SplitSpan(Span* srcSpan, Span*& dstSpanA, Span*& dstSpanB, uint32_t SpanAPageNum);
		
		void MergeSpan(Span* srcSpanA, Span* srcSpanB, Span*& dstSpan);

		void InitSpan(Span* span, void* header, uint32_t PageNum);
		void ResetSpan(Span* span);
		void ClearSpan(Span* span);
		void DelSpanfromSpanLists(Span* span);
		Span* AllocfromSystem(uint32_t PageNum);
		Span* newSpan_(uint32_t srcPageNum, uint32_t dstPageNum);

	private:
		ObjectPool<Span> obj_pool_;
		SpanList span_lists_[PC_BUCKET_NUM];
		// mymutex mutexlist_[PC_BUCKET_NUM];
		mymutex mutex_;
		PageIDSpanMap map_;	
	};
}

