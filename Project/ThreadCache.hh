#pragma once

#include "myeasylog.hpp"
#include <sys/mman.h>

namespace oldking
{
	#define TC_MAX 1024*1024*1024
	#define TC_BUCKET_NUM 32768
	#define TC_MEMORY_BLOCK_MIN_SIZE 8

	class ThreadCache
	{
	public:
		typedef void* pfree_list;
		
		ThreadCache()
		: memory_(nullptr)
		, free_table_(TC_BUCKET_NUM, nullptr)
		, free_size_(0)
		{}

		~ThreadCache()
		{}

		void* New(uint32_t size);

		void Delete(void* obj, uint32_t size);

		uint32_t FreeSize()
		{
			return free_size_;
		}
	
	private:
		uint16_t table_pos(uint32_t size);

	private:
		char* memory_;
		std::vector<pfree_list> free_table_;
		uint32_t free_size_;
	};
}

