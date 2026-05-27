#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/memlayout.h"

void print_binary64(uint64 n)
{
        for (int i = sizeof(n) * 8 - 1; i >= 0; i--)
        {
                printf("%c", ((n & (1 << i)) ? '1' : '0'));
        }
        printf("\n");
}

void print_binary32(uint32 n)
{
        for (int i = sizeof(n) * 8 - 1; i >= 0; i--)
        {
                printf("%c", ((n & (1 << i)) ? '1' : '0'));
        }
        printf("\n");
}

void print_binary16(uint16 n)
{
        for (int i = sizeof(n) * 8 - 1; i >= 0; i--)
        {
                printf("%c", ((n & (1 << i)) ? '1' : '0'));
        }
        printf("\n");
}

void print_binary8(uint8 n)
{
        for (int i = sizeof(n) * 8 - 1; i >= 0; i--)
        {
                printf("%c", ((n & (1 << i)) ? '1' : '0'));
        }
        printf("\n");
}

int main(int argc, char const *argv[])
{
        uint64 res = 0;
        // 读一下
        ugetpid();

        pgaccess(USYSCALL, 64, (uint64)&res);

        print_binary64(res);
        exit(0);
}
