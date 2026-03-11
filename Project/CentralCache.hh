#pragma once

#include "common_struct.hpp"
#include "FreeTable.hpp"

namespace oldking
{
	class CentralCache
	{
	public:
		CentralCache()
		: HT_(0) // todo
		{}

		~CentralCache()
		{}

	private:
		oldking::FreeTable<int> HT_;	
	};
}


