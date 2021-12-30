//
// Created by cg on 2021/12/27.
//


#include <sys/epoll.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <memory.h>

/**
 *  Linux网络模型epoll实现
 *  优点：
 *  1、O(1)复杂度获取发生事件的描述符
 *  2、有状态，存在内核空间数据结构
 *  3、可以处理大量的文件描述符
 *
 */


#define SERVER_IP "172.16.1.171"
#define PORT 1235
#define MAXFD 10000

// 函数声明
int server_socket_ipv4_tcp(char *ip, int port);
void readLine(int fd, char *buff);
int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

int main(){

    // 创建服务器socket
    int lfd = server_socket_ipv4_tcp(SERVER_IP,PORT);
    // 创建epoll对象,最大的文件描述符数量
    int epollObj = epoll_create(MAXFD);

    // 事件数组,接收事件
    struct epoll_event totalEvent[MAXFD];
    memset(&totalEvent, -1, sizeof(totalEvent));

    // 监听socket事件
    struct epoll_event listenEvent;
    listenEvent.events = EPOLLIN; // 使用ET加速,就绪只发送一次通知;
    listenEvent.data.fd = lfd;

    // 注册监听socket到epoll
    if(epoll_ctl(epollObj,EPOLL_CTL_ADD,lfd,&listenEvent) == 0){
        printf("将监听socket放入epoll对象中\n");
        fflush(stdout);
    }

    int connectCount = 0;

    while(1){
        // -1表示阻塞,阻塞等待事件发生
        printf("等待事件,当前连接数：%d\n",connectCount);
        fflush(stdout);

        int count = epoll_wait(epollObj,totalEvent,MAXFD,-1);
        if(count > 0){
            printf("有事件到来\n");
            fflush(stdout);
        }

        // 遍历所有事件,从事件数组中取出前count个
        int i = 0;

        for (; i < count; ++i) {
            struct epoll_event ev = totalEvent[i];
            // 从事件数组中取出文件描述符
            if(ev.data.fd == lfd){// 如果是监听fd,有新连接到来
                printf("新连接到来\n");
                fflush(stdout);

                struct sockaddr_in clientSockAddr;
                socklen_t len;
                int clientFd = accept(lfd, (struct sockaddr *) &clientSockAddr, &len);
                if(clientFd == -1){
                    perror("accept\n");
                }

                // 注册到epoll
                struct epoll_event clientEvent;
                clientEvent.data.fd = clientFd;
                clientEvent.events = EPOLLIN; // 使用ET加速,就绪只发送一次通知

                if(epoll_ctl(epollObj,EPOLL_CTL_ADD,clientFd,&clientEvent) == 0){
                    printf("接收到新连接,注册到epoll成功\n");
                    fflush(stdout);
                }else{
                    perror("注册到epoll失败\n");
                }

                connectCount++;

            }else if(ev.events & EPOLLIN){ // 如果是其它的fd,则表示客户端有数据到来
                printf("数据到来\n");
                fflush(stdout);
                int clientFd = ev.data.fd;
                char buff[255];
                readLine(clientFd, (char *) &buff);
                printf("接收到客户端FD:%d的数据:%s\n",clientFd,buff);
                fflush(stdout);
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