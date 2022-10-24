#include "lib/common.h"

void process(int conn_fd) {
    char buf[1024];

    // 存储当前路径
    char curr_path[1024];
    getcwd(curr_path, sizeof(curr_path));
    fprintf(stdout, "curr path: %s\n", curr_path);

    for (;;) {
        ssize_t n = read(conn_fd, buf, 1024);
        if (n < 0) {
            printf("error read.\n");
            break;
        } else if (n == 0) {
            printf("client closed. \n");
            break;
        }

        buf[n] = 0;
        // printf("received %zd bytes: %s\n", n, buf);

        if (strncmp(buf, "pwd", 3) == 0) {
//            char p[1024];
//            getcwd(p, sizeof(p));
//            fprintf(stdout, "pwd: %s\n", p);

            ssize_t write_len = send(conn_fd, curr_path, strlen(curr_path), 0);
            printf("send bytes: %zu \n", write_len);
            if (write_len < 0) {
                printf("error write \n");
            }
        } else if (strncmp(buf, "cd", 2) == 0) {
            char send_line[1024];
            sprintf(send_line, "Hi, %s", buf);

            ssize_t write_len = send(conn_fd, send_line, strlen(send_line), 0);
            printf("send bytes: %zu \n", write_len);
        } else if (strncmp(buf, "ls", 2) == 0) {
            char send_line[1024];
            sprintf(send_line, "Hi, %s", buf);

            ssize_t write_len = send(conn_fd, send_line, strlen(send_line), 0);
            printf("send bytes: %zu \n", write_len);
        } else {
            printf("not support command: %s \n", buf);
            char no_support[1024];
            sprintf(no_support, "not support command: %s", buf);
            ssize_t send_len = send(conn_fd, no_support, strlen(no_support), 0);
            if (send_len < 0) {
                printf("error write \n");
            }
        }
    }
}

int main(int argc, char **argv) {
    int listen_fd;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    // bind
    struct sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(7777);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0) {
        error(EXIT_FAILURE, 0, "bind failed ");
    }

    // listen
    if (listen(listen_fd, 1024) < 0) {
        error(EXIT_FAILURE, 0, "listen failed ");
    }

    for (;;) {
        struct sockaddr_in client_addr;
        socklen_t  client_addr_len = sizeof(client_addr);

        int conn_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_addr_len);

        printf("new conn %d\n", conn_fd);
        process(conn_fd);
        close(conn_fd);
    }
}
