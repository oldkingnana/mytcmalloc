#include "ThreadCache.hh"
#include "CentralCache.hh"
#include "Utils.hpp"
#include "myexception.hpp"
#include "global.hh"

#include <cmath>
#include <new>
#include <string>

// ==================ThreadCache===================

void* oldking::ThreadCache::allocate(uint32_t size)
{
	uint32_t time = 0;
    uint32_t begin = 1, end = begin + (FT_BUCKET_NUM * pow(2, time) * FT_BASE_ALIGNMENT);

    while(size > 96 && begin < FT_MAX_BLOCK)
    {
        time++;
        begin = end;
        end = begin + (FT_BUCKET_NUM * pow(2, time) * FT_BASE_ALIGNMENT);
        if(begin <= size && size < end)
                break;
    }
	
	uint32_t real_size = begin - 1 + (pow(2, time) * FT_BASE_ALIGNMENT);
	while(real_size < size)
	{
		real_size += (pow(2, time) * FT_BASE_ALIGNMENT);
		if(real_size > FT_MAX_BLOCK)
			throw std::bad_alloc();
	}

	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new begin");
	void* pobj = nullptr;

	auto pos = SizeClass::table_pos(real_size);
	if(FT_[pos].is_empty())
	{
		//if(free_size_ < real_size)	
		//{
			// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "mmap!");
		//  	memory_ = (char*)mmap(
		//						  NULL,
		//						  TC_MAX,
		//						  PROT_READ | PROT_WRITE,
		//						  MAP_ANONYMOUS | MAP_PRIVATE,
		//						  -1,
		//						  0
		//						  );
		//	free_size_ += TC_MAX;
		//	if(memory_ == MAP_FAILED)
		//		throw std::bad_alloc();
		//}

		// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new from memory_!");
		// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "free_size_: " + std::to_string(free_size_));
		// pobj = (void*)memory_;
		// memory_ += real_size;
		// free_size_ -= real_size;

	//}
	//else 
	//{
	//	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new from free_list_!");
	//	pobj = FT_.pop(real_size);
	

		void* start = nullptr;
		void* end = nullptr;
		uint16_t obj_num = oldking::CentralCache::GetInstance().FetchRangeObj(start, end, TC_BATCH_MAX, real_size);

		FT_[pos].push_list(start, end, obj_num);
	}

	pobj = FT_[pos].pop();

// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new finish, new obj!");
	
	return pobj;
}	

bool oldking::ThreadCache::deallocate(void* obj, uint32_t size)
{
	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "delete begin");
	uint32_t time = 0;
    uint32_t begin = 1, end = begin + (FT_BUCKET_NUM * pow(2, time) * FT_BASE_ALIGNMENT);

    while(size > 96 && begin < FT_MAX_BLOCK)
    {
        time++;
        begin = end;
        end = begin + (FT_BUCKET_NUM * pow(2, time) * FT_BASE_ALIGNMENT);
        if(begin <= size && size < end)
                break;
    }
	
	uint32_t real_size = begin - 1 + (pow(2, time) * FT_BASE_ALIGNMENT);
	while(real_size < size)
	{
		real_size += (pow(2, time) * FT_BASE_ALIGNMENT);
		if(real_size > FT_MAX_BLOCK)
			throw std::bad_alloc();
	}

	auto pos = SizeClass::table_pos(real_size);
	
	FT_[pos].push(obj);

	// too much obj
	if(FT_[pos].num_ > TC_BATCH_MAX * 3)
	{
		void* start = nullptr;
		void* end = nullptr;

		FT_[pos].pop_list(start, end, TC_BATCH_MAX);

		CentralCache::GetInstance().ReleaseListToSpans(start, TC_BATCH_MAX, real_size);
	}

	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "delete finish");
	return true;
}

