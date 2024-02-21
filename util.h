#ifndef __UTIL_H__
#define __UTIL_H__

#include <pthread.h>
#include <string>
#include <thread>
#include <memory>
#include <functional>
#include <sys/syscall.h>
#include <unistd.h>

/// 获取当前线程ID
pid_t GetThreadId();

/// 获取当前协程ID
uint32_t GetFiberId();

#endif