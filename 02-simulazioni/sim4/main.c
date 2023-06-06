#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <unistd.h>

#define EXIT_ARGS 1
#define EXIT_FILE 2

int fd;

void check_args(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pathToLogFile>\n", argv[0]);
        exit(EXIT_ARGS);
    }
}

// Signal Logger
void update_logfile(pid_t pidA, int signo) {
    pid_t isMain = fork();
    if (!isMain) {
        sleep(3);
        dprintf(fd, "%d-%d\n", pidA, signo);
        exit(0);
    }
}

void handlerUSR(int signo, siginfo_t *info, void *empty) {
    // Repeater
    if (signo == SIGUSR1) {
        kill(info->si_pid, SIGUSR1);
    }
    // Relay di un segnale
    if (signo == SIGUSR2) {
        pid_t isMain = fork();
        if (!isMain) {
            kill(info->si_pid, SIGUSR2);
            exit(0);
        }
    }
    update_logfile(info->si_pid, signo);
}

void handlerINT(int signo) {
    dprintf(fd, "stop\n");
    exit(0);
}

void signal_handling_setup() {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

    sa.sa_sigaction = handlerUSR;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    signal(SIGINT, handlerINT);
}

int main(int argc, char **argv) {
    check_args(argc, argv);

    fd = open(argv[1], O_WRONLY | O_APPEND | O_CREAT, 0777);
    if (fd == -1)
        return EXIT_FILE;

    signal_handling_setup();

    key_t qKey = ftok(argv[1], 1);
    int qId = msgget(qKey, 0777 | IPC_CREAT);

    struct msg_t {
        long mtype;
        pid_t pid;
    } msg;

    while (1) {
        msgrcv(qId, &msg, sizeof(msg.pid), 0, 0);
        kill(msg.pid, SIGALRM);
    }
}
