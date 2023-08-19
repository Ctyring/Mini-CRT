linux编译命令
```shell
gcc -c -fno-builtin -nostdlib -fno-stack-protector entry.c malloc.c stdio.c string.c printf.c
ar -rs minicrt.a entry.o malloc.o stdio.o string.o printf.o
```
-fno-builtin: 不使用内建函数
-nostdlib: 不使用标准库和启动文件(包含-nostartfiles)
-fno-stack-protector: 不使用堆栈保护

windows编译命令
```shell
cl /c /DWIN32 /GS- entry.c malloc.c stdio.c string.c printf.c
lib entry.obj malloc.obj stdio.obj string.obj printf.obj /OUT:minicrt.lib
```

/DWIN32: 定义WIN32宏
/GS-: 禁用堆栈保护