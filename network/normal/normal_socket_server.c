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

#define SERVER_IP "172.16.1.171"
#define PORT 1235
#define MAXFD 10

/**
 * 阻塞的Socket服务器端
 */
int main() {
    // domain:协议,IPV4或者IPV6或者其它
    // type:socket类型,流式套接字或者数据报套接字
    // protocol:0
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
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (bind(lfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        exit(-1);
    }

    // 设置socket为监听模式,第二个参数表示能处理的最大连接数,AF_INET最大128
    if (listen(lfd, MAXFD) == -1) {
        perror("listen");
        exit(-1);
    }

    while (1) {
        printf("等待连接......\n");
        // 接收新连接
        struct sockaddr_in acceptAddr;
        memset(&acceptAddr, 0, sizeof(acceptAddr));
        socklen_t addrLen = 0;
        int acceptFd = accept(lfd, (struct sockaddr *) &acceptAddr, &addrLen);

        if (acceptFd < 0) {
            perror("accept");
            exit(-1);
        }

        printf("接收到新连接:%d\n", acceptFd);

        char buf[255];
        memset(buf, 0, 255);
        int len = recv(acceptFd, buf, 255, MSG_WAITALL);
        printf("接收到数据:%s,长度:%d\n", buf, len);
    }
}