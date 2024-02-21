#ifndef FIBER_H
#define FIBER_H
#include <memory> // Include the necessary header file
#include <functional> // Include the necessary header file
#include <ucontext.h>

/**
 * @brief 协程类定义和函数声明
*/
class Fiber: public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;
        
    /**
     * @brief 协程状态
     * @details 三种状态：就绪态、运行态、结束态
    */
    enum State {
        /// 就绪态
        READY,
        /// 运行态
        RUNNING,
        /// 结束态
        TERM
    };

private:
    /**
     * @brief 无参构造函数
     * @attention 只能用于创建线程主函数的协程
     * 主函数协程只能由GetThis()调用
    */
    Fiber();

public:
    /**
     * @brief 带参构造函数，用于创建用户协程
     * @param[in] cb 协程入口函数
     * @param[in] stacksize 栈大小
     * @param[in] run_in_scheduler 是否参与调度器调度，默认值为true
    */
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool run_in_scheduler = true);

    /**
     * @brief 折构函数
    */
    ~Fiber();

    /**
     * @brief 重制协程状态和入口函数，复用栈空间
     * @param[in] cb
    */
    void reset(std::function<void()> cb);

    /**
     * @brief 使当前协程切换到运行态
     * @details 之前运行的协程切换到就绪态
    */
    void resume();

    /**
     * @brief 使当前协程让出执行权
     * @details 使让渡执行权给当前协程的协程恢复到运行态
    */
    void yeild();

    /**
     * @brief 获取协程ID
    */
    uint64_t getId() const { return m_id; }

    /**
     * @brief 获取协程状态
    */
    State getState() const { return m_state; }

public:
    /**
     * @brief 设置当前运行协程t_fiber
    */
    static void SetThis(Fiber *f);

    /**
     * @brief 返回当前运行的协程
     * @details 当前未创建协城时，初始化主协程
     * 主协程负责调度其他协程
     * 通过主协程调用resume函数来恢复其他协程
    */
    static Fiber::ptr GetThis();

    /**
     * @brief 获取总协程
    */
    static uint64_t TotalFibers();

    /**
     * @brief 协程入口函数
    */
    static void MainFunc();

    /**
     * @brief 获取当前线程ID
    */
    static uint64_t GetFiberId();

private:
    /// 协程id
    uint64_t m_id = 0;
    /// 协程栈大小
    uint64_t m_stacksize = 0;
    /// 协程状态
    State m_state = READY;
    /// 协程上下文
    ucontext_t m_ctx;
    /// 协程栈地址
    void *m_stack = nullptr;
    /// 协程函数入口
    std::function<void()> m_cb;
    bool m_runInScheduler;
};
#endif