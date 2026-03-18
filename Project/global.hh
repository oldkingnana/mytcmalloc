#pragma once

#include <stdint.h>


namespace oldking 
{
	// ==================ThreadCache==========================
	static const uint32_t TC_MAX = 1024*1024*1024;
	static const uint32_t TC_BATCH_MAX = 32;

	// ===================FreeTable===========================
	static const uint32_t FT_BUCKET_NUM = 12;  // after every 12 buckets, the alignment will double
	static const uint32_t FT_BASE_ALIGNMENT = 8;
	static const uint32_t FT_MAX_BLOCK = 256*1024;
	static const uint32_t FT_BUCKET_NUM_TOTAL = 12*12;  // after every 12 buckets, the alignment will double
	
	// =====================Span==============================
	static const uint32_t SP_PAGE_LEN = 4 * 1024;

	// ==================ObjectPool=============================
	static const uint32_t OP_INIT_NUM = 32;

	// ==================PageCache=============================
	static const uint32_t PC_BUCKET_NUM = 129;
	static const uint32_t PC_MMAP_PAGE_NUM = 256;
	static const uint32_t PC_SPAN_PAGE_MAX_NUM = 128;
	static const uint32_t PC_MMAP_PAGE_NUM_TOTAL = 512 * 1024 * 1024;

	typedef uint64_t PageID;

	enum class SpanState
	{
		NOT_INIT = 0,
		IN_PAGE_CACHE,
		IN_CENTRAL_CACHE
	};
}





