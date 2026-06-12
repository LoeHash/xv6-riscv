#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"
#include "ds.h"
#include "ipc.h"

uint64
sys_exit(void)
{
        int n;
        argint(0, &n);
        kexit(n);
        return 0; // not reached
}

uint64
sys_getpid(void)
{
        return myproc()->pid;
}

uint64
sys_fork(void)
{
        return kfork();
}

uint64
sys_wait(void)
{
        uint64 p;
        argaddr(0, &p);
        return kwait(p);
}

uint64
sys_sbrk(void)
{
        uint64 addr;
        int t;
        int n;

        argint(0, &n);
        argint(1, &t);
        struct proc *p = myproc();

        addr = p->sz;

        // t == SBRK_EAGER || n < 0
        if (0)
        {
                if (growproc(n) < 0)
                {
                        return -1;
                }
        }
        else
        {

                // lazy
                if (p->sz + n > TRAPFRAME)
                {
                        return -1;
                }

                p->sz += n;
        }

        return addr;
}

uint64
sys_pause(void)
{
        int n;
        uint ticks0;

        argint(0, &n);
        if (n < 0)
                n = 0;
        acquire(&tickslock);
        ticks0 = ticks;
        while (ticks - ticks0 < n)
        {
                if (killed(myproc()))
                {
                        release(&tickslock);
                        return -1;
                }
                sleep(&ticks, &tickslock);
        }
        release(&tickslock);
        return 0;
}

uint64
sys_kill(void)
{
        int pid;

        argint(0, &pid);
        return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
        uint xticks;

        acquire(&tickslock);
        xticks = ticks;
        release(&tickslock);
        return xticks;
}

uint64 sys_sleep(void)
{
        uint64 now_tick = sys_uptime();
        if (now_tick < 0)
        {
                return 1;
        }

        uint64 time_to_sleep = 0;
        // 1: register
        // 2: ptr
        //  copy the data from user space
        //  to the kernel space.
        argaddr(0, &time_to_sleep);

        while (sys_uptime() <= (now_tick + time_to_sleep))
        {
                yield();
        }

        return 0;
}

uint64 sys_getcwd(void)
{
        return kgetcwd();
}

uint64 sys_trace(void)
{
        int sys_call_code;
        argint(0, &sys_call_code);

        myproc()->traced_system_call = sys_call_code;

        return 0;
}

uint64 sys_sysinfo()
{
        uint64 info_addr = 0;

        argaddr(0, &info_addr);

        struct sysinfo_struct info;

        info.free_memory = kget_free_mem_size();
        info.processes = cpupcount();

        copyout(myproc()->pagetable, info_addr, (char *)&info, sizeof(info));

        return 0;
}

// 检测被访问的page
uint64 sys_pgaccess()
{
        // start: start va
        // pg_count: the number to check
        // res : return back
        uint64 start, pg_count, res, tmp;

        argaddr(0, &start);
        argaddr(1, &pg_count);
        argaddr(2, &res);

        if (pgaccess(myproc()->pagetable, start, pg_count, &tmp) != 0)
        {
                return -1;
        }

        copyout(myproc()->pagetable, res, (char *)&(tmp), 8);

        return 0;
}

uint64 sys_signal(void)
{

        // arg:
        uint64 signum, sig_handler;
        argaddr(0, &signum);
        argaddr(1, &sig_handler);
        // printf("%ld", sig_handler);
        // ((void (*)(int))(myproc()->sig_handlers[0]))(1121);

        return signal(myproc(), signum, (void (*)(int))sig_handler);
}

uint64 sys_sendsig(void)
{
        uint64 signum, pid;
        argaddr(0, &pid);
        argaddr(1, &signum);

        sendsig(pid, signum);

        // printf("%ld\n", myproc()->sig_pending[0]);

        // return 0;
        return sendsig(pid, signum);
}

uint64 sys_sigreturn(void)
{
        uint64 signum;
        argaddr(0, &signum);

        // printf("绕回来了! 恢复寄存器! %ld\n", signum);
        struct proc *p = myproc();
        struct trapframe *tf = p->trapframe;
        uint64 trap_sp = tf->sp;
        struct trapframe save_tf;
        if (copyin(p->pagetable, (char *)&save_tf, trap_sp, sizeof(save_tf)))
        {
                return -1;
        }

        *tf = save_tf;

        return 0;
}

///////////////////////////////////////////
uint64 sys_sem_init(void)
{
        struct semaphore sem;
        uint64 sem_addr, name_addr;
        int step, perms;
        char name[255];

        argaddr(0, &sem_addr);
        if (copyin(myproc()->pagetable, (char *)&sem, sem_addr, sizeof(struct semaphore)) == -1)
        {
                return -1;
        }

        argaddr(1, &name_addr);
        if (copyin(myproc()->pagetable, name, name_addr, 254) == -1)
        {
                return -1;
        }
        name[254] = '\0';
        sem.name = name;

        argint(2, &step);
        argint(3, &perms);

        ksem_init(&sem, name, step, perms);

        if (copyout(myproc()->pagetable, sem_addr, (char *)&sem, sizeof(struct semaphore)) == -1)
        {
                panic("damn!");
        }

        return 0;
}

uint64 sys_sem_p(void)
{
        struct semaphore sem;
        uint64 sem_addr;

        argaddr(0, &sem_addr);
        if (copyin(myproc()->pagetable, (char *)&sem, sem_addr, sizeof(struct semaphore)) == -1)
        {
                return -1;
        }

        return (uint64)ksem_p(&sem, sem_addr);
}

uint64 sys_sem_v(void)
{
        struct semaphore sem;
        uint64 sem_addr;

        argaddr(0, &sem_addr);
        if (copyin(myproc()->pagetable, (char *)&sem, sem_addr, sizeof(struct semaphore)) == -1)
        {
                return -1;
        }

        return (uint64)ksem_v(&sem, sem_addr);
}