#include <cppcoro/task.hpp>
#include <cppcoro/sync_wait.hpp>
#include <cppcoro/when_all.hpp>
#include <cppcoro/generator.hpp>
#include <cppcoro/async_generator.hpp>
#include <cppcoro/static_thread_pool.hpp>
#include <cppcoro/schedule_on.hpp>

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <random>

// 示例1: 基础协程任务
cppcoro::task<std::string> fetch_user_data(int user_id) {
    std::cout << "开始获取用户 " << user_id << " 的数据..." << std::endl;
    
    // 模拟异步操作（如网络请求）
    co_await std::suspend_always{};
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    co_return "用户 " + std::to_string(user_id) + " 的数据";
}

// 示例2: 带异常处理的协程
cppcoro::task<double> calculate_average(const std::vector<int>& numbers) {
    if (numbers.empty()) {
        throw std::invalid_argument("数字列表不能为空");
    }
    
    int sum = 0;
    for (int num : numbers) {
        sum += num;
        // 模拟计算过程中的暂停
        co_await std::suspend_always{};
    }
    
    co_return static_cast<double>(sum) / numbers.size();
}

// 示例3: 生成器 - 斐波那契数列
cppcoro::generator<int> fibonacci(int limit) {
    int a = 0, b = 1;
    
    while (a <= limit) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}

// 示例4: 异步生成器 - 模拟数据流
cppcoro::async_generator<std::string> data_stream(int count) {
    for (int i = 0; i < count; ++i) {
        // 模拟异步数据产生
        co_await std::suspend_always{};
        co_yield "数据块 " + std::to_string(i + 1);
    }
}

// 示例5: 使用线程池的并行计算
cppcoro::task<int> parallel_computation(cppcoro::static_thread_pool& pool, int value) {
    // 将任务调度到线程池
    co_await pool.schedule();
    
    std::cout << "在线程 " << std::this_thread::get_id() 
              << " 上计算 " << value << std::endl;
    
    // 模拟计算密集型任务
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    int result = value * value;
    
    co_return result;
}

// 示例6: 并行执行多个任务
cppcoro::task<> run_parallel_tasks() {
    cppcoro::static_thread_pool pool{4};  // 4个线程的线程池
    
    std::cout << "\n=== 并行任务示例 ===" << std::endl;
    
    // 创建多个并行任务
    std::vector<cppcoro::task<int>> tasks;
    for (int i = 1; i <= 8; ++i) {
        tasks.push_back(parallel_computation(pool, i));
    }
    
    // 等待所有任务完成
    auto results = co_await cppcoro::when_all(std::move(tasks));
    
    std::cout << "并行任务结果: ";
    for (const auto& result : results) {
        std::cout << result << " ";
    }
    std::cout << std::endl;
}

// 示例7: 顺序执行任务
cppcoro::task<> run_sequential_tasks() {
    std::cout << "\n=== 顺序任务示例 ===" << std::endl;
    
    try {
        // 顺序执行任务
        auto user1 = co_await fetch_user_data(1);
        auto user2 = co_await fetch_user_data(2);
        
        std::cout << "用户1: " << user1 << std::endl;
        std::cout << "用户2: " << user2 << std::endl;
        
        // 计算平均值
        std::vector<int> numbers{1, 2, 3, 4, 5};
        auto average = co_await calculate_average(numbers);
        std::cout << "平均值: " << average << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "错误: " << e.what() << std::endl;
    }
}

// 示例8: 生成器使用
cppcoro::task<> use_generators() {
    std::cout << "\n=== 生成器示例 ===" << std::endl;
    
    // 使用同步生成器
    std::cout << "斐波那契数列 (<=100): ";
    for (auto num : fibonacci(100)) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
    
    // 使用异步生成器
    std::cout << "数据流: ";
    auto stream = data_stream(5);
    while (auto data = co_await stream) {
        std::cout << *data << " ";
    }
    std::cout << std::endl;
}

// 示例9: 复杂的任务组合
cppcoro::task<std::string> process_user_data(int user_id) {
    std::cout << "\n=== 复杂任务组合示例 ===" << std::endl;
    
    // 并行获取用户基本信息和朋友列表
    auto [basic_info, friends] = co_await cppcoro::when_all(
        fetch_user_data(user_id),
        fetch_user_data(user_id * 100)  // 模拟朋友数据
    );
    
    // 模拟数据处理
    co_await std::suspend_always{};
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    co_return "处理完成: " + basic_info + " + " + friends;
}

// 主协程
cppcoro::task<> run_all_examples() {
    std::cout << "开始 CppCoro 示例演示..." << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // 运行各种示例
    co_await run_sequential_tasks();
    co_await run_parallel_tasks();
    co_await use_generators();
    
    auto result = co_await process_user_data(42);
    std::cout << result << std::endl;
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        end_time - start_time);
    
    std::cout << "\n所有示例完成! 总耗时: " << duration.count() << "ms" << std::endl;
}

// 主函数
int main() {
    std::cout << "C++ 协程示例程序" << std::endl;
    std::cout << "==================" << std::endl;
    
    try {
        // 使用 sync_wait 来运行顶层的协程
        cppcoro::sync_wait(run_all_examples());
        
        std::cout << "\n程序正常结束!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "程序异常: " << e.what() << std::endl;
        return 1;
    }
}