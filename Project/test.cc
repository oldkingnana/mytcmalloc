#include "ThreadCache.hh"
#include "alloc.hh"
#include "myexception.hpp"

#include <ctime>
#include <random>
#include <vector>
#include <map>
#include <cstdlib>
#include <iterator>
#include <unistd.h>
#include <pthread.h>

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

void func2()
{
	//std::list<void*> list;

	//for(int i = 0; i < 10; i++)
	//	list.push_front(oldking::alloc(10));

	//for(int i = 0; i < 10; i++)
	//{
	//	oldking::dealloc(list.front(), 10);
	//	list.pop_front();
	//}

	srandom(std::time(nullptr));
	
	std::list<std::pair<void*, uint32_t>> memory_list;

	for(int i = 1, flag = 0; i <= 100000; i++) 
	{
		auto r = random() / oldking::TC_MAX_BLOCK;
		void* p = oldking::alloc(r);
		memory_list.emplace_front(p, r);
		if(flag != i / 10000)
		{
			std::cout << "i == " << i << std::endl;
			flag = i / 10000;
			sleep(4);
		}
	}

	for(int i = 1, flag = 0; !memory_list.empty(); i++)
	{
		oldking::dealloc(memory_list.front().first, memory_list.front().second);
		memory_list.pop_front();

		if(flag != i / 10000)
		{
			std::cout << "j == " << i << std::endl;
			flag = i / 10000;
			sleep(4);
		}
	}
}

void* threadfunc(void* arg)
{
	(void)arg;

	auto tid = pthread_self();
	
	srandom(std::time(nullptr));
	
	std::list<std::pair<void*, uint32_t>> memory_list;

	for(int i = 1; i <= 100; i++) 
	{
		auto r = random() / oldking::TC_MAX_BLOCK;
		void* p = oldking::alloc(r);
		memory_list.emplace_front(p, r);
		std::cout << "tid: " << tid << "  alloc " << r << "bytes" << std::endl;
	}

	for(; !memory_list.empty();)
	{
		oldking::dealloc(memory_list.front().first, memory_list.front().second);
		std::cout << "tid: " << tid << "  dealloc " << memory_list.front().second << "bytes" << std::endl;
		memory_list.pop_front();
	}
	
	return {};
}

void func3()
{
	pthread_t newthread1;
	int ret1 = pthread_create(&newthread1, NULL, threadfunc, NULL);

	if(ret1 != 0)
		std::cout << "create thread1 err" << std::endl;


	pthread_t newthread2;
	int ret2 = pthread_create(&newthread2, NULL, threadfunc, NULL);

	if(ret2 != 0)
		std::cout << "create thread2 err" << std::endl;

	pthread_join(newthread1, nullptr);
	pthread_join(newthread2, nullptr);
}

int main()
{
	try 
	{
		// func();
		// func2();	
		func3();
	}
	catch (oldking::my_exception ex)
	{
		std::cout << ex.what() << std::endl;
	}
	catch (...) 
	{
		std::cout << "unknow err" << std::endl;	
	}

	return 0;
}
