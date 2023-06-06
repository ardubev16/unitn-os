#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define READ 0
#define WRITE 1

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s n\n", argv[0]);
        return 1;
    }

    int childn = atoi(argv[1]);
    if (childn <= 0) {
        printf("Insert a valid number (n >= 1)\n");
        return 1;
    }
    int fd[2];

    uint sum = 0;
    int buf;
    for (int i = 0; i < childn; i++) {
        pipe(fd);
        if (!fork()) {
            close(fd[READ]);
            srand(time(NULL) + getpid());
            uint rn = rand() % 100;
            printf("Child %d: %d\n", i + 1, rn);
            write(fd[WRITE], &rn, sizeof(buf));
            close(fd[WRITE]);
            return 0;
        }
        close(fd[WRITE]);
        read(fd[READ], &buf, sizeof(buf));
        sum += buf;
        close(fd[READ]);
    }

    printf("Final sum: %d\n", sum);
    return 0;
}
