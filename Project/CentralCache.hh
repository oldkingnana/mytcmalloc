#pragma once

#include "common_struct.hpp"
#include "Utils.hpp"
#include "global.hh"
#include "mutex.hpp"

namespace oldking
{
	class CentralCache
	{
	private:
		CentralCache()
		{}

		~CentralCache()
		{}

	public:
		uint32_t FetchRangeObj(void*& start, void*& end, uint32_t batch_num, uint32_t size_class);

		void ReleaseListToSpans(void* start, uint32_t batch_num, uint32_t size_class);

		static CentralCache& GetInstance()
		{
			static CentralCache centralcache_instance;
			return centralcache_instance;
		}

	private:
		Span* GetOneSpan(uint32_t batch_num, uint32_t size_class);

		Span* FindSpan(void* pointer, uint32_t size_class);

		void InsertObj(void* pointer, Span* span);	

	private:
		SpanList FT_[FT_BUCKET_NUM_TOTAL];	
		oldking::mymutex MutexList[FT_BUCKET_NUM_TOTAL];
	};
}


