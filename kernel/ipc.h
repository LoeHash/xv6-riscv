

// kernel/ipc.h
#ifndef _IPC_H_
#define _IPC_H_

#define IS_UNDER_VAL(val) val > 0xffffffff ? 0 : 1

/**
 * 思路:
 *      1. 我们会创建sem结构体,但是不能直接将这个指针返回给用户
 *         我们需要一种机制, 能让用户唯一地去标识一个信号量结构体
 *         这种情况是全局共享的信号量, 我们先不实现, 做一个简易实现
 *
 *
 *
 *
 */
#define MAX_SEMAPHORE_COUNT

#include "types.h"
#include "sleeplock.h" // ← 添加这一行，让 struct sleeplock 完整可见

struct semaphore
{
        volatile uint32 val;
        int step;
        uint16 perms;
        char *name;
        uint64 chan;
        struct sleeplock sleep_lock; // 现在这个定义没问题了
        int prev_op_pid;
};

// ... 其他声明

// ipc
//  1. sem
void ksem_init(struct semaphore *sem,
               char *name,
               int step,
               int perms);
uint32 ksem_p(struct semaphore *sem, uint64);
uint32 ksem_v(struct semaphore *sem, uint64);

#endif