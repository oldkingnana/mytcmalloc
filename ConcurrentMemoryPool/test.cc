#include "ConcurrentMemoryPool.hpp"
#include <iostream>

template<class T>
struct list_node
{
	T value1;
	T value2;
	struct list_node* next;
};

void print_list(list_node<long long>* head)
{
	for(auto begin = head; begin != nullptr; begin = begin->next)
	{
		std::cout << (long long)(begin->value1) << "->";
	}
	std::cout << "nullptr" << std::endl;
}

int main()
{
	oldking::ObjectPool<list_node<long long>> pool;

	list_node<long long>* head = nullptr;

	for(long long i = 0; i < 30; i++)
	{
		list_node<long long>* newnode = pool.New();
		newnode->value1 = i;
		newnode->value2 = i;
		newnode->next = head;
		head = newnode;
	}

	print_list(head);

	for(long long i = 0; i < 10; i++)
	{
		list_node<long long>* deletenode = head;
		head = deletenode->next;
		pool.Delete(deletenode);
	}

	print_list(head);
	
	for(long long i = 40; i < 50; i++)
	{
		list_node<long long>* newnode = pool.New();
		newnode->value1 = i;
		newnode->value2 = i;
		newnode->next = head;
		head = newnode;
	}
	
	print_list(head);

	return 0;
}
