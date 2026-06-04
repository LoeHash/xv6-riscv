#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void handler(int info);

int main(int argc, char *argv[])
{
        int pid;
        if ((pid = fork()) == 0)
        {
                signal(0, (uint64)&handler);
                while (1)
                {
                        sleep(2);
                        printf("[%d] working.....\n", getpid());
                }
        }
        while (1)
        {
                sleep(10);
                sendsig(pid, 1);
        }

        wait(0);
}
void handler(int info)
{
        printf("收到信号了!: %d\n", info);
}
