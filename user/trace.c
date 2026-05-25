#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
        if (argc < 2)
        {
                fprintf(2, "usage: trace [sys-call-id] [command] [args]\n");
                exit(1);
        }

        int sys_call_id = atoi(argv[1]);
        int status = 0;
        char *exec_name = (char *)argv[2];
        char **argvs = (char **)&argv[2];

        int pid = fork();

        if (pid == 0)
        {
                trace(sys_call_id);
                exec(exec_name, argvs);
        }
        wait(&status);
        return 0;
}
