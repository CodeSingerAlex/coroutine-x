#include <iostream>
#include<vector>

#include "thread.h"
#include "util.h"
#include "mutex.h"

int count = 0;

coroutine_x::RWMutex s_mutex;

void fun1()
{
    std::cout << "name: " << coroutine_x::Thread::GetName()
              << " this.name: " << coroutine_x::Thread::GetThis()->getName()
              << " id: " << coroutine_x::GetThreadId()
              << " this.id: " << coroutine_x::Thread::GetThis()->getId()
              << std::endl;
    for(int i = 0; i < 100000; i++) 
    {
        coroutine_x::RWMutex::WriteLock lock(s_mutex);
        ++count; 
    }
}

void fun2()
{

} 

int main(int argc, char** argv)
{
    std::cout << "Thread test begin" << std::endl;
    std::vector<coroutine_x::Thread::ptr> thrs;
    for(int i = 0; i < 5; i++)
    {
        coroutine_x::Thread::ptr thr(new coroutine_x::Thread(&fun1, "name_" + std::to_string(i)));
        thrs.push_back(thr);
    }

    for(int i = 0; i < 5; i++)
    {
        thrs[i]->join();
    }
    std::cout << "count: " << count << std::endl;
    std::cout << "thread test end" << std::endl;
    return 0;
}
