// Create a zombie process that
// must be reparented at exit.

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
        if (argc != 2)
        {
                printf("zombie plus usage: zombie_plus [count]\n");
                exit(1);
        }

        int zombie_count = atoi(argv[1]);
        for (int i = 0; i < zombie_count; i++)
        {
                if (fork() == 0)
                {
                        pause(5);
                        exit(1);
                }
        }

        pause(10);

        exit(0);
}
