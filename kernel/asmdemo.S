.section .text
.global sum_to
sum_to:
        li t0, 1
        li t1, 0
        loop:
                add t1, t1, t0
                addi t0, t0, 1
                blt t0, a0,loop
        mv a0, t1