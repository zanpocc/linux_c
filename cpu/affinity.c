//
// Created by cg on 2021/12/30.
//

/**
 * 设置线程的CPU亲和性
 *
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <memory.h>
#include <sched.h>

int main(){

    cpu_set_t cpuset;
    int len = sizeof(cpuset);
    memset(&cpuset,0,len);
    // Linux下cpu_set_t大小为128字节,表示支持1024个CPU
    printf("cpu_set_t size：%lu\n", sizeof(cpuset));

    CPU_ZERO(&cpuset);
    // 为1表示亲和,可以在上面执行,将该线程设置为只能在CPU 1上执行,CPU索引从0开始
    CPU_SET(1,&cpuset);

    // 0表示当前调用线程的pid
    sched_setaffinity(0, sizeof(cpuset),&cpuset);

    scanf("%s");
    return 0;
}
