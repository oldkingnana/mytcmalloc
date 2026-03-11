#pragma once

#include "myexception.hpp"
#include "global.hh"
#include "common_struct.hpp"

#include <string>
#include <stdint.h>
#include <vector>
#include <cmath>

namespace oldking 
{
	// bad
	class SizeIndexMapper
	{
	public:
		SizeIndexMapper()
		{}

		~SizeIndexMapper()
		{}

		static int16_t table_pos(uint32_t size)
		{
			int16_t pos = -1;
		
			if(size == 0)
				return pos;
			if(size > FT_MAX_BLOCK)
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
			uint32_t begin = 1, end = begin + (FT_BUCKET_NUM * pow(2, time) * FT_BASE_ALIGNMENT);
			
			while(size > 96 && begin < FT_MAX_BLOCK)
			{
				time++;
				begin = end;
				end = begin + (FT_BUCKET_NUM * pow(2, time) * FT_BASE_ALIGNMENT);
				if(begin <= size && size < end)
					break;
			}
			uint32_t pos_begin = (time) * FT_BUCKET_NUM;
			pos = pos_begin + ((size - begin == 0) ? 0 : (size - begin) / (pow(2, time) * FT_BASE_ALIGNMENT));
		
			return pos;
		}
	};

	class FL_FreeTable : public SizeIndexMapper
	{
	public:
		FL_FreeTable(uint16_t num)
		: table_(num)
		{}

		~FL_FreeTable()
		{}

		bool push(void* pointer, uint32_t size)
		{
			int16_t pos = table_pos(size);
			if(pos == -1)
			{
				oldking::my_exception ex(std::string("FreeTable::push -> pos == -1") + " pointer == " + std::to_string((long long)pointer) + " size == " + std::to_string(size));
				return false;
			}
		
			table_[pos].push(pointer);
			return true;
		}

		void* pop(uint32_t size)
		{
			return table_[table_pos(size)].pop();
		}

		bool find(uint32_t size)
		{
			return !(table_[table_pos(size)].is_empty());
		}

	private:
		std::vector<FreeList> table_;
	};


	class SL_FreeTable : public SizeIndexMapper
	{
	public:
		SL_FreeTable()
		: table_()
		{}

		~SL_FreeTable()
		{}

		bool push(Span* newspan, uint32_t size)
		{
			int16_t pos = table_pos(size);
			if(pos == -1)
			{
				oldking::my_exception ex(std::string("FreeTable::push -> pos == -1") + " pointer == " + std::to_string((long long)newspan) + " size == " + std::to_string(size));
				return false;
			}
		
			table_[pos].push_back(newspan);
			return true;
		}

		Span* pop(uint32_t size)
		{
			return table_[table_pos(size)].pop_back();
		}

		bool find(uint32_t size)
		{
			return !(table_[table_pos(size)].is_empty());
		}

	private:
		std::vector<SpanList> table_;
	};
}



