.PHONY: debug release clean PageCache_test CentralCache_test Benchmark_test

debug: 
	g++ -std=c++20 -lpthread test.cc ThreadCache.cc alloc.cc -o test_exe_debug -g 

release:
	g++ -std=c++20 -lpthread test.cc ThreadCache.cc alloc.cc -o test_exe_release 

PageCache_test:
	g++ -std=c++14 PageCacheTest.cc PageCache.cc -o test_pagecache -lpthread

CentralCache_test:
	g++ -std=c++14 PageCacheTest.cc PageCache.cc CentralCache.cc -o test_centralcache -lpthread

Benchmark_test:
	g++ -O3 -std=c++14 test_benchmark.cc CentralCache.cc PageCache.cc ThreadCache.cc alloc.cc -o test_benchmark -lpthread

clean:
	rm -f log.txt test_exe* core* test_pagecache test_centralcache


