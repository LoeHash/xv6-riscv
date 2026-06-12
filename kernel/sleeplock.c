// Sleeping locks

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"

void initsleeplock(struct sleeplock *lk, char *name)
{
        initlock(&lk->lk, "sleep lock");
        lk->name = name;
        lk->locked = 0;
        lk->pid = 0;
}

void acquiresleep_at(struct sleeplock *lk, void *sleep_at)
{
        acquire(&lk->lk);
        while (lk->locked)
        {
                sleep(sleep_at, &lk->lk);
        }
        lk->locked = 1;
        lk->pid = myproc()->pid;
        release(&lk->lk);
}

void acquiresleep(struct sleeplock *lk)
{
        acquiresleep_at(lk, lk);
}

void releasesleep_at(struct sleeplock *lk, void *release_at)
{
        acquire(&lk->lk);
        lk->locked = 0;
        lk->pid = 0;
        wakeup(release_at);
        release(&lk->lk);
}

void releasesleep(struct sleeplock *lk)
{
        releasesleep_at(lk, lk);
}

int holdingsleep(struct sleeplock *lk)
{
        int r;

        acquire(&lk->lk);
        r = lk->locked && (lk->pid == myproc()->pid);
        release(&lk->lk);
        return r;
}
