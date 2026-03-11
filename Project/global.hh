#pragma once

#include <stdint.h>


namespace oldking 
{
	const uint32_t TC_MAX = 1024*1024*1024;
	const uint32_t FT_BUCKET_NUM = 12;  // after every 12 buckets, the alignment will double
	const uint32_t FT_BASE_ALIGNMENT = 8;
	const uint32_t FT_MAX_BLOCK = 256*1024;
	const uint32_t FT_BUCKET_NUM_TOTAL = 12*12;  // after every 12 buckets, the alignment will double
	
	const uint32_t SP_PAGE_LEN = 4 * 1024;


	typedef uint32_t PageID;
}





