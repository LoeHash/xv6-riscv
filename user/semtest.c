#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/ipc.h"
#include "user/user.h"

int main()
{
        int pid;
        char *name = "fucku";
        struct semaphore *sem = malloc(sizeof(struct semaphore));

        sem->chan = 88488878;
        sem->name = name;
        sem->perms = 1; // not using now.
        sem->step = 1;
        sem->val = 5;

        sem_init(sem, name, 1, 1);

        if ((pid = fork()) == 0)
        {
                while (1)
                {
                        printf("存放前, sem.val: %d\n", sem->val);
                        sleep(20);
                        int prev = sem_p(sem);
                        printf("tooked %d, prev: %d\n", 1, prev);
                        sleep(10);
                }
        }

        // // 父进程不停地存放
        // while (1)
        // {
        //         sleep(60);
        //         sem_v(sem);
        // }
}