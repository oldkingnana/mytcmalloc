// AI

#include "alloc.hh"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <cstdlib> // for malloc/free

using namespace std;
using namespace std::chrono;

// 避免编译器把测试代码优化掉的哑变量
atomic<size_t> dummy_counter{0};

// 测试工作线程
void BenchmarkWorker(int rounds, int batch_size, bool use_mytcmalloc) {
    std::vector<void*> ptrs(batch_size, nullptr);
    size_t local_counter = 0;

    for (int i = 0; i < rounds; ++i) {
        // 1. 批量分配阶段
        for (int j = 0; j < batch_size; ++j) {
            // 生成 8 到 1024 字节大小的请求，避免使用带锁的 rand()
            size_t size = (j % 128) * 8 + 8; 
            
            if (use_mytcmalloc) {
                ptrs[j] = oldking::alloc(size);
            } else {
                ptrs[j] = malloc(size);
            }
            
            // 随便写点东西，触发缺页中断，模拟真实写入
            if (ptrs[j]) {
                *((char*)ptrs[j]) = 'x';
                local_counter += (size_t)ptrs[j];
            }
        }

        // 2. 批量释放阶段
        for (int j = 0; j < batch_size; ++j) {
            if (ptrs[j]) {
                if (use_mytcmalloc) {
                    oldking::dealloc(ptrs[j]);
                } else {
                    free(ptrs[j]);
                }
            }
        }
    }
    
    // 把结果喂给原子变量，彻底断绝编译器把上述代码当成废代码优化掉的念头
    dummy_counter += local_counter;
}

void RunBenchmark(int num_threads, int rounds, int batch_size) {
    cout << "=========================================" << endl;
    cout << "Threads: " << num_threads 
         << " | Rounds: " << rounds 
         << " | Batch Size: " << batch_size << endl;
    cout << "Total Allocs per Allocator: " << (long long)num_threads * rounds * batch_size << endl;

    // ----- 测试原生 malloc -----
    auto start_malloc = high_resolution_clock::now();
    vector<thread> threads_malloc;
    for (int i = 0; i < num_threads; ++i) {
        threads_malloc.emplace_back(BenchmarkWorker, rounds, batch_size, false);
    }
    for (auto& t : threads_malloc) t.join();
    auto end_malloc = high_resolution_clock::now();
    auto time_malloc = duration_cast<milliseconds>(end_malloc - start_malloc).count();
    
    cout << "[glibc malloc] Time: " << time_malloc << " ms" << endl;

    // ----- 测试你的 mytcmalloc -----
    auto start_my = high_resolution_clock::now();
    vector<thread> threads_my;
    for (int i = 0; i < num_threads; ++i) {
        threads_my.emplace_back(BenchmarkWorker, rounds, batch_size, true);
    }
    for (auto& t : threads_my) t.join();
    auto end_my = high_resolution_clock::now();
    auto time_my = duration_cast<milliseconds>(end_my - start_my).count();

    cout << "[mytcmalloc]   Time: " << time_my << " ms" << endl;
    
    // ----- 战果播报 -----
    cout << "-----------------------------------------" << endl;
    if (time_my < time_malloc) {
        double speedup = (double)time_malloc / time_my;
        cout << "🏆 mytcmalloc WINS! (" << speedup << "x faster)" << endl;
    } else {
        double speedup = (double)time_my / time_malloc;
        cout << "🐢 glibc malloc WINS (" << speedup << "x faster). Need more tuning!" << endl;
    }
    cout << "=========================================\n" << endl;
}

int main() {
    cout << "Starting Memory Allocation Benchmark...\n" << endl;
    
    // 预热一下
    RunBenchmark(1, 1000, 100);
    
    // 多线程中度压测
    RunBenchmark(4, 10000, 100);
    
    // 多线程高强度压测
    RunBenchmark(8, 20000, 100);

    return 0;
}
