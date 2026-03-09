.PHONY: debug release clean
debug: 
	g++ -std=c++20 -lpthread test.cc ThreadCache.cc alloc.cc -o test_exe_debug -g 

release:
	g++ -std=c++20 -lpthread test.cc ThreadCache.cc alloc.cc -o test_exe_release 

clean:
	rm -f log.txt test_exe* core* 


