//
// Created by cg on 2021/12/24.
//



#include <poll.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <memory.h>

/**
 * 使用poll实现的网络模型，相比与select模型，
 * 优点：
 * 1、可以监控的连接数没有限制
 * 2、不需要再次将描述符重新加入待检测集合中。
 *
 * 缺点：也是缺少状态，系统调用时每次需要从用户空间到内核空间复制待检测的fd
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
    int lfd = server_socket_ipv4_tcp(SERVER_IP,PORT);
    int maxFd = lfd;
    struct pollfd pollfds[MAXFD];
    struct pollfd listenPollfd;
    listenPollfd.fd = lfd;
    listenPollfd.events = POLLIN; // 监听读事件
    listenPollfd.revents = 0;
    pollfds[0] = listenPollfd;

    // 初始化其它的为-1
    int i = 1;
    for (; i < MAXFD; ++i) {
        pollfds[i].fd = -1;
        pollfds[i].events = POLLIN;
        pollfds[i].revents = 0;
    }

    for (;;){
        printf("等待事件到来\n");
        fflush(stdout);

        // -1表示阻塞
        int count = poll((struct pollfd *) &pollfds, maxFd, -1);
        if(count > 0){
            printf("事件到来数量:%d\n",count);
            fflush(stdout);
        }

        // 监听socket发生读事件:有新连接到来
        if(pollfds[0].revents & POLLIN){
            printf("新连接到来\n");
            fflush(stdout);

            struct sockaddr_in clientSock;
            socklen_t len;
            int clientFd = accept(listenPollfd.fd, (struct sockaddr *) &clientSock, &len);

            // 找到一个坑放进去
            i = 1;
            for (; i < MAXFD; ++i) {
                if(pollfds[i].fd < 0){
                    pollfds[i].fd = clientFd;
                    break;
                }
            }
            maxFd = max(maxFd,clientFd);

            // 放满了
            if(i == MAXFD){
                perror("too many clients");
                break;
            }
        }

        // 已连接的socket发生读事件：有客户端数据到来
        i = 1;
        for (; i <= maxFd; ++i) {
            //
            struct pollfd pollfd = pollfds[i];
            if(pollfd.fd < 0){
                continue;
            }

            // 发生读事件
            if(pollfd.revents & POLLIN){
                char buff[255];
                memset(&buff,0,255);
                readLine(pollfd.fd,buff);

                printf("接收到:%d的数据：%s\n",pollfd.fd,buff);
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