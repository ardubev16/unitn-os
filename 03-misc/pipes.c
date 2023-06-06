#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define WRITE 1
#define READ 0
#define BUF_LEN 50

int cnt = 0;

void send_cnt(int fd) {
    dprintf(fd, "%d", cnt);
    cnt++;
}

int recv_data(int fd, char *buf) {
    read(fd, buf, BUF_LEN);
    if (!strcmp(buf, "EOF"))
        return 1;
    return 0;
}

int main() {
    int pipe1[2], pipe2[2];
    char buf[50];

    pipe(pipe1);
    pipe(pipe2);

    pid_t p2 = !fork();

    if (p2) {
        close(pipe1[WRITE]);
        close(pipe2[READ]);
        for (int i = 0; i < 10; i++) {
            send_cnt(pipe2[WRITE]);
            int exit = recv_data(pipe1[READ], buf);
            printf("recvd 1: %s, sent 1: %d\n", buf, cnt);
            if (exit) {
                close(pipe1[READ]);
                break;
            }
            sleep(1);
        }
        write(pipe2[WRITE], "EOF", 3);
        close(pipe2[WRITE]);
    } else {
        close(pipe1[READ]);
        close(pipe2[WRITE]);
        for (int i = 0; i < 15; i++) {
            send_cnt(pipe1[WRITE]);
            int exit = recv_data(pipe2[READ], buf);
            printf("recvd 2: %s, sent 2: %d\n", buf, cnt);
            if (exit) {
                close(pipe2[READ]);
                break;
            }
        }
        write(pipe1[WRITE], "EOF", 3);
        close(pipe1[WRITE]);
    }
}
