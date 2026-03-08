#include "ThreadCache.hh"
#include <iterator>

struct node_l
{
	long long value;
	node_l* next;
};

struct node_c
{
	char value;
	node_c* next;
};

void func()
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

//	uint32_t size;
//	while(true)
//	{
//		std::cin >> size; 
//		std::cout << oldking::FreeTable::table_pos(size) << std::endl;
//	}

	oldking::ThreadCache TC;

	node_l* header_l = nullptr;

	for(int i = 0; i < 10; i++)
	{
		node_l* newnode = static_cast<node_l*>(TC.allocate(sizeof(node_l)));
		std::cout << "header_l allocate: " << newnode << std::endl;
		newnode->next = header_l;
		newnode->value = i;
		header_l = newnode;
	}

	node_c* header_c = nullptr;

	for(int i = 0; i < 10; i++)
	{
		node_c* newnode = static_cast<node_c*>(TC.allocate(sizeof(node_c)));
		std::cout << "header_c allocate: " << newnode << std::endl;
		newnode->next = header_c;
		newnode->value = i + 10;
		header_c = newnode;
	}

	if(header_l == nullptr || header_c == nullptr)
	{
		std::cout << "nullptr!" << std::endl;
	}

	for(node_l* cur = header_l; cur != nullptr; cur = cur->next)
	{
		std::cout << cur->value << " -> " ;
	}
	std::cout << "nullptr" << std::endl;

	for(node_c* cur = header_c; cur != nullptr; cur = cur->next)
	{
		std::cout << (int)cur->value << " -> " ;
	}
	std::cout << "nullptr" << std::endl;
}

int main()
{
	try {
		func();
	} catch (...) {
		std::cout << "err" << std::endl;	
	}

	return 0;
}
