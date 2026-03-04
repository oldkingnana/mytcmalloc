#pragma once 

#include <iostream>
#include <new>
#include <stddef.h>
#include <string>
#include <unistd.h>
#include <sys/mman.h>

#include "myeasylog.hpp"

namespace oldking
{
	#define MMAP_LEN 64

	template<class T>
	class ObjectPool
	{
	public:
		ObjectPool()
		: memory_(nullptr)
		, free_list_(nullptr)
		, free_size_(0)
		, len_(0)
		{
			len_ = ((MMAP_LEN / sizeof(T)) + 1) * sizeof(T);
		}

		~ObjectPool()
		{}

		T* New()
		{
			MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new begin");
			T* pobj = nullptr;
			if(free_list_ == nullptr)
			{
				if(free_size_ < sizeof(T))	
				{
					MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "mmap!");
				  	memory_ = (char*)mmap(
										  NULL,
										  len_,
										  PROT_READ | PROT_WRITE,
										  MAP_ANONYMOUS | MAP_PRIVATE,
										  -1,
										  0
										  );
					free_size_ += len_;
					if(memory_ == MAP_FAILED)
						throw std::bad_alloc();
				}

				MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new from memory_!");
				MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "free_size_: " + std::to_string(free_size_));
				pobj = (T*)memory_;
				memory_ += sizeof(T);
				free_size_ -= sizeof(T);
			}
			else 
			{
				MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new from free_list_!");
				pobj = (T*)free_list_;
				free_list_ = *((void**)pobj);
			}
			new(pobj)T();
			MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "new finish, new obj!");
			return pobj;
		}	

		void Delete(T* obj)
		{
			MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "delete begin");
			obj->~T();
			*((void**)obj) = free_list_;
			free_list_ = obj;
			MyEasyLog::GetInstance().WriteLog(LOG_INFO, "ConcurrentMemoryPool", "delete finish");
		}

		size_t FreeSize()
		{
			return free_size_;
		}

	private:
		char* memory_;
		void* free_list_;
		size_t free_size_;
		size_t len_;
	};
}

