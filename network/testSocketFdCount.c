//
// Created by cg on 2021/12/28.
//

#include <printf.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "172.16.1.171"
#define PORT 1235
#define MAXFD 10000

int server_socket_ipv4_tcp(char *ip, int port);
void readLine(int fd, char *buff);

/**
 * 测试最大fd数量
 */
//int main(){
//    int i = 9000;
//
//    for (; i < 11000; ++i) {
//        int fd = server_socket_ipv4_tcp((char*)"127.0.0.1",i);
//        printf("监听:%d端口,fd:%d\n",i,fd);
//    }
//    printf("完成\n");
//}

int main(){
    int lfd = server_socket_ipv4_tcp((char*)"172.16.1.171",9000);
    int i = 0;

    struct sockaddr_in addr;
    socklen_t len;
    while(1){
        printf("等待连接,%d\n",i);
        fflush(stdout);
        int clientFd= accept(lfd, (struct sockaddr *) &addr, &len);
        if(clientFd == -1){
            perror("accept\n");
        }
        i++;
        printf("接收到连接:%d,fd:%d\n",i,clientFd);
        fflush(stdout);
    }
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