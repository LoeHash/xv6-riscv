#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

void cat(int fd)
{
        int n;

        while ((n = read(fd, buf, sizeof(buf))) > 0)
        {
                if (write(1, buf, n) != n)
                {
                        fprintf(2, "cat: write error\n");
                        exit(1);
                }
        }
        if (n < 0)
        {
                fprintf(2, "cat: read error\n");
                exit(1);
        }
}

int main(int argc, char *argv[])
{
        char *from_path, filename;

        if (argc <= 1)
        {
                fprintf(2, "find usage: find (where) (name)\n");
                exit(0);
        }
        exit(0);
}
