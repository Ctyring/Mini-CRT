//
// Created by cty on 2023/8/19.
//
#include "minicrt.h"

int fputc(int c, FILE *stream) {
    if (fwrite(&c, 1, 1, stream) != 1)
        return EOF;
    else
        return c;
}

int fputs(const char *str, FILE *stream) {
    int len = strlen(str);
    if (fwrite(str, 1, len, stream) != len)
        return EOF;
    else
        return len;
}

#ifndef WIN32
#define va_list char*
#define va_start(ap, arg) (ap = (va_list)&arg + sizeof(arg))
#define va_arg(ap, t) (*(t*)((ap += sizeof(t)) - sizeof(t)))
#define va_end(ap) (ap = (va_list)0)
#else
#include <Windows.h>
#endif

/**
 * 输入流
 * format: 格式化字符串
 * arg_list: 可变参数列表
 * */
int vfprintf(FILE *stream, const char* format, va_list arg_list){
    int translating = 0; // 判断是否要转换
    int ret = 0;
    const char* p = 0;
    for(p = format; *p != '\0'; ++p){
        switch (*p) {
            case '%':
                if(!translating){
                    // 标记下一个需要转换
                    translating = 1;
                }else{
                    // 正常输出
                    if(fputc('%', stream) < 0)
                        return EOF;
                    ++ret;
                    translating = 0;
                }
                break;
            case 'd':
                if(translating){
                    char buf[16];
                    translating = 0;
                    // 把参数中的整数转换成字符串，放入buf
                    // arg_list指向下一个参数
                    itoa(va_arg(arg_list, int), buf, 10);
                    if(fputs(buf, stream) < 0)
                        return EOF;
                    ret += strlen(buf);
                }else if(fputc('d', stream) < 0){
                    return EOF;
                }else{
                    ++ret;
                }
                break;
            case 's':
                if(translating){
                    // 把参数中的字符串输出
                    // arg_list指向下一个参数
                    const char* str = va_arg(arg_list, const char*);
                    translating = 0;
                    if(fputs(str, stream) < 0)
                        return EOF;
                    ret += strlen(str);
                }else if(fputc('s', stream) < 0){
                    return EOF;
                }else{
                    ++ret;
                }
                break;
            default:
                if(translating){
                    translating = 0;
                }
                if(fputc(*p, stream) < 0)
                    return EOF;
                else
                    ++ret;
                break;
        }
    }
    return ret;
}

int printf(const char* format, ...){
    va_list(arg_list);
    va_start(arg_list, format);
    return vfprintf(stdout, format, arg_list);
}

int fprintf(FILE* stream, const char* format, ...){
    va_list(arg_list);
    va_start(arg_list, format);
    return vfprintf(stream, format, arg_list);
}