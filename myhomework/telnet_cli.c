#include "lib/common.h"

#define  MAXLINE     1024


int main(int argc, char **argv) {
    if (argc != 3) {
        error(1, 0, "usage: tcp_client <IPaddress> <port>");
    }
    int port = atoi(argv[2]);
    int socket_fd = tcp_client(argv[1], port);

    char recv_line[MAXLINE], send_line[MAXLINE];
    int n;

    fd_set readmask;
    fd_set allreads;

    // 把 fd_set 置为 0
    FD_ZERO(&allreads);
    // 把 stdin 加入到 fd_set 中，这样就可以 read from stdin
    FD_SET(0, &allreads);
    // 把 socket_fd 加入到 fd_set 中，这样就可以监听 socket 的可读事件
    FD_SET(socket_fd, &allreads);

    for (;;) {
        // 这里很重要，要重置待测试的描述符集合
        readmask = allreads;

        // 最后一个参数是 timeout，如果指定为 null，select 会一直阻塞，直到有事件到达
        // 只设置了 readfds，也就是只监控有数据可读取的事件
        int rc = select(socket_fd + 1, &readmask, NULL, NULL, NULL);

        if (rc <= 0) {
            error(1, errno, "select failed");
        }

        // socket_fd 中是否有事件
        if (FD_ISSET(socket_fd, &readmask)) {
            n = read(socket_fd, recv_line, MAXLINE);
            if (n < 0) {
                error(1, errno, "read error");
            } else if (n == 0) {
                printf("server closed \n");
                break;
            }
            recv_line[n] = 0;
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }

        // stdin 是否有事件
        if (FD_ISSET(STDIN_FILENO, &readmask)) {
            // 从 stdin 等待输入
            if (fgets(send_line, MAXLINE, stdin) != NULL) {
                int i = strlen(send_line);
                if (send_line[i - 1] == '\n') {
                    send_line[i - 1] = 0;
                }

                if (strncmp(send_line, "quit", strlen(send_line)) == 0) {
                    if (shutdown(socket_fd, 1)) {
                        error(1, errno, "shutdown failed");
                    }
                }

                // 把输入的东西发给 server
                size_t rt = write(socket_fd, send_line, strlen(send_line));
                if (rt < 0) {
                    error(1, errno, "write failed ");
                }
            }
        }
    }

    exit(0);
}
