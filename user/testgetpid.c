#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/ds.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
        for (int i = 0; i < 1024 * 1024; i++)
        {
                ugetpid();
        }
        printf("\n%d\n", ugetpid());
        return 0;
}
