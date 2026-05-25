#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/ds.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
        struct sysinfo_struct *info = (struct sysinfo_struct *)malloc(sizeof(struct sysinfo_struct));

        sysinfo(info);

        printf("free mem: %ld, unused processes: %ld\n", (info->free_memory), info->processes);

        return 0;
}
