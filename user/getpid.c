#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/ds.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
        printf("\n%d\n", ugetpid());
        return 0;
}
