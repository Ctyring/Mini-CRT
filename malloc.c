//
// Created by bug on 2023/8/19.
//
#include "minicrt.h"

typedef struct _heap_header {
    enum {
        // 魔数 空闲块
        HEAP_BLOCK_FREE = 0xABABABAB,
        // 魔数 已分配块
        HEAP_BLOCK_USED = 0xCDCDCDCD,
    } type;
    // 堆块大小
    unsigned size;
    struct _heap_header *next;
    struct _heap_header *prev;
} heap_header;

// 地址a+o
#define ADDR_ADD(a, o) (((char*)(a)) + o)
// header的大小
#define HEADER_SIZE (sizeof(heap_header))
// 堆头的地址
static heap_header *list_head = NULL;

// 释放内存
void free(void *ptr) {
    // 当前指针向前移动找到header的位置
    heap_header *header = (heap_header *) ADDR_ADD(ptr, -HEADER_SIZE);
    if (header->type != HEAP_BLOCK_USED)
        return;

    header->type = HEAP_BLOCK_FREE;
    if(header->prev != NULL && header->prev->type == HEAP_BLOCK_FREE){
        // 合并前面的空闲块
        header->prev->next = header->next;
        if(header->next != NULL)
            header->next->prev = header->prev;
        header->prev->size += header->size;
        header = header->prev;
    }

    if(header->next != NULL && header->next->type == HEAP_BLOCK_FREE){
        // 合并后面的空闲块
        header->size += header->next->size;
        header->next = header->next->next;
    }
}

// 申请内存
void* malloc(unsigned size){
    heap_header *header;

    if(size == 0){
        return NULL;
    }

    header = list_head;
    while(header != 0){
        // 找到合适的空闲块
        if(header->type == HEAP_BLOCK_USED){
            header = header->next;
            continue;
        }
        if(header->size > size + HEADER_SIZE && header->size <= size + HEADER_SIZE * 2){
            // 只够分配时
            header->type = HEAP_BLOCK_USED;
            return ADDR_ADD(header, HEADER_SIZE);
        }
        if(header->size > size + HEADER_SIZE * 2){
            // 足够切割时
            heap_header *next = (heap_header *)ADDR_ADD(header, size + HEADER_SIZE);
            next->prev = header;
            next->next = header->next;
            next->type = HEAP_BLOCK_FREE;
            next->size = header->size - (size + HEADER_SIZE);
            header->next = next;
            header->size = size + HEADER_SIZE;
            header->type = HEAP_BLOCK_USED;
            return ADDR_ADD(header, HEADER_SIZE);
        }
        header = header->next;
    }

    return NULL;
}

#ifndef WIN32
// 调用linux下的brk
static int brk(void* end_data_segment){
    int ret = 0;
    asm("movl $45, %%eax \n\t" // 45号系统调用：brk
        "movl %1, %%ebx \n\t" // end_data_segment
        "int $0x80 \n\t" // 80号系统调用：参数为eax
        "movl %%eax, %0 \n\t" // 返回值放入ret
        :"=r"(ret):"m"(end_data_segment));
    return ret;
}
#endif
#ifdef WIN32
#include <Windows.h>
#endif

// 初始化堆
int mini_crt_heap_init(){
    void *base = NULL;
    heap_header *header = NULL;
    // 32MB堆空间
    unsigned heap_size = 1024 * 1024 * 32;

#ifdef WIN32
    base = VirtualAlloc(0, heap_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if(base == NULL)
        return 0;
#else
    base = (void*)brk(0);
    void* end = ADDR_ADD(base, heap_size);
    end = (void*)brk(end);
    if(!end)
        return 0;
#endif

    header = (heap_header*)base;
    header->size = heap_size;
    header->type = HEAP_BLOCK_FREE;
    header->next = NULL;
    header->prev = NULL;

    list_head = header;
    return 1;
}