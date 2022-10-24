#include "lib/common.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        error(EXIT_FAILURE, 0, "usage: homeclient <ip> <port>");
    }

    // argv[1]  ip
    // argv[2]  port

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error(EXIT_FAILURE, 0, "socket created error ");
    }

    // connect
    struct sockaddr_in srv_addr;

    bzero(&srv_addr, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    char * port = argv[2];
    srv_addr.sin_port = htons(atoi(port));
    inet_pton(AF_INET, argv[1], &srv_addr.sin_addr);

    socklen_t socklen = sizeof(srv_addr);
    int conn_rt = connect(sockfd, (struct sockaddr *) &srv_addr, socklen);
    if (conn_rt < 0) {
        error(EXIT_FAILURE, 0, "connect failed ");
    }

    char read_buf[1024], buf[1024];

    while (1) {
        if (fgets(buf, sizeof(buf), stdin) != NULL) {

            size_t input_len = strlen(buf);
            buf[input_len] = 0;

            if (strncmp(buf, "quit", 4) == 0) {
                break;
            }

            ssize_t write_len = send(sockfd, buf, input_len - 1, 0);
            printf("send bytes: %zu, %s", write_len, buf);

            bzero(&read_buf, sizeof(read_buf));
            ssize_t n = read(sockfd, read_buf, 1024);
            printf("read %zd bytes: %s\n", n, read_buf);
        }
    }

    printf("client close. \n");
    close(sockfd);
    sleep(30);
}
