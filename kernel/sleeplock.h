// kernel/sleeplock.h
#ifndef _SLEEPLOCK_H_
#define _SLEEPLOCK_H_

#include "spinlock.h" // ← 必须，因为 sleeplock 内嵌了 spinlock

struct sleeplock
{
        struct spinlock lk;
        char *name;
        int locked;
        int pid;
};

#endif