#include "util.h"
namespace coroutine_x
{
pid_t GetThreadId()
{
    return syscall(SYS_gettid);
}

uint32_t GetFiberId()
{
    return 0;
}
}