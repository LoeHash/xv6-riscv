#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static int pipe_fds[2];

static void wait_for_f()
{
        char buf = 'c';
        int read_count = 0;
        while (read_count != 1)
        {
                read_count = read(pipe_fds[0], &buf, 1);
        }

        fprintf(1, "[%d] child get ping!\n", getpid());

        write(pipe_fds[1], &buf, 1);

        close(pipe_fds[0]);
        close(pipe_fds[1]);

        exit(0);
}

int main(int argc, char const *argv[])
{

        if (pipe(pipe_fds) != 0)
        {
                exit(1);
        }

        int pid = fork();
        if (pid == 0)
        {
                wait_for_f();
        }

        char buf = 'c';

        write(pipe_fds[1], &buf, 1);

        // 写完立刻关闭
        close(pipe_fds[1]);
        sleep(10);
        int read_count = 0;

        while (read_count != 1)
        {
                read_count = read(pipe_fds[0], &buf, 1);
        }

        fprintf(1, "[%d] father get ping!\n", getpid());

        close(pipe_fds[0]);

        exit(0);
}
