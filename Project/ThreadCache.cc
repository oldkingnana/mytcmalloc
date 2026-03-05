#include "ThreadCache.hh"


void* oldking::ThreadCache::New(uint32_t size)
{
	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new begin");
	uint16_t pos = table_pos(size);
	void* pobj = nullptr;
	if(free_table_[pos] == nullptr)
	{
		if(free_size_ < size)	
		{
			// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "mmap!");
		  	memory_ = (char*)mmap(
								  NULL,
								  TC_MAX,
								  PROT_READ | PROT_WRITE,
								  MAP_ANONYMOUS | MAP_PRIVATE,
								  -1,
								  0
								  );
			free_size_ += TC_MAX;
			if(memory_ == MAP_FAILED)
				throw std::bad_alloc();
		}

		// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new from memory_!");
		// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "free_size_: " + std::to_string(free_size_));
		pobj = (void*)memory_;
		memory_ += size;
		free_size_ -= size;
	}
	else 
	{
		// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new from free_list_!");
		pobj = (void*)free_table_[pos];
		free_table_[pos] = *((void**)pobj);
	}
	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new finish, new obj!");
	return pobj;
}	

void oldking::ThreadCache::Delete(void* obj, uint32_t size)
{
	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "delete begin");
	uint16_t pos = table_pos(size);
	*((void**)obj) = free_table_[pos];
	free_table_[pos] = obj;
	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "delete finish");
}



uint16_t oldking::ThreadCache::table_pos(uint32_t size)
{
	return (TC_BUCKET_NUM * TC_MEMORY_BLOCK_MIN_SIZE) / size;  	
}

