cmake_minimum_required(VERSION 3.20)
project(my_project C)

set(CMAKE_C_STANDARD 11)

# cl /c /DWIN32 /GS- entry.c malloc.c stdio.c string.c printf.c
# 写出在cmake中的等价写法

add_library(entry STATIC entry.c)
add_library(malloc STATIC malloc.c)
add_library(stdio STATIC stdio.c)
add_library(string STATIC string.c)
add_library(printf STATIC printf.c)

