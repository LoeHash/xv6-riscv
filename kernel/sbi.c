#include "types.h"
void sbi_send_ipi(unsigned long hart_mask)
{
        register uint64 a0 asm("a0") = hart_mask;
        register uint64 a7 asm("a7") = 0x735049; // SBI IPI extension
        register uint64 a6 asm("a6") = 0;
        asm volatile("ecall" : "+r"(a0) : "r"(a6), "r"(a7) : "memory");
}