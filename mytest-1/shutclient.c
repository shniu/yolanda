#include "lib/common.h"

#define    SRV_PORT      43211
#define    MAXLINE      4096

int main(int argc, char **argv) {
    if (argc != 2) {
        error(1, 0, "usage: shutclient <IPaddress>");
    }

    // 创建 socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 构造 服务端的地址 sockaddr_in
    struct sockaddr_in srv_addr;
    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port = htons(SRV_PORT);
    inet_pton(AF_INET, argv[1], &srv_addr.sin_addr);

    // connect to server
    socklen_t srv_len = sizeof(srv_addr);
    int conn_rt = connect(sockfd, (struct sockaddr *) &srv_addr, srv_len);
    if (conn_rt < 0) {
        error(1, errno, "connect failed");
    }

    char send_line[MAXLINE], recv_line[MAXLINE + 1];
    int n;

    fd_set readmask;
    fd_set allreads;

    FD_ZERO(&allreads);
    FD_SET(0, &allreads);
    FD_SET(sockfd, &allreads);

    for (;;) {
        readmask = allreads;

        select(sockfd + 1, &readmask, NULL, NULL, NULL);
        // TODO
    }
}