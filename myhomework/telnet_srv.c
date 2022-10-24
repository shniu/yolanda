#include "lib/common.h"

char *run_cmd(char *cmd) {
    char *data = malloc(16384);
    bzero(data, sizeof(data));

    FILE *fdp;
    const int max_buffer = 256;
    char buffer[max_buffer];
    fdp = popen(cmd, "r");
    char *data_index = data;

    if (fdp) {
        while (!feof(fdp)) {
            if (fgets(buffer, max_buffer, fdp) != NULL) {
                int len = strlen(buffer);
                memcpy(data_index, buffer, len);
                data_index += len;
            }
        }
        pclose(fdp);
    }

    return data;
}

int main(int argc, char **argv) {
    int listen_fd;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(7777);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    if (bind(listen_fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0) {
        error(EXIT_FAILURE, 0, "bind failed ");
    }

    if (listen(listen_fd, 1024) < 0) {
        error(EXIT_FAILURE, 0, "listen failed ");
    }

    signal(SIGPIPE, SIG_IGN);

    struct sockaddr_in client_addr;
    socklen_t  client_addr_len = sizeof(client_addr);
    int conn_fd;

    char buf[512];
    int count = 0;

    for (;;) {
        conn_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_addr_len);
        if (conn_fd < 0) {
            error(EXIT_FAILURE, 0, "accept failed ");
        }

        while (1) {
            bzero(buf, sizeof(buf));
            ssize_t n = read(conn_fd, buf, sizeof(buf));
            if (n < 0) {
                error(EXIT_FAILURE, 0, "read error ");
            } else if (n == 0) {
                printf("client closed \n");
                close(conn_fd);
                break;
            }

            count++;
            buf[n] = 0;

            if (strncmp(buf, "ls", n) == 0) {
                char *res = run_cmd("ls");
                if (send(conn_fd, res, strlen(res), 0) < 0) {
                    return EXIT_FAILURE;
                }
                free(res);
            } else if (strncmp(buf, "pwd", n) == 0) {
                char p[256];
                // 获取当前目录的命令
                char *res = getcwd(p, 256);
                if (send(conn_fd, res, strlen(res), 0) < 0)
                    return EXIT_FAILURE;
            } else if (strncmp(buf, "cd ", 3) == 0) {
                char target[256];
                bzero(target, sizeof(target));
                memcpy(target, buf + 3, strlen(buf) - 3);
                // 切换目录的命令
                if (chdir(target) == -1) {
                    printf("change dir failed, %s\n", target);
                }
                char *ok = "OK";
                send(conn_fd, ok, strlen(ok), 0);
            } else {
                char *error = "error: unknown command <";

                size_t err_len = strlen(error) + strlen(buf) + 1;

                char res[err_len];
                bzero(res, sizeof(res));

                strcat(res, error);
                strcat(res, buf);
                strcat(res, ">");

                if(send(conn_fd, res, strlen(res), 0) < 0) {
                    return EXIT_FAILURE;
                }
            }
        }
    }
}
