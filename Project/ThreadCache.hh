#pragma once

#include "myeasylog.hpp"
#include <sys/mman.h>
// #include <unordered_map>

namespace oldking
{
	const uint32_t TC_MAX = 1024*1024*1024;
	const uint32_t TC_BUCKET_NUM = 12;  // after every 12 buckets, the alignment will double
	const uint32_t TC_BASE_ALIGNMENT = 8;
	const uint32_t TC_MAX_BLOCK = 256*1024;

	//static const std::unordered_map<uint32_t, uint32_t> AlignmentList = 
	//	{{96, 8}, {288, 16}, {672, 32}, {1440, 64}, {2976, 128}, {6048, 256}, {12192, 512}, {24480, 1024}, {49056, 2048}, {98208, 4096}, {196512, 8192}, {262144, 16384}};
	// 1~96       -> 8    12
	// 97~288     -> 16   12
	// 289~672    -> 32   12 
	// 673~1440   -> 64   12 
	// 1441~2976  -> 128  12 


	class FreeList
	{
	public:
		FreeList()
		: header_(nullptr)
		{}

		~FreeList()
		{}

		void push(void* pointer);

		void* pop();

	private:
		void* header_;
	};

	class FreeTable
	{
	public:
		FreeTable()
		: table_()
		{}

		~FreeTable()
		{}

		bool push(void* pointer, uint32_t size);

		void* pop(uint32_t size);

	//private:
		static int16_t table_pos(uint32_t size);

	private:
		std::vector<FreeList> table_;
	};

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
	
		uint16_t table_pos(uint32_t size);

	private:
		char* memory_;
		std::vector<pfree_list> free_table_;
		uint32_t free_size_;
	};
}

