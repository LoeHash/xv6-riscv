#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void my_handler(int sig)
{
        // seems not safe..
        printf("\nouch: %d\n", sig);
}

int main()
{
        int pid;
        if ((pid = fork()) == 0)
        {
                /* code */
                signal(0, (uint64)&my_handler);
                while (1)
                {
                        sleep(1);
                }
        }

        while (1)
        {
                sleep(3);
                sendsig(pid, 0);
        }
}