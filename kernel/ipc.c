#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "ipc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"
#include "proc.h"

#define NSEM_LOCKS 16
#define SEM_NAME_MAX 32

static struct
{
        struct sleeplock lock;   // 真正的锁
        char name[SEM_NAME_MAX]; // 映射用的名字
        int used;
} lock_table[NSEM_LOCKS];

// 简单哈希：线性查找，不够可改用哈希，但16个够用
static struct sleeplock *
get_lock_by_name(char *name)
{
        // 查找已有
        for (int i = 0; i < NSEM_LOCKS; i++)
        {
                if (lock_table[i].used && strncmp(lock_table[i].name, name, SEM_NAME_MAX) == 0)
                {
                        return &lock_table[i].lock;
                }
        }
        // 分配新槽
        for (int i = 0; i < NSEM_LOCKS; i++)
        {
                if (!lock_table[i].used)
                {
                        lock_table[i].used = 1;
                        safestrcpy(lock_table[i].name, name, SEM_NAME_MAX);
                        initsleeplock(&lock_table[i].lock, name);
                        return &lock_table[i].lock;
                }
        }
        panic("lock_table full");
        return 0;
}

void ksem_init(struct semaphore *sem,
               char *name,
               int step,
               int perms)
{
        sem->sleep_lock = *get_lock_by_name(name);
        sem->prev_op_pid = 0;
        sem->name = name;
        sem->step = step;
        sem->perms = perms;

        printf("123, done.\n");
        safestrcpy(sem->name, name, SEM_NAME_MAX);
        printf("123, done.\n");
}

uint32 ksem_p(struct semaphore *sem, uint64 sem_addr)
{
        if (sem->val == 0)
        {
                sleep((void *)sem->chan, &sem->sleep_lock.lk);
        }

        acquiresleep_at(&sem->sleep_lock, (void *)sem->chan);

        while (sem->val == 0 || sem->val < sem->step)
        {
                sleep((void *)sem->chan, &sem->sleep_lock.lk);
        }
        uint32 tmp = sem->val;
        sem->val -= sem->step;

        if (copyout(myproc()->pagetable, sem_addr, (char *)&sem, sizeof(struct semaphore)) == -1)
        {
                panic("damn!");
        }

        releasesleep_at(&sem->sleep_lock, (void *)sem->chan);

        return tmp;
}

uint32 ksem_v(struct semaphore *sem, uint64 sem_addr)
{
        acquiresleep_at(&sem->sleep_lock, (void *)sem->chan);

        sem->val += sem->step;

        if (copyout(myproc()->pagetable, sem_addr, (char *)&sem, sizeof(struct semaphore)) == -1)
        {
                panic("damn!");
        }

        releasesleep_at(&sem->sleep_lock, (void *)sem->chan);

        return sem->val;
}
