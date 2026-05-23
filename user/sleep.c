#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char const *argv[])
{
        if (argc != 2)
        {
                fprintf(2, "Usage: sleep [time for sec]\n");
                exit(1);
        }

        long time_sleep = 0;
        time_sleep = atoi(argv[1]);

        // sleep.
        sleep(time_sleep);

        exit(0);
}
