
// AI

#include "CentralCache.hh"
#include "PageCache.hh"
#include "PageMap.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <mutex>

using namespace oldking;

std::mutex print_mtx;
void Log(const std::string& msg) {
    std::lock_guard<std::mutex> lock(print_mtx);
    std::cout << "[TEST] " << msg << std::endl;
}

// 1. 基础批发与回收测试
void TestCentralCache_Basic() {
    Log("--- Start Basic CentralCache Test ---");
    
    void* start = nullptr;
    void* end = nullptr;
    uint32_t batch_num = 10;
    uint32_t size_class = 16; // 申请 16 字节大小的对象

    // 向 CentralCache 批发 10 个 16 字节的对象
    uint32_t actual_fetched = CentralCache::GetInstance().FetchRangeObj(start, end, batch_num, size_class);
    
    assert(actual_fetched > 0);
    assert(start != nullptr);
    assert(end != nullptr);

    // 遍历链表，在每个对象的后 8 个字节（偏移量为 8）写入魔法值验证
    void* cur = start;
    uint32_t count = 0;
    while (cur != nullptr) {
        int* data = (int*)((char*)cur + 8); // 避开前 8 字节的 next 指针
        *data = 0xDEADBEEF; // 写入魔法值
        
        count++;
        if (cur == end) break;
        cur = *(void**)cur;
    }
    assert(count == actual_fetched);

    // 验证并回收
    cur = start;
    while (cur != nullptr) {
        int* data = (int*)((char*)cur + 8);
        assert(*data == (int)0xDEADBEEF); // 确保值没被篡改
        
        if (cur == end) break;
        cur = *(void**)cur;
    }

    // 连带头节点一起还给 CentralCache
    CentralCache::GetInstance().ReleaseListToSpans(start, actual_fetched, size_class);
    
    Log("Basic CentralCache Test: PASS");
}

// 2. 多线程高并发批发与回收压测
void ConcurrentWorker(int thread_id) {
    // 模拟 ThreadCache 的行为，不同线程请求不同的 SizeClass
    uint32_t size_class = (thread_id % 4 + 1) * 16; // 16, 32, 48, 64 字节
    
    for (int i = 0; i < 5000; i++) {
        void* start = nullptr;
        void* end = nullptr;
        uint32_t batch_num = 32; // 每次批发 32 个

        uint32_t fetched = CentralCache::GetInstance().FetchRangeObj(start, end, batch_num, size_class);
        if (fetched == 0) continue;

        // 写入 ThreadID 标记
        void* cur = start;
        while (cur != nullptr) {
            int* data = (int*)((char*)cur + 8);
            *data = thread_id; 
            
            if (cur == end) break;
            cur = *(void**)cur;
        }

        // 模拟业务处理耗时，让线程交错，制造锁竞争
        std::this_thread::yield();

        // 验证并释放
        cur = start;
        while (cur != nullptr) {
            int* data = (int*)((char*)cur + 8);
            assert(*data == thread_id); // 如果断言失败，说明桶锁没锁住，内存发生交叉践踏！
            
            if (cur == end) break;
            cur = *(void**)cur;
        }

        CentralCache::GetInstance().ReleaseListToSpans(start, fetched, size_class);
    }
}

void TestCentralCache_Concurrent() {
    Log("--- Start Concurrent Stress Test ---");
    const int THREAD_COUNT = 12; // 开启 12 个线程狂轰滥炸
    std::vector<std::thread> threads;

    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.push_back(std::thread(ConcurrentWorker, i));
    }

    for (auto& t : threads) {
        t.join();
    }
    Log("Concurrent Stress Test: PASS (Bucket Locks are Rock Solid!)");
}

int main() {
    Log("Starting CentralCache Integration Tests...");
    
    TestCentralCache_Basic();
    TestCentralCache_Concurrent();

    Log("All tests finished successfully!");
    return 0;
}
