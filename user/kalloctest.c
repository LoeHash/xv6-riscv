#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static void
test1()
{
        int i;
        int pid;

        // 创建三个子进程
        for (i = 0; i < 3; i++)
        {
                pid = fork();
                if (pid < 0)
                {
                        printf("fork failed\n");
                        exit(1);
                }
                if (pid == 0)
                {
                        // 子进程：通过系统调用让内核打印锁统计信息
                        int tot = getlockstat();
                        exit(tot);
                }
        }

        int total = 0;
        // 父进程等待所有子进程完成，并累加它们返回的统计值
        for (i = 0; i < 3; i++)
        {
                int stat;
                wait(&stat);
                total += stat;
        }

        // 输出竞争总次数（kmem 和 bcache 锁的自旋次数之和）
        printf("tot= %d\n", total);
        if (total < 5000)
        {
                printf("test1 OK\n");
        }
        else
        {
                printf("test1 FAIL\n");
        }
}

static void
test2()
{
        // 测试内存分配器的功能正确性
        int total = getfreemem();
        printf("total free number of pages: %d (out of 32768)\n", total);
        if (total > 30000)
        {
                printf("test2 OK\n");
        }
        else
        {
                printf("test2 FAIL\n");
        }
}

static void
test3()
{
        // 测试内存分配器的并发安全性
        int i, pid;

        for (i = 0; i < 10; i++)
        {
                pid = fork();
                if (pid < 0)
                {
                        printf("fork failed\n");
                        exit(1);
                }
                if (pid == 0)
                {
                        // 子进程进行大量内存分配和释放操作
                        for (int j = 0; j < 10000; j++)
                        {
                                void *p = sbrk(4096);
                                if (p == (void *)-1)
                                {
                                        printf("sbrk failed\n");
                                        exit(1);
                                }
                                // 使用内存页
                                *(char *)p = 1;
                                if (sbrk(-4096) == (void *)-1)
                                {
                                        printf("sbrk failed\n");
                                        exit(1);
                                }
                        }
                        exit(0);
                }
        }

        // 等待所有子进程完成
        for (i = 0; i < 10; i++)
        {
                wait(0);
        }
        printf("test3 OK\n");
}

int main(int argc, char *argv[])
{
        test1(); // 执行锁竞争测试
        test2(); // 测试内存分配功能
        test3(); // 测试并发安全性
        exit(0);
}