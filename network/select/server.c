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
#include <sys/select.h>
#include <unistd.h>

#define SERVER_IP "172.16.1.171"
#define PORT 1235
#define MAXFD 10

int server_socket_ipv4_tcp(char *ip, int port);

void readLine(int fd, char *buff);


int main() {
    // 维护客户端fd
    int fds[MAXFD];
    memset(fds, 0, sizeof(int) * MAXFD);
    // 客户端描述数量
    int fdIndex = 0;
    // 创建服务器socket
    int sfd = server_socket_ipv4_tcp((char *) SERVER_IP, PORT);
    fd_set readFdSet, writeFdSet;
    // 清空要监控的集合
    FD_ZERO(&readFdSet);
    FD_ZERO(&writeFdSet);
    // 设置要监控的fd到集合
    FD_SET(sfd, &readFdSet);

    struct timeval time;
    time.tv_sec = 0;
    time.tv_usec = 1000;

    while (1) {
        int count = select(sfd + 1, &readFdSet, &writeFdSet, NULL, NULL);
        if (count == -1) {
            perror("select");
            exit(-1);
        }

        if (count > 0) {
            printf("count>0\n");
        }

        // 有连接到来
        if (FD_ISSET(sfd, &readFdSet)) {
            printf("有连接到来\n");
            struct sockaddr_in clientSockAddr;
            socklen_t len;

            int clientFd = accept(sfd, (struct sockaddr *) &clientSockAddr, &len);
            printf("有连接到来,IP：%s,port:%d\n", inet_ntoa(clientSockAddr.sin_addr), clientSockAddr.sin_port);
            FD_SET(clientFd, &writeFdSet);
            // 保存连接
            fds[fdIndex] = clientFd;
            fdIndex++;
        }

        // 有数据到来
        int i = 0;
        for (; i < MAXFD; i++) {
            if (fds[i] == 0) {
                continue;
            }
            if (FD_ISSET(fds[i], &writeFdSet)) {
                printf("有数据到来\n");
                char buff[255];
                memset(buff, 0, 255);
                int clientFd = fds[i];
                readLine(clientFd, buff);
                printf("接收到:%d的数据:%s\n", clientFd, buff);
            }
        }

        i = 0;
        FD_ZERO(&writeFdSet);
        for (; i < MAXFD; i++) {
            if (fds[i] == 0) {
                continue;
            }
            FD_SET(fds[i], &writeFdSet);
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