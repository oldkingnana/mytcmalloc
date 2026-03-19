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
	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new begin");
	void* pobj = nullptr;
	auto real_size = SizeClass::round_up(size);
	auto pos = SizeClass::table_pos(real_size);
	if(FT_[pos].is_empty())
	{
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
	auto real_size = SizeClass::round_up(size);
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

