#include "ThreadCache.hh"

int main()
{
//	oldking::ThreadCache TC;
//	std::cout << TC.table_pos(16) << std::endl;
//	std::cout << TC.table_pos(15) << std::endl;
//	std::cout << TC.table_pos(1000) << std::endl;
//	std::cout << TC.table_pos(1) << std::endl;
//	std::cout << TC.table_pos(2) << std::endl;
//	std::cout << TC.table_pos(10) << std::endl;
//	std::cout << TC.table_pos(5000) << std::endl;
//	std::cout << TC.table_pos(67131) << std::endl;

	uint32_t size;
	while(true)
	{
		std::cin >> size; 
		std::cout << oldking::FreeTable::table_pos(size) << std::endl;
	}

	return 0;
}


