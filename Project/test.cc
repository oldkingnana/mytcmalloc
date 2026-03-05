#include "ConcurrentMemoryPool.hpp"
#include <iostream>
#include <vector>

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

void print_vector(const std::vector<char*>& v)
{
	for(auto& it : v)
	{
		std::cout << (int)(*it) << " ";
	}
	std::cout << std::endl;
}

void object_pool_test_1()
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
}

void object_pool_test_2()
{
	int N = 100000;
	int time = 3;

	// glibc new&delete test
	auto begin1 = clock();
	for(int n = 0; n < time; n++)
	{
		list_node<long long>* head1 = nullptr;
		for(long long i = 0; i < N; i++)
		{
			list_node<long long>* newnode = new list_node<long long>();
			newnode->value1 = i;
			newnode->value2 = i;
			newnode->next = head1;
			head1 = newnode;
		}
	
		for(long long i = 0; i < N; i++)
		{
			list_node<long long>* deletenode = head1;
			head1 = deletenode->next;
			delete deletenode;
		}
	}	
	auto end1 = clock();



	// ConcurrentMemoryPool test
	oldking::ObjectPool<list_node<long long>> pool;

	auto begin2 = clock();
	for(int n = 0; n < time; n++)
	{
		list_node<long long>* head2 = nullptr;
	
		for(long long i = 0; i < N; i++)
		{
			list_node<long long>* newnode = pool.New();
			newnode->value1 = i;
			newnode->value2 = i;
			newnode->next = head2;
			head2 = newnode;
		}
	
		for(long long i = 0; i < N; i++)
		{
			list_node<long long>* deletenode = head2;
			head2 = deletenode->next;
			pool.Delete(deletenode);
		}
	}
	auto end2 = clock();

	std::cout << "glibc new & delete: " << end1 - begin1 << std::endl;
	std::cout << "ConcurrentMemoryPool: " << end2 - begin2 << std::endl;
}

void object_pool_test_3()
{
	oldking::ObjectPool<char> pool;
	std::vector<char*> v;
	for(int i = 0; i < 30; i++)
	{
		char* newchar = pool.New();
		*newchar = (char)i;
		v.push_back(newchar);
	}

	print_vector(v);	

	for(int i = 0; i < 10; i++)
	{
		pool.Delete(v.front());
		v.erase(v.begin());
	}
	
	print_vector(v);	
	
	for(int i = 30; i < 40; i++)
	{
		char* newchar = pool.New();
		*newchar = (char)i;
		v.push_back(newchar);
	}
	
	print_vector(v);	
}


int main()
{
	// object_pool_test_2();
	
	object_pool_test_3();
	
	return 0;
}
