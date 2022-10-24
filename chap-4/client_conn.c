#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>

int make_conn(char *srv_host, uint16_t srv_port) {
    int sockfd;
    struct sockaddr_in srv_addr;

    // 创建 socket
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // bzero 目的是初始化 sockaddr_in ，确保都是零值
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(srv_port);
    // inet_pton 的作用是把一个字符数组表示的 ip 地址转成 network format (in network byte order)
    int valid = inet_pton(AF_INET, srv_host, &srv_addr.sin_addr);
    if (valid < 1) {
        perror("wrong ip addr");
        exit(EXIT_FAILURE);
    }

    socklen_t srvaddr_len = sizeof(srv_addr);
    int conn_rt = connect(sockfd, (struct sockaddr *) & srv_addr, srvaddr_len);
    if (conn_rt < 0) {
        perror("connection failed");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

int main(int argc, char **argv) {
    int connfd = make_conn("127.0.0.1", 12345);
    printf("connfd = %d", connfd);
    close(connfd);
    exit(0);
}