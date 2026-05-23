#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static int pipe_fds[2];

void find_prime(int start, int end, int *p, int maxSize);

int main(int argc, char const *argv[])
{
        if (argc < 5)
        {
                fprintf(2, "Usage: prime [process_count] [prime count] [start with] [end with]\n");
                exit(1);
        }

        int process_count = atoi(argv[1]);
        int prime_count = atoi(argv[2]);
        int *primes = (int *)malloc(sizeof(int) * prime_count);
        int start_with = atoi(argv[3]);
        int end_with = atoi(argv[4]);

        if (start_with > end_with)
        {
                printf("start most less than end!");
                exit(1);
        }

        printf("starting to find prime, taks info: \n");
        printf("process_count: %d prime_count: %d start_with: %d end_with: %d \n", process_count, prime_count, start_with, end_with);

        int range = end_with - start_with;
        int spilt = range / process_count;
        int addSize;
        addSize = range - (spilt * process_count);

        // 打开pipe
        pipe(pipe_fds);

        // no concurrent.
        for (int i = 0; i < process_count; i++)
        {

                int pid = fork();

                if (pid == 0)
                {
                        // 关闭读通道
                        close(pipe_fds[0]);
                        if (i + 1 == process_count)
                        {
                                // printf("[%d] start: %d end: %d\n",getpid(), i * spilt, (i + 1) * spilt + addSize);
                                find_prime(i * spilt, (i + 1) * spilt + addSize, primes, prime_count);
                        }
                        else
                        {
                                // printf("[%d] start: %d end: %d\n", getpid(), i * spilt, (i + 1) * spilt);
                                find_prime(i * spilt, (i + 1) * spilt, primes, prime_count);
                        }
                        exit(0);
                }
        }

        // 关闭写通道
        close(pipe_fds[1]);
        // 父进程开始读取 pipe
        int buf = 0;
        int counter = 0;
        while (1)
        {
                int read_count = read(pipe_fds[0], &buf, 4);
                if (read_count <= 0)
                {
                        break;
                }
                primes[counter++] = buf;
        }

        for (int i = 0; i < counter; i++)
        {
                printf("%d ", primes[i]);
        }
        printf("\n");
        close(pipe_fds[0]);
        exit(0);
}

void find_prime(int start, int end, int *p, int maxSize)
{
        int flag = 1;
        int findCount = 0;
        int tmp[maxSize];

        for (int i = start; i <= end; i++)
        {
                if (i == 0 || i == 1)
                {
                        continue;
                }

                for (int j = 2; j * j <= i; j++)
                {
                        // other factors.
                        if (i % j == 0)
                        {
                                flag = 0;
                                break;
                        }
                }
                if (flag)
                {
                        tmp[findCount++] = i;
                }

                flag = 1;
        }

        for (int i = 0; i < findCount; i++)
        {
                write(pipe_fds[1], &tmp[i], 4);
        }
        close(pipe_fds[1]);
}
