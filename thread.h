#ifndef __THREAD_H__
#define __THREAD_H__

#include<memory>
#include<thread>
#include<functional>
#include<string>
#include<pthread.h>
#include<semaphore.h>

template<class T>
struct ScopedLockImpl {
public: 
    ScopedLockImpl(T& mutex)
        :m_mutex(mutex) 
    {
        m_mutex.lock();
        m_locked = true;
    }

    ~ScopedLockImpl()
    {
        unlock();
    }

    void lock()
    {
        if(!m_locked)
        {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked =  false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

template<class T>
struct ReadScopedLockImpl {
public: 
    ReadScopedLockImpl(T& mutex)
        :m_mutex(mutex) 
    {
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadScopedLockImpl()
    {
        unlock();
    }

    void lock()
    {
        if(!m_locked)
        {
            m_mutex.rdlock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked =  false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

template<class T>
struct WriteScopedLockImpl {
public: 
    WriteScopedLockImpl(T& mutex)
        :m_mutex(mutex) 
    {
        m_mutex.wrlock();
        m_locked = true;
    }

    ~WriteScopedLockImpl()
    {
        unlock();
    }

    void lock()
    {
        if(!m_locked)
        {
            m_mutex.wrlock();
            m_locked = true;
        }
    }

    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked =  false;
        }
    }
private:
    T& m_mutex;
    bool m_locked;
};

class RWMutex {
public:
    typedef ReadScopedLockImpl<RWMutex> ReadLock;
    typedef WriteScopedLockImpl<RWMutex> WriteLock;
    RWMutex() 
    {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex()
    {
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock()
    {
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock()
    {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock()
    {
        pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t m_lock;
};

class Semaphore {
public:
    Semaphore(uint32_t count = 0);
    ~Semaphore(); 
    void wait();
    void notify();

private:
    Semaphore(const Semaphore&) = delete;
    Semaphore(const Semaphore&&) = delete;
    Semaphore& operator=(const Semaphore&) = delete;

private:
    sem_t m_semaphore;
};

class Thread {
public:
    /// 指向Thread类型对象的智能指针
    typedef std::shared_ptr<Thread> ptr;

    /**
     * @brief 带参构造：创建新的Thread类型对象
     * @param[in] cb 线程执行的函数
     * @param[in] name 线程名称
    */
    Thread(std::function<void()> cb, const std::string& name);

    /// 析构函数
    ~Thread();
    
    /// 获取线程名称
    const std::string& getName() const { return m_name; }
    
    /// 获取线程ID
    pid_t getId() const { return m_id; }
    
    /// 等待线程结束
    void join();
    
    /// 获取当前线程指针
    static Thread* GetThis();
    
    /// 获取当前线程的名称
    static const std::string& GetName();
    
    /// 设置当前线程的名称
    static void SetName(const std::string& name);

private:
    /// 禁止拷贝构造函数
    Thread(const Thread&) = delete;

    /// 禁止移动构造函数
    Thread(const Thread&&) = delete;

    /// 禁止拷贝赋值运算符
    Thread& operator=(const Thread&) = delete;

    /// 线程执行函数
    static void* run(void* arg);

private:
    /// 线程ID
    pid_t m_id = -1;

    /// 线程
    pthread_t m_thread;

    /// 线程执行函数
    std::function<void()> m_cb;
    
    /// 线程名称
    std::string m_name;

    /// 封装的信号量
    Semaphore m_semaphore;
};
#endif