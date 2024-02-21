#ifndef __COROUTINE_X_SCHEDULER_H
#define __COROUTINE_X_SCHEDULER_H

#include "fiber.h"
#include "mutex.h"

namespace coroutine_x
{
class Scheduler {
public:
    typedef std::shared_ptr<Scheduler> prt;
    typedef Mutex MutexType;
private:
    
};
} // namespace coroutine_x

#endif