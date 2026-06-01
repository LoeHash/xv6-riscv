#ifndef SYSINFO_H
#define SYSINFO_H
#include "types.h"

struct sysinfo_struct
{
        uint64 free_memory;
        uint64 processes;
};

struct usyscall
{
        uint64 pid;
        uint64 ppid;
};
#endif

#ifndef SYSSIG
#define SYSSIG

#define SIG_DFL ((void (*)(int))0) // 默认处理
#define SIG_IGN ((void (*)(int))1) // 忽略

#define SCAUSE2SIG(sca) (sc - 64)
#define SIGALA 0
#define SIGKILL 9

#endif
