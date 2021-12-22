//
// Created by cg on 2021/10/19.
//

#include<fcntl.h>
#include<stdio.h>
#include <stdlib.h>
#include<unistd.h>

/**
 * 测试Linux文件描述符限制
 */
int main(){

    int size = 1025;
    int i = 0;

    char fileName[10] = {0};

    for(;i <= size;i++){
        sprintf(fileName,"file%d",i);
        int r = creat(fileName,O_RDONLY);
        if(r == -1){
            printf("%d\n",i);
            exit(EXIT_FAILURE);
        }
    }


    printf("创建成功\n");
    sleep(1000000000);
    return 0;
}