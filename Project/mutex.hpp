#pragma once
#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <iostream>

namespace oldking
{
	class mymutex 
	{
	public:
		mymutex()
		{
			pthread_mutex_init(&_mutex, nullptr);
		}

		void Lock()
		{
			int tmp_errno;
			if((tmp_errno = pthread_mutex_lock(&_mutex)) != 0)
			{
				std::cerr << "pthread_mutex_lock err: " << strerror(tmp_errno) << std::endl;
			}
		}

		void Unlock()
		{
			int tmp_errno;
			if((tmp_errno = pthread_mutex_unlock(&_mutex)) != 0)
			{
				std::cerr << "pthread_mutex_lock err: " << strerror(tmp_errno) << std::endl;
			}
		}

		~mymutex()
		{
			pthread_mutex_destroy(&_mutex);
		}

	private:
		pthread_mutex_t _mutex;
	};

}

