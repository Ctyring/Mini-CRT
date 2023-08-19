//
// Created by cty on 2023/8/19.
//
/**
 * 转换n为redix进制表示的字符串
 * */
char* itoa(int n, char* str, int radix){
    char digit[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char* p = str;
    char* head = str;
    // 非法
    if(!p || radix < 2 || radix > 36)
        return p;
    if(radix != 10 && n < 0)
        return p;

    // 特殊处理0
    if(n == 0){
        *p++ = '0';
        *p = 0;
        return p;
    }

    // 处理负数
    if(radix == 10 && n < 0){
        *p++ = '-';
        n = -n;
    }

    // 计算
    while(n){
        *p++ = digit[n % radix];
        n /= radix;
    }
    *p = 0;

    // 翻转
    for(--p; head < p; ++head, --p){
        char temp = *head;
        *head = *p;
        *p = temp;
    }

    return str;
}

// 字符串比较
int strcmp(const char* src, const char* dst){
    int ret = 0;
    unsigned char* p1 = (unsigned char*)src;
    unsigned char* p2 = (unsigned char*)dst;
    while(!(ret = *p1 - *p2) && *p2)
        ++p1, ++p2;
    if(ret < 0)
        ret = -1;
    else if(ret > 0)
        ret = 1;
    return ret;
}

// 字符串复制
char* strcpy(char* dest, const char* src){
    char* ret = dest;
    while(*src)
        *dest++ = *src++;
    *dest = '\0';
    return ret;
}

// 字符串长度
unsigned strlen(const char* str){
    int cnt = 0;
    if(!str)
        return 0;
    for(; *str != '\0'; ++str)
        ++cnt;
    return cnt;
}