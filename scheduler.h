/**
 * @brief 协程调度类
 * @details 封装N-M协程调度器
 *          内部有协程池，支持协程在协程池中切换
*/
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include<mutex>
#include<memory>
#include<string>
#include "fiber.h" 
#include "mute.h"
#include "thread.h"
#include<atomic>
#include<vector>
#include<list>

class Scheduler {
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MuteTYpe;

    /**
     * @brief 创建调度器
     * @param[in] threads 线程数
     * @param[in] use_called 是否使用当前线程作为调度线程
     * @param[in] name 调度器名称
    */
    Scheduler(size_t threads = 1, bool use_called = true, const std::string &name = "Scheduler");

    /**
     * @brief 折构函数
    */
    virtual ~Scheduler();

    /**
     * @brief 获取调度器名称
    */
    const std::string& getName() const { return m_name; }

    /**
     * @brief 获取当前线程调度器指针
    */
    static Scheduler* GetThis();

    /**
     * @brief 获取当前线程的主协程
    */
    static Fiber* GetMainFiber();

    /**
     * @brief 添加调度任务
     * @tparam FiberOrCb 调度任务类型，协程对象或者函数指针
     * @param[] fc 协程对象或指针
     * @param[] thread 指定运行该任务的线程号，-1表示任意线程
    */
    template <class FiberOrCb>
    void scheduler(FiberOrCb fc, int thread = -1) {
        bool need_tickle = false;
        {
            MuteTYpe::Lock lock(m_mutex);
            need_tickle = shedulerNoLock(fc, thread);
        }
        if(need_tickle) {
            tickle();
        }
    }

    /**
     * @brief 启动调度器
    */
    void start();

    /**
     * @brief 停止调度器
    */
    void stop();
protected:
    /**
     * @brief 通知协程调度器有新任务的加入
    */
    virtual void tickle();

    /**
     * @brief 协程调度函数
    */
    void run();

    /**
     * @brief 执行空闲协程
    */
    virtual void idle();

    /**
     * @brief 返回是否可以停止
    */
    virtual bool stopping();

    /**
     * @brief 设置当前协程的调度器
    */
    void setThis();

    /**
     * @brief 返回是否有空闲线程
     * @details 当调度协程加入空闲时空闲线程数+1，从空闲线程返回时空闲线程数-1
    */
    bool hashIdelThreads() { return m_idleThreadCount > 0; }
private:
    struct ScheduleTask
    {
        Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;

        ScheduleTask(Fiber::ptr f, int thr) {
            fiber = f;
            thread = thr;
        }

        ScheduleTask(Fiber::ptr *f, int thr) {
            fiber.swap(*f);
            thread = thr;
        }

        ScheduleTask(std::function<void()> f, int thr) {
            cb = f;
            thread = thr;
        }

        ScheduleTask() { thread = -1; }

        void reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

private:
    // 协调调度器名称
    std::string m_name;
    // 互斥锁
    Mutex m_mutex;
    // 线程池
    std::vector<Thread::ptr> m_threads;
    // 任务队列
    std::list<ScheduleTask> m_tasks; 
    // 线程池的线程ID数组
    std::vector<int> m_threadIds;
    // 线程数量
    size_t m_threadCount = 0;
    // 工作线程数量，不包含use_called的主线程
    std::atomic<size_t> m_activeThreadCount= {0};
    // 空闲线程数量
    std::atomic<size_t> m_idleThreadCount = {0};
};
#endif