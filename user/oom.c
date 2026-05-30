#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[])
{

        int *p = (int *)0x00007890080808ff;

        printf("%d", *p);
}
