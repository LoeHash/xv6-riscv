// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

// all zero
int mem_reference_counter[(PHYSTOP - KERNBASE) / PGSIZE] = {0};
int isInit = 1;

struct run
{
        struct run *next;
};

struct
{
        struct spinlock mem_lock;
        struct spinlock counter_lock;
        struct spinlock init_lock;
        struct run *freelist;
} kmem;

void kinit()
{
        initlock(&kmem.mem_lock, "kmem");
        initlock(&kmem.counter_lock, "kcounter");
        initlock(&kmem.init_lock, "kinit");
        printf("size of arry: %ld\n", (uint64)(PHYSTOP - KERNBASE) / PGSIZE);
        printf("test of arry: %ld\n", (uint64)(mem_reference_counter[32768]));

        freerange(end, (void *)PHYSTOP);
}

void freerange(void *pa_start, void *pa_end)
{
        char *p;
        p = (char *)PGROUNDUP((uint64)pa_start);
        for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE)
                kfree(p);
        acquire(&kmem.init_lock);
        isInit = 0;
        release(&kmem.init_lock);
}

// va
void uvm_mem_add_ref(uint64 pa)
{

        uint64 down_pa = PGROUNDDOWN(pa);
        mem_reference_counter[PA2_REFCOUN_IDX(down_pa)]++;
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa)
{
        struct run *r;

        if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
                panic("kfree");

        // 初始化阶段：直接放回 freelist
        if (isInit)
        {
                memset(pa, 1, PGSIZE);
                r = (struct run *)pa;
                acquire(&kmem.mem_lock);
                r->next = kmem.freelist;
                kmem.freelist = r;
                release(&kmem.mem_lock);
                return;
        }

        uint64 pa_idx = PA2_REFCOUN_IDX(pa);

        acquire(&kmem.counter_lock);

        if (mem_reference_counter[pa_idx] > 1)
        {
                mem_reference_counter[pa_idx]--;
                // printf("- idx: %ld, ref: %d\n", pa_idx, mem_reference_counter[pa_idx]);
                release(&kmem.counter_lock);
                return;
        }

        if (mem_reference_counter[pa_idx] == 1)
        {
                mem_reference_counter[pa_idx]--;
                // printf("- idx: %ld, ref: %d\n", pa_idx, mem_reference_counter[pa_idx]);
                release(&kmem.counter_lock);
                memset(pa, 1, PGSIZE);
                struct run *r = (struct run *)pa;
                acquire(&kmem.mem_lock);
                r->next = kmem.freelist;
                kmem.freelist = r;
                release(&kmem.mem_lock);
                return;
        }
        release(&kmem.counter_lock);

        panic("fuck you!");
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
        struct run *r;

        acquire(&kmem.mem_lock);
        r = kmem.freelist;

        if (r)
                kmem.freelist = r->next;

        release(&kmem.mem_lock);

        acquire(&kmem.counter_lock);
        mem_reference_counter[PA2_REFCOUN_IDX(r)]++;
        release(&kmem.counter_lock);

        if (r)
                memset((char *)r, 5, PGSIZE); // fill with junk
        return (void *)r;
}

uint64 kget_free_mem_size(void)
{

        // kmem 是内存页
        // 它永远指向最后一个可用的内存页
        struct run *r = kmem.freelist;

        uint64 page_count = 0;

        while (r)
        {
                page_count++;
                r = r->next;
        }

        return page_count * PGSIZE;
}