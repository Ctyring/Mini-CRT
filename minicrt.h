//
// Created by bug on 2023/8/19.
//

#ifndef MINI_CRT_MINICRT_H
#define MINI_CRT_MINICRT_H
typedef int FILE;
#define EOF (-1)

#ifdef WIN32
#define stdin ((FILE*)(GetStdHandle(STD_INPUT_HANDLE)))
#define stdout ((FILE*)(GetStdHandle(STD_OUTPUT_HANDLE)))
#define stderr ((FILE*)(GetStdHandle(STD_ERROR_HANDLE)))
#else
#define stdin ((FILE*)0)
#define stdout ((FILE*)1)
#define stderr ((FILE*)2)
#endif //MINI_CRT_MINICRT_H
