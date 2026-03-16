#pragma once 

#include "common_struct.hpp"
#include "global.hh"

#include <list>

namespace oldking
{
	template<typename obj_type>
	class ObjectPool
	{
	public:
		ObjectPool()
		: obj_list_({})
		, free_count_(0)
		, use_count_(0)
		{}

		~ObjectPool()
		{}
		
		obj_type* get()
		{
			if(free_count_ == 0)
				new_obj((bool)use_count_ ? use_count_ * 2 : OP_INIT_NUM); // todo
			obj_type* ret = obj_list_.front();
			obj_list_.pop_front();
			use_count_++;
			free_count_--;
			return ret;
		}

		void put_back(obj_type* obj)
		{
			obj_list_.push_front(obj);
			use_count_--;
			free_count_++;
		}

		uint32_t free_count() { return free_count; }

		bool is_empty() { return free_count_ == 0; }

	private:
		inline void new_obj(uint32_t k)
		{
			for(uint32_t i = 0; i < k; i++)
				obj_list_.push_front(new obj_type);
			free_count_ += k;
		}

	private:
		std::list<Span*> obj_list_; // uinque_ptr is better?
		uint32_t free_count_;
		uint32_t use_count_;
	};
}

