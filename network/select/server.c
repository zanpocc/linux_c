//
// Created by cg on 2021/12/23.
//

#include<sys/types.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <memory.h>
#include<arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_IP "172.16.1.171"
#define PORT 1235
#define MAXFD 5000

/**
 * 使用select实现的单线程网络模型：
 * 缺点：
 * 1、fd_set大小有限, 只能监听1024个fd的读写状态变化
 * 2、无状态,每次调用都需要重新设置，需要遍历所有监听的fd，检查是否状态发生变化
 */

int server_socket_ipv4_tcp(char *ip, int port);

void readLine(int fd, char *buff);

int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int main() {

    // 维护客户端fd
    int fds[MAXFD];
    memset(fds, 0, sizeof(int) * MAXFD);
    // 客户端描述数量
    int fdIndex = 1;
    // 创建服务器socket
    int sfd = server_socket_ipv4_tcp((char *) SERVER_IP, PORT);

    // 最大文件描述符
    int maxFd = sfd;

    fd_set readFdSet;
    // 清空要监控的集合
    FD_ZERO(&readFdSet);

    // 设置要监控的fd到集合
    FD_SET(sfd, &readFdSet);
    fds[0] = sfd;

    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = 1000;

    while (1) {
        printf("等待事件:%d...\n", maxFd);
        fflush(stdout);
        int count = select(maxFd + 1, &readFdSet, NULL, NULL, NULL);
        if (count == -1) {
            perror("select");
            exit(-1);
        }

        if (FD_ISSET(sfd, &readFdSet)) {
            // 有连接到来
            struct sockaddr_in clientSockAddr;
            memset(&clientSockAddr, 0, sizeof(clientSockAddr));
            socklen_t len;

            printf("等待连接...\n");
            fflush(stdout);

            int clientFd = accept(sfd, (struct sockaddr *) &clientSockAddr, &len);
            if(clientFd == -1){
                perror("accept");
                exit(-1);
            }

            printf("有连接到来,IP：%s,port:%d,fd:%d\n", inet_ntoa(clientSockAddr.sin_addr), clientSockAddr.sin_port,clientFd);
            fflush(stdout);

            FD_SET(clientFd, &readFdSet);
            // 保存连接
            maxFd = max(maxFd, clientFd);
            fds[fdIndex++] = clientFd;
        } else {
            // 有数据到来
            int i = 0;
            for (; i < MAXFD; i++) {
                int clientFd = fds[i];
                if (clientFd <= 0) {
                    continue;
                }
                if (FD_ISSET(clientFd, &readFdSet)) {
                    char buff[255];
                    memset(&buff, 0, 255);

                    printf("读取数据...\n");
                    fflush(stdout);

                    readLine(clientFd, buff);

                    printf("接收到:%d的数据:%s\n", clientFd, buff);
                    fflush(stdout);
                }
            }
        }

        // 把所有的fd都放入监控,因为没有事件的FD对应的位会被清空
        FD_ZERO(&readFdSet);
        int i = 0;
        for (i; i <= fdIndex; i++) {
            if (fds[i] != 0) {
                FD_SET(fds[i], &readFdSet);
            }
        }
    }
    return 0;
}

int server_socket_ipv4_tcp(char *ip, int port) {
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("socket");
        exit(-1);
    }

    // sa_family与创建socket的一致
    // sa_data依据sa_family的不同，结构不同
    struct sockaddr_in addr;

    // 绑定本机所有地址的PORT
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    // 防止终止后还占用端口
    int opt = 1;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof( opt ));

    if (bind(lfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(-1);
    }

    // 设置socket为监听模式,第二个参数表示能处理的最大连接数,AF_INET最大128
    if (listen(lfd, MAXFD) == -1) {
        perror("listen");
        exit(-1);
    }

    return lfd;
}

void readLine(int fd, char *buff) {
    char c;
    int i = 0;
    while (1) {
        read(fd, &c, 1);
        if (c == '\n') {
            buff[i] = c;
            break;
        } else {
            buff[i] = c;
            i++;
        }
    }
}