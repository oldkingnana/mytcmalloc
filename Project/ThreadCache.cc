#include "ThreadCache.hh"
#include <cmath>
#include <new>

// ==================FreeList====================

void oldking::FreeList::push(void* pointer)
{
	*((void**)pointer) = header_;
	header_ = pointer;
}

void* oldking::FreeList::pop()
{
	if(header_ == nullptr)
		return nullptr;
	void* obj = header_;
	header_ = *((void**)obj);
	return obj;
}

// ==================FreeTable===================

bool oldking::FreeTable::push(void* pointer, uint32_t size)
{
	int16_t pos = table_pos(size);
	if(pos == -1)
		return false;

	table_[pos].push(pointer);
	return true;
}

void* oldking::FreeTable::pop(uint32_t size)
{
	return table_[table_pos(size)].pop();
}
		
bool oldking::FreeTable::find(uint32_t size)
{
	return !(table_[table_pos(size)].is_empty());
}

int16_t oldking::FreeTable::table_pos(uint32_t size)
{
	int16_t pos = -1;

	if(size == 0)
		return pos;
	if(size > TC_MAX_BLOCK)
		return pos;
	if(size == 1)
		return 0;

	// 在不考虑动态对齐长度的情况下
	
	//  num                               case                               loss (bytes)                   loss (percent)                 alignment          
	//   0                          (0, 8] -> 8bytes                              7                           7 / 8 = 87%                       8
	//   1                          (8, 16] -> 16bytes                            7                           7 / 16 = 43%                      8 
	//   2                          (16, 24] -> 24bytes                           7                           7 / 24 = 29%                      8 
	//   3                          (24, 32] -> 32bytes                           7                           7 / 32 = 21%                      8 
	//   4                          (32, 40] -> 40bytes                           7                           7 / 40 = 17%                      8 
	//   5                          (40, 48] -> 48bytes                           7                           7 / 48 = 14%                      8 
	//  ...                               ...                                    ...                              ...                          ...
	//   X    (alignment * num, alignment * (num + 1)] -> alignment * num    alignment - 1     (alignment - 1) / (alignment * (num + 1))        8


	// hardcode (shit)

	uint32_t time = 0;
	uint32_t begin = 1, end = begin + (TC_BUCKET_NUM * pow(2, time) * TC_BASE_ALIGNMENT);
	
	while(size > 96 && begin < TC_MAX_BLOCK)
	{
		time++;
		begin = end;
		end = begin + (TC_BUCKET_NUM * pow(2, time) * TC_BASE_ALIGNMENT);
		if(begin <= size && size < end)
			break;
	}
	uint32_t pos_begin = (time) * TC_BUCKET_NUM;
	pos = pos_begin + ((size - begin == 0) ? 0 : (size - begin) / (pow(2, time) * TC_BASE_ALIGNMENT));

	return pos;
}

// ==================ThreadCache===================

void* oldking::ThreadCache::allocate(uint32_t size)
{
	uint32_t time = 0;
    uint32_t begin = 1, end = begin + (TC_BUCKET_NUM * pow(2, time) * TC_BASE_ALIGNMENT);

    while(size > 96 && begin < TC_MAX_BLOCK)
    {
        time++;
        begin = end;
        end = begin + (TC_BUCKET_NUM * pow(2, time) * TC_BASE_ALIGNMENT);
        if(begin <= size && size < end)
                break;
    }
	
	uint32_t real_size = begin - 1 + (pow(2, time) * TC_BASE_ALIGNMENT);
	while(real_size < size)
	{
		real_size += (pow(2, time) * TC_BASE_ALIGNMENT);
		if(real_size > TC_MAX_BLOCK)
			throw std::bad_alloc();
	}

	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new begin");
	void* pobj = nullptr;

	if(!FT_.find(real_size))
	{
		if(free_size_ < real_size)	
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
		memory_ += real_size;
		free_size_ -= real_size;
	}
	else 
	{
		// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new from free_list_!");
		pobj = FT_.pop(real_size);
	}
	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new finish, new obj!");
	return pobj;
}	

void oldking::ThreadCache::deallocate(void* obj, uint32_t size)
{
	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "delete begin");
	uint32_t real_size = ((FT_.table_pos(size) / TC_BUCKET_NUM) + 1) * TC_BASE_ALIGNMENT;
	FT_.push(obj, real_size);
	// MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "delete finish");
}

