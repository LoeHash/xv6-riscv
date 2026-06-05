// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define N 100000

void forktest(void)
{
        while (1)
        {
                int pid = fork();
                if (pid == 0)
                {
                        char *argv[] = {"ls", 0};
                        exec("ls", argv);
                }
                wait(0);
                sleep(1);
        }
        printf("fork test OK\n");
}

int main(void)
{
        forktest();
        exit(0);
}
