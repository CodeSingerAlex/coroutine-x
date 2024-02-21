/**
 * @brief 协程类定义
 * @attention 不同于sylar项目中的协程库，本项目没有日志系统和配置文件
*/
#include "fiber.h"
#include<ucontext.h>
#include<atomic>
#include "macro.h"
#include "scheduler.h"

#define FIBER_STACK_SIZE_DEFAULT (128 * 1024)

static std::atomic<uint64_t> s_fiber_id(0);
static std::atomic<uint64_t> s_fiber_count(0);

static thread_local Fiber* t_fiber = nullptr;
static thread_local Fiber::ptr t_thread_fiber = nullptr;
class MallocStackAllocator {
public:
    static void* Alloc(size_t size) {
        return malloc(size);
    }

    static void Dealloc(void* vp) {
        return free(vp);
    }
};

using StackAllocator = MallocStackAllocator;

Fiber::Fiber() {
    SetThis(this);
    m_state = RUNNING;

    if(getcontext(&m_ctx)) {
        SYLAR_ASSERT2(false, "getcontext");
    }

    ++s_fiber_count;
    m_id = s_fiber_id++;
}

Fiber::Fiber(std::function<void()> cb, size_t stacksize, bool run_in_sheduler)
    : m_id(s_fiber_id++)
    , m_cb(cb) 
    , m_runInScheduler(run_in_sheduler) {
    ++s_fiber_count;
    // 修改配置文件中的g_fiber_stack_size->getValue()为宏定义
    m_stacksize = stacksize ? stacksize : FIBER_STACK_SIZE_DEFAULT;
    m_stack = StackAllocator::Alloc(m_stacksize);

    if(getcontext(&m_ctx)) {
        SYLAR_ASSERT2(false, "getcontext");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;

    makecontext(&m_ctx, &Fiber::MainFunc, 0);
}

Fiber::~Fiber() {
}

void Fiber::SetThis(Fiber* f) {
    t_fiber = f;
}

Fiber::ptr Fiber::GetThis() {
    if(t_fiber) {
        return t_fiber->shared_from_this();
    }

    Fiber::ptr main_fiber(new Fiber);
    SYLAR_ASSERT(t_fiber == main_fiber.get());
    t_thread_fiber = main_fiber;
    return t_fiber->shared_from_this();
}

void Fiber::resume() {
    SYLAR_ASSERT(m_state != TERM && m_state != RUNNING);
    SetThis(this);
    m_state = RUNNING;
    
    // 参与调度器调度的协程与调度协程切换
    if(m_runInScheduler) {
        if(swapcontext(&(Scheduler::GetMainFiber()->m_ctx), &m_ctx)) {
            SYLAR_ASSERT2(false, "swapcontext");
        }
    } else { // 不参与调度器调度的协程和主线程协程切换
        if(swapcontext(&(t_thread_fiber->m_ctx), &m_ctx)) {
            SYLAR_ASSERT2(false, "swapcontext");
        }
    }
}

void Fiber::yeild() {
    SYLAR_ASSERT(m_state == RUNNING || m_state == TERM);
    SetThis(t_thread_fiber.get());
    if(m_state != TERM) {
        m_state = READY;
    }

    // 参与调度器调度的协程与调度协程切换
    if(m_runInScheduler) {
        if(swapcontext(&m_ctx, &(Scheduler::GetMainFiber()->m_ctx))) {
            SYLAR_ASSERT2(false, "swapcontext");
        }
    } else {
        if(swapcontext(&m_ctx, &(t_thread_fiber->m_ctx))) {
            SYLAR_ASSERT2(false, "swapcontext");
        }
    }
}

void Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    SYLAR_ASSERT(cur);

    cur->m_cb();
    cur->m_cb = nullptr;
    cur->m_state = TERM;

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->yeild();
}

void Fiber::reset(std::function<void()> cb) {
    SYLAR_ASSERT(m_stack);
    SYLAR_ASSERT(m_state == TERM);
    m_cb = cb;
    if(getcontext(&m_ctx)) {
        SYLAR_ASSERT2(false, "getcontext");
    }
    
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = READY;
}
