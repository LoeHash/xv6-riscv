
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#ifndef MY_MALLOC
#define MASK_P 0x0000000000000001

#define META_LENGTH 16
// (16 + N) bytes in total
typedef struct
{
        uint64 prev_len;
        // 0000...00 P(only 1 lower bit for checking the prev chunk whether used or not)
        uint64 size;
        char data[]; // flexible arr
} mem_chunk;

typedef struct
{
        char *start_ptr;
        char *end_ptr;
        uint64 size;
} mem_box;
void infalte_box(mem_box *box, uint64 inflate_size);
void mfree(void *mem_chunk_data);
void *find_space(uint64 size);
uint64 resize_length(uint64 val);
void shrink_box(mem_box *box, uint64 shrink_size);
mem_chunk *create_mem_chunk(char *start_ptr, uint64 length, uint64 prev_length);
void create_mem_box(char *start, uint64 size);
void *my_malloc(uint64 size);
void init_my_malloc();

// 默认情况下, 我们需要一个初始化的堆长度 (64 bit)
// 这里先来默认为 4096 bytes
static uint64 HEAP_LENGTH = 2 << 11;
static uint64 HEAP_USED = 0;
static char *HEAP_START_PTR;
static char *HEAP_END_PTR;
// static mem_chunk *start_chunk;
static mem_box main_box;
#endif

void init_my_malloc()
{
        // 记录堆起始地址
        // 位于静态全局变量初始化区域
        HEAP_START_PTR = sbrk(0);

        // 拉高堆pb
        HEAP_END_PTR = sbrk(HEAP_LENGTH) + HEAP_LENGTH - 1;

        // 初始化一个最初的chunk，放在heap开头
        // 同时记录当前第一个节点
        // start_chunk = create_mem_chunk(HEAP_START_PTR, 0, 0);

        // 紧接着第一个chunk，我们创建一个box
        create_mem_box(HEAP_START_PTR, HEAP_LENGTH - HEAP_USED);
}

void create_mem_box(char *start, uint64 size)
{
        // safety checking.
        if (start < HEAP_START_PTR || (start + size - 1) > HEAP_END_PTR)
        {
                exit(1);
        }

        // 创建箱子
        main_box.start_ptr = start;
        main_box.size = size;
        main_box.end_ptr = start + size - 1;
}

// 创建内存chunk
mem_chunk *create_mem_chunk(char *start_ptr, uint64 length, uint64 prev_length)
{
        // TODO:
        if (length < 0)
        {
                exit(1);
        }

        if (start_ptr < HEAP_START_PTR || start_ptr > HEAP_END_PTR)
        {
                exit(1);
        }

        // 必须保证长度为八的整数倍
        uint64 chunk_size = META_LENGTH + resize_length(length);
        HEAP_USED += chunk_size;

        // 1. 检查当前堆长度 ( 是否需要扩容 )
        if (main_box.start_ptr + (chunk_size - 1) > main_box.end_ptr)
        {
                // 如果堆不够用，尝试拉高
                // length: HEAP_LENGTH << 1
                size_t grow = HEAP_LENGTH;
                HEAP_LENGTH = grow;
                HEAP_END_PTR = sbrk(HEAP_LENGTH) + grow;

                main_box.end_ptr = HEAP_END_PTR;
                main_box.size += grow;
                printf("heap is full!");
                printf("setting the pb to: %p", HEAP_END_PTR);
                printf("increased: %lld", (long long)(HEAP_LENGTH));
        }

        // 2. 检查前面的chunk
        // 若是系统数据
        char *tmp_ptr = (char *)start_ptr;

        if (tmp_ptr == HEAP_START_PTR && length == 0)
        {
                // 直接写入即可
                // 前一个chunk 为 0
                *((long *)tmp_ptr) = 0;
                tmp_ptr += 8;
                // size 为固定16
                //  P 永远为1
                *((long *)tmp_ptr) = (1 << 4) + 1;
                tmp_ptr += 8;
        }
        else
        {
                // 不作其他检查，flag位由外部调用者设置
                // 3. 根据传入的起始位置ptr, 写入数据
                // start_ptr 起始位置，指向

                *((uint64 *)tmp_ptr) = prev_length;
                tmp_ptr += 8;
                // size 根据 长度 + 元数据
                // 同时不设置flag
                *((uint64 *)tmp_ptr) = ((length) + META_LENGTH) | MASK_P;
                tmp_ptr -= 8;

                shrink_box(&main_box, chunk_size);
        }

        return (mem_chunk *)tmp_ptr;
}

void shrink_box(mem_box *box, uint64 shrink_size)
{
        // 裁减box
        box->start_ptr += shrink_size;
        box->size -= shrink_size;
}

// 膨胀盒子
void infalte_box(mem_box *box, uint64 inflate_size)
{

        box->start_ptr -= inflate_size;
        box->size += inflate_size;
}

// 将val 的数值重新调整
// 为 8 的整数倍
uint64 resize_length(uint64 val)
{
        if (!val)
        {
                return 0;
        }

        uint64 bits = 3;
        while (1)
        {
                if (val <= (1 << bits))
                {
                        break;
                }
                else
                {
                        bits += 1;
                }
        }
        return 1 << bits;
}

void *find_space(uint64 size)
{
        // 暂且返回box
        if (main_box.size < size + META_LENGTH)
        {

                // 空间不足，尝试拉伸
                HEAP_LENGTH = HEAP_LENGTH << 1;
                HEAP_END_PTR = sbrk(HEAP_LENGTH);
                main_box.end_ptr = HEAP_END_PTR;
                main_box.size += HEAP_LENGTH - HEAP_USED;
        }

        return main_box.start_ptr;
}

// no safty
void *my_malloc(uint64 size)
{
        char *start_ptr = find_space(size);
        // 暂时没用到prev length
        mem_chunk *chunk = create_mem_chunk(start_ptr, size, 0);
        return chunk->data;
}

/* mem_chunk
 *  - no the struct mem_chunk!
 *  - it's poiting to the mem_chunk->data !
 *
 * basic think:
 *    |------------------------------------------------------------------------------|
 *    |  size  |    data    |  size   |           |                                  |
 *    | xxxxxx |            | xxxxxx  |           |        box....                   |
 *    |        |            |  data   |           |        box....                   |
 *    |        |            |         |           |        box....                   |
 *    |        |            |         |           |                                  |
 *    |------------------------------------------------------------------------------|
 *
 */

// 若归还的内存chunk正好和 box 邻近
// 则直接归还到box中
//     boxing alg (盒子算法)
// 一定要確保當前ptr是由malloc 创建了
// 同时, 我们还需要保证: mem_chunk_data 是没有被移动过的
void mfree(void *mem_chunk_data)
{
        // todo :
        //  1: 先找到整个mem_chunk 的起始地址
        char *mem_chunk_start = (char *)mem_chunk_data - META_LENGTH;

        uint64 *mem_chunk_size = (uint64 *)(mem_chunk_start + 8);

        // 设置此内存块为释放
        *mem_chunk_size &= ~MASK_P;

        // 说明和盒子临近
        // 则此时将盒子膨胀
        if ((mem_chunk_start + *mem_chunk_size) == main_box.start_ptr)
        {
                infalte_box(&main_box, *mem_chunk_size);
        }
}

int main()
{
        init_my_malloc();
        char *ptr = my_malloc(1024);

        for (size_t i = 0; i < 1024; i++)
        {
                ptr[i] = 0xff;
        }
}