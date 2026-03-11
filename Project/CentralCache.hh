#pragma once

#include "common_struct.hpp"
#include "FreeTable.hpp"

namespace oldking
{
	class CentralCache
	{
	public:
		CentralCache()
		: FT_()
		{}

		~CentralCache()
		{}

	private:
		oldking::SL_FreeTable FT_;	
	};
}


