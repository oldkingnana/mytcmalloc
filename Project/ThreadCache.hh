#pragma once

#include "myeasylog.hpp"
#include "FreeTable.hpp"
#include "CentralCache.hh"
#include "common_struct.hpp"


#include <cstdint>
#include <sys/mman.h>
#include <pthread.h>


// #include <unordered_map>

namespace oldking
{

	//static const std::unordered_map<uint32_t, uint32_t> AlignmentList = 
	//	{{96, 8}, {288, 16}, {672, 32}, {1440, 64}, {2976, 128}, {6048, 256}, {12192, 512}, {24480, 1024}, {49056, 2048}, {98208, 4096}, {196512, 8192}, {262144, 16384}};
	// 1~96       -> 8    12
	// 97~288     -> 16   12
	// 289~672    -> 32   12 
	// 673~1440   -> 64   12 
	// 1441~2976  -> 128  12 

	class ThreadCache
	{
	public:
		typedef void* pfree_list;
		
		ThreadCache()
		: FT_(FT_BUCKET_NUM_TOTAL)
		, free_size_(0)
		{}

		~ThreadCache()
		{}

		void* allocate(uint32_t size);

		bool deallocate(void* obj, uint32_t size);

		uint32_t FreeSize()
		{
			return free_size_;
		}

		bool get_span(oldking::CentralCache* pCC, uint32_t size);

	private:
		FreeTable<FreeList> FT_;
		uint32_t free_size_;
	};

	static __thread ThreadCache* pthreadcache = nullptr;	
}

