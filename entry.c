//
// Created by bug on 2023/8/18.
//
#include "minicrt.h"

#ifdef WIN32
#include <Windows.h>
#endif

// main由用户定义
extern int main(int argc, char* argv[]);

// 调用由atexit注册的退出回调函数
// 实现结束进程,调用linux的1号系统调用实现
void exit(int);

static void crt_fatal_error(const char* msg)
{
    //printf("fatal error: %s", msg);
    exit(1);
}

void mini_crt_entry(void)
{
    int ret;

#ifdef WIN32
    int flag = 0;
    char* argv[16]; // 最多支持16个参数
    char* cl = GetCommandLineA(); // 获取命令行

    // 解析命令行
    argv[0] = cl;
    argc++;
    while(*cl){
        if(*cl == '\"')
            if(flag == 0) flag = 1;
            else flag = 0;
        else if(*cl == ' ' && flag == 0){
            if(*(cl+1)){
                argv[argc] = cl + 1;
                argc++;
            }
            *cl = '\0';
        }
        cl++;
    }

#else
    int argc;
    char** argv;
    char* ebp_reg = 0;
    // ebp_reg是main函数的栈底指针
    asm("movl %%ebp, %0 \n":"=r"(ebp_reg)); // 从汇编里面直接拿ebp寄存器
    argc = *(int*)(ebp_reg + 4);
    argv = (char**)(ebp_reg + 8);
#endif
    // 初始化堆
    if(!mini_crt_heap_init())
        crt_fatal_error("heap initialize failed");

    // 初始化IO
    if(!mini_crt_io_init())
        crt_fatal_error("IO initialize failed");

    // 调用main函数
    ret = main(argc, argv);

    // 退出进程
    exit(ret);
}

// 退出进程
void exit(int exitCode)
{
#ifdef WIN32
    ExitProcess(exitCode);
#else
    asm("movl %0, %%ebx \n\t" // 将exitCode放入ebx寄存器
        "movl $1, %%eax \n\t" // 将1放入eax寄存器 表示调用1号系统调用
        "int $0x80 \n\t" // 触发软中断
        "hlt \n\t"::"m"(exitCode)); // ::表示输入操作数 m表示输入操作数的类型为内存操作数 ::"m"(exitCode) 表示将变量 `exitCode` 的值作为内存操作数传递给占位符 `%0`
    )
#endif
}