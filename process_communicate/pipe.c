//
// Created by 陈功 on 2021/10/18.
//

#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

int main() {
    int file_des[2] = {0};
    int r = pipe(file_des);
    if(r == -1){
        perror("pipe error\n");
        exit(EXIT_FAILURE);
    }

    int pid = fork(); // 子进程继承父进程的文件描述符
    if(pid == 0){ // 子进程返回0
        char recv_buff[255] = {0};
        read(file_des[0],recv_buff,255);
        printf("接受到数据%s\n",recv_buff);
        close(file_des[0]);
    }else if(pid > 0){ // 父进程返回pid
        // file_des[0]读 file_des[1]写
        char buff[255] = {"hello world!"};
        write(file_des[1],buff,sizeof(buff));
        close(file_des[1]);
    }else{
        perror("fork error\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
