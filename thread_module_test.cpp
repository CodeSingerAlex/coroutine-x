#include <iostream>
#include "thread.h"
#include "util.h"
#include<vector>
int count = 0;

RWMutex s_mutex;

void fun1()
{
    std::cout << "name: " << Thread::GetName()
              << " this.name: " << Thread::GetThis()->getName()
              << " id: " << GetThreadId()
              << " this.id: " << Thread::GetThis()->getId()
              << std::endl;
    for(int i = 0; i < 100000; i++) 
    {
        RWMutex::WriteLock lock(s_mutex);
        ++count; 
    }
}

void fun2()
{

} 

int main(int argc, char** argv)
{
    std::cout << "Thread test begin" << std::endl;
    std::vector<Thread::ptr> thrs;
    for(int i = 0; i < 5; i++)
    {
        Thread::ptr thr(new Thread(&fun1, "name_" + std::to_string(i)));
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