#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define EXIT_ARGS 1
#define EXIT_FILE 2

void check_args(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <target> <n>\n", argv[0]);
        exit(EXIT_ARGS);
    }
}

int open_file_if_no_exist(char *path) {
    int fd = open(path, O_WRONLY | O_APPEND | O_CREAT | O_EXCL, 0777);
    if (fd == -1) {
        perror("Error reading file");
        exit(EXIT_FILE);
    }
    return fd;
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

void handler_sigaction(int signo, siginfo_t *info, void *empty);
void handler_simple(int signo);

void signal_handling_setup() {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

    sa.sa_sigaction = handler_sigaction;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    signal(SIGINT, handler_simple);
}

int main(int argc, char **argv) {
    check_args(argc, argv);
    // TODO: examination, Barbie is your creation
}
