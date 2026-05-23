#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
        // print now
        printf("%d\n", getcwd());

        exit(0);
}
