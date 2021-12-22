//
// Created by cg on 2021/10/18.
//

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(){

    const char *fifo_path = "/Users/chengong/Desktop/tmp_fifo";

    // 创建一个fifo
    if(mkfifo(fifo_path,0644)){
        perror("makefifo error\n");
        exit(EXIT_FAILURE);
    }

    // 打开子进程，读取

    int pid = fork();
    if(pid == 0){
        int fd = open(fifo_path,O_RDONLY);
        char buff[255] = {0};
        read(fd,buff,sizeof(buff));
        printf("子进程读取成功:%s\n",buff);
        close(fd);
    } else if(pid > 0){
        int fd = open(fifo_path,O_WRONLY);
        char buff[255] = {"hello world!"};
        write(fd,buff,sizeof(buff));
        printf("父进程写入成功\n");
        close(fd);
    }else{
        perror("fork error\n");
        exit(EXIT_FAILURE);
    }

    // 删除fifo文件
    remove(fifo_path);
    return 0;
}