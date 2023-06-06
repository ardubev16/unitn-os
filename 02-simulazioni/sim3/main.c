#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EXIT_ARGS 1
#define EXIT_FILE 2
#define EXIT_MAXCHILD 3
#define EXIT_PID 4

#define MAX_CHILDREN 128

int fd;
pid_t children[MAX_CHILDREN] = {-1};
int childn = 0;

void check_args(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <target> <n>\n", argv[0]);
        exit(EXIT_ARGS);
    }
}

int readline(int fd, char *buf, int size) {
    int i = 0;
    char c;
    while (read(fd, &c, 1) && c != '\n' && i < size) {
        buf[i] = c;
        i++;
    }
    buf[i] = '\0';
    return i;
}

int open_file_if_no_exist(char *path) {
    int fd = open(path, O_WRONLY | O_APPEND | O_CREAT | O_EXCL, 0777);
    if (fd == -1) {
        perror("Error reading file");
        exit(EXIT_FILE);
    }
    return fd;
}

void server_handler(int signo) {
    if (signo == SIGUSR1) {
        if (childn >= MAX_CHILDREN) {
            fprintf(stderr, "Too many children, max %d\n", MAX_CHILDREN);
            exit(EXIT_MAXCHILD);
        }
        pid_t isServer = fork();
        if (isServer) {
            dprintf(fd, "+%d\n", isServer);
            printf("[server] %d\n", isServer);
            children[childn] = isServer;
            childn++;
        } else {
            close(fd);
            pause();
        }
    }
    if (signo == SIGUSR2) {
        if (childn > 0) {
            int to_kill = children[childn - 1];
            dprintf(fd, "-%d\n", to_kill);
            printf("[server] %d\n", to_kill);
            kill(to_kill, SIGINT);
            childn--;
        } else {
            dprintf(fd, "0\n");
            printf("[server] 0\n");
        }
    }
    if (signo == SIGINT) {
        dprintf(fd, "%d\n", childn);
        exit(EXIT_SUCCESS);
    }
}

int mode_server(char *path) {
    fd = open_file_if_no_exist(path);
    pid_t myPid = getpid();
    dprintf(fd, "%d\n", myPid);
    printf("[server:%d]\n", myPid);

    signal(SIGUSR1, server_handler);
    signal(SIGUSR2, server_handler);
    signal(SIGINT, server_handler);

    while (1)
        pause();
}

int mode_client(char *path) {
    do {
        fd = open(path, O_RDONLY);
    } while (fd == -1);
    char buf[32];
    readline(fd, buf, 32);
    pid_t pid = atoi(buf);
    if (pid <= 0) {
        fprintf(stderr, "Invalid pid read from file: %s\n", buf);
        exit(EXIT_PID);
    }
    printf("[client] server %d\n", pid);
    int counter = 0;

    while (1) {
        int c = getchar();
        if (c == '+' && counter < 10) {
            kill(pid, SIGUSR1);
            counter++;
            printf("[client] %d\n", counter);
        }
        if (c == '-' && counter > 0) {
            kill(pid, SIGUSR2);
            counter--;
            printf("[client] %d\n", counter);
        }
        if (c == '\n') {
            for (int i = counter; i > 0; i--) {
                kill(pid, SIGUSR2);
                printf("[client] %d\n", i);
                sleep(1);
            }
            kill(pid, SIGINT);
            exit(EXIT_SUCCESS);
        }

        while (getchar() != '\n')
            ;
    }
}

int main(int argc, char **argv) {
    check_args(argc, argv);

    char *mode = argv[1];
    if (strcmp(mode, "server") == 0) {
        mode_server(argv[2]);
    } else if (strcmp(mode, "client") == 0) {
        mode_client(argv[2]);
    } else {
        fprintf(stderr, "Invalid mode %s\n", mode);
        exit(EXIT_ARGS);
    }
}
