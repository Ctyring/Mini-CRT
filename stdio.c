//
// Created by cty on 2023/8/19.
//
#include "minicrt.h"

int mini_crt_heap_init()
{
    return 1;
}

#ifdef WIN32
#include <Windows.h>

FILE* fopen(const char* filename, const char* mode)
{
    HANDLE hFile = 0;
    int access = 0;
    int creation = 0;

    if(strcmp(mode, "w") == 0){
        access |= GENERIC_WRITE;
        creation |= CREATE_ALWAYS;
    }
    if(strcmp(mode, "w+") == 0){
        access |= GENERIC_WRITE | GENERIC_READ;
        creation |= CREATE_ALWAYS;
    }
    if(strcmp(mode, "r") == 0){
        access |= GENERIC_READ;
        creation |= OPEN_EXISTING;
    }
    if(strcmp(mode, "r+") == 0){
        access |= GENERIC_WRITE | GENERIC_READ;
        creation |= OPEN_ALWAYS;
    }

    hFile = CreateFileA(filename, access, 0, 0, creation, 0, 0);
    if(hFile == INVALID_HANDLE_VALUE)
        return 0;
    else
        return (FILE*)hFile;
}

int fread(void* buffer, int size, int count, FILE* stream)
{
    int read = 0;
    if(!ReadFile((HANDLE)stream, buffer, size * count, &read, 0))
        return 0;
    return read;
}

int fwrite(const void* buffer, int size, int count, FILE* stream)
{
    int written = 0;
    if(!WriteFile((HANDLE)stream, buffer, size * count, &written, 0))
        return 0;
    return written;
}

int fclose(FILE* fp)
{
    return CloseHandle((HANDLE)fp);
}

int fseek(FILE* fp, int offset, int set)
{
    return SetFilePointer((HANDLE)fp, offset, 0, set);
}

#else

static int open(const char *pathname, int flags, int mode){
    int fd = 0;
    asm("movl $5, %%eax \n\t" // 5号系统调用：sys_open
        "movl %1, %%ebx \n\t" // pathname
        "movl %2, %%ecx \n\t" // flags
        "movl %3, %%edx \n\t" // mode
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t"
        :"=r"(fd):"m"(pathname),"m"(flags),"m"(mode));
    return fd;
}

static int read(int fd, void* buffer, unsigned size){
    int ret = 0;
    asm("movl $3, %%eax \n\t" // 3号系统调用：sys_read
        "movl %1, %%ebx \n\t" // fd
        "movl %2, %%ecx \n\t" // buffer
        "movl %3, %%edx \n\t" // size
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t"
        "=m"(ret):"m"(fd),"m"(buffer),"m"(size));
    return ret;
}

static int write(int fd, const void* buffer, unsigned size){
    int ret = 0;
    asm("movl $4, %%eax \n\t" // 4号系统调用：sys_write
        "movl %1, %%ebx \n\t" // fd
        "movl %2, %%ecx \n\t" // buffer
        "movl %3, %%edx \n\t" // size
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t"
        "=m"(ret):"m"(fd),"m"(buffer),"m"(size));
    return ret;
}

static int close(int fd){
    int ret = 0;
    asm("movl $6, %%eax \n\t" // 6号系统调用：sys_close
        "movl %1, %%ebx \n\t" // fd
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t"
        "=m"(ret):"m"(fd));
}

static int seek(int fd, int offset, int mode){
    int ret = 0;
    asm("movl $19, %%eax \n\t" // 19号系统调用：sys_lseek
        "movl %1, %%ebx \n\t" // fd
        "movl %2, %%ecx \n\t" // offset
        "movl %3, %%edx \n\t" // mode
        "int $0x80 \n\t"
        "movl %%eax, %0 \n\t"
        "=m"(ret):"m"(fd),"m"(offset),"m"(mode));
    return ret;
}

FILE *fopen(const char* filename, const char* mode){
    int fd = -1;
    int flags = 0;
    int access = 00700; // 文件权限

#define O_RDONLY    00
#define O_WRONLY    01
#define O_RDWR      02
#define O_CREAT     0100
#define O_TRUNC     01000
#define O_APPEND    02000

    if(strcmp(mode, "w") == 0)
        flags |= O_WRONLY | O_CREAT | O_TRUNC;
    if(strcmp(mode, "w+") == 0)
        flags |= O_RDWR | O_CREAT | O_TRUNC;
    if(strcmp(mode, "r") == 0)
        flags |= O_RDONLY;
    if(strcmp(mode, "r+") == 0)
        flags |= O_RDWR | O_CREAT;

    fd = open(filename, flags, access);
    return (FILE*)fd;
}

int fread(void* buffer, int size, int count, FILE* stream){
    return read((int)stream, buffer, size * count);
}

int fwrite(const void* buffer, int size, int count, FILE* stream){
    return write((int)stream, buffer, size * count);
}

int fclose(FILE* fp){
    return close((int)fp);
}

int fseek(FILE* fp, int offset, int set){
    return seek((int)fp, offset, set);
}

#endif