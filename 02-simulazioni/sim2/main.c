#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define EXIT_ARGS 3
#define EXIT_N 4
#define EXIT_FILE 5
#define EXIT_ERROR 6
#define MAX_LEAVES 10
#define KILL_ALL 0
#define KILL_ONE 1

pid_t leaves[MAX_LEAVES] = {-1};
int active_leaves;
extern int errno;

void check_args(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <target> <n>\n", argv[0]);
        exit(EXIT_ARGS);
    }
    int n = atoi(argv[2]);
    if (n < 1 || n > MAX_LEAVES) {
        fprintf(stderr, "n must be between 1 and 10, got %s\n", argv[2]);
        exit(EXIT_N);
    }
}

void kill_leaves(pid_t pid, int kill_one) {
    union sigval value;
    value.sival_int = pid;
    for (int i = 0; i < MAX_LEAVES; i++) {
        if (leaves[i] != -1) {
            sigqueue(leaves[i], SIGUSR2, value);
            leaves[i] = -1;
            active_leaves--;
            if (kill_one)
                break;
        }
    }
}

void manager_handler(int signo, siginfo_t *info, void *empty) {
    if (signo == SIGTERM) {
        kill_leaves(info->si_pid, KILL_ALL);
        exit(0);
    }
    if (signo == SIGUSR1) {
        kill_leaves(info->si_pid, KILL_ONE);
    }
}

void leaf_handler(int signo, siginfo_t *info, void *empty) {
    kill(info->si_value.sival_int, SIGUSR2);
}

int manager_main(int fd, int n) {
    pid_t manager = getpid();

    sigset_t set;
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    for (int i = 0; i < n; i++) {
        if (manager == getpid()) {
            pid_t isManager = fork();
            if (isManager) {
                dprintf(fd, "%d\n", isManager);
                leaves[i] = isManager;
                active_leaves++;
            } else {
                close(fd);
                sa.sa_sigaction = leaf_handler;
                sigaction(SIGUSR2, &sa, NULL);
                sigemptyset(&set);
                sigaddset(&set, SIGCHLD);
                sigaddset(&set, SIGCONT);
                sigprocmask(SIG_BLOCK, &set, NULL);
                pause();
                return 0;
            }
        }
    }
    close(fd);

    sa.sa_sigaction = manager_handler;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sigprocmask(SIG_BLOCK, &set, NULL);

    while (active_leaves > 0) {
        pause();
    }
    return 0;
}

int main(int argc, char **argv) {
    check_args(argc, argv);

    int fd = open(argv[1], O_WRONLY | O_APPEND | O_CREAT | O_EXCL, 0777);
    if (fd == -1) {
        perror("Error reading file");
        exit(EXIT_FILE);
    }
    int n = atoi(argv[2]);

    dprintf(fd, "%d\n", getpid());
    pid_t isParent = fork();
    if (isParent) {
        dprintf(fd, "%d\n", isParent);
        close(fd);
        return 0;
    } else {
        manager_main(fd, n);
    }
    return errno == 0 ? 0 : EXIT_ERROR;
}
