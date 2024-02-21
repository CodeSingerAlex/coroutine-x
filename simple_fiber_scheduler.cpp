/**
 * @brief 实现简单的协程调度
*/
#include "fiber.h"
#include<list>
#include<iostream>
class Scheduler {
public: 
    /**
     * @brief 添加协程调度任务
    */
    void shedule(Fiber::ptr task) {
        m_tasks.push_back(task);
    }

    /**
     * @brief 执行调度任务
    */
    void run() {
        Fiber::ptr task;
        auto it = m_tasks.begin();
        while (it != m_tasks.end()) {
            task = *it;
            m_tasks.erase(it++);
            task->resume();
        }
    }

private:
    std::list<Fiber::ptr> m_tasks;
};

void test_fiber(int i) {
    std::cout << "hello world" << i << std::endl;
}
int main() {
    /// 初始化协程主协程
    Fiber::GetThis();

    /// 创建调度器
    Scheduler sc;

    /// 添加调度任务
    for(auto i = 0; i < 10; i++) {
        Fiber::ptr fiber(new Fiber(std::bind(test_fiber, i)));
        sc.shedule(fiber);
    }

    /// 执行调度任务
    sc.run();

    return 0;
}