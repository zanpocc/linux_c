//
// Created by cg on 2021/12/23.
//

#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>

#define PORT 1235
#define SERVER_IP "172.16.1.171"

/**
 * 阻塞的Socket客户端
 */
int main() {

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    addr.sin_port = htons(PORT);

    int sfd = connect(fd, (const struct sockaddr *) &addr, sizeof(addr));
    if (sfd == -1) {
        perror("connect");
        exit(-1);
    }

    printf("连接到服务器成功\n");
    fflush(stdout);

    char buf[255];

    while(1){
        memset(buf, 0, 255);
        // 从控制台读取输入到缓冲区
        read(STDIN_FILENO, buf, sizeof(buf));
        // 发送缓冲区数据到服务器
        int len = send(fd, buf, strlen(buf), MSG_WAITALL);
        if (len >= 0) {
            printf("发送数据成功,len:%d\n", len);
            fflush(stdout);
        }
    }

    return 0;
}