#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>

struct msg_buffer {
    long mtype;
    char mtext[100];
} msg;

void usage(char *prog) {
    printf("Usage: %s <s|r>\n", prog);
    exit(-1);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
    }
    if (argv[1][0] != 's' && argv[1][0] != 'r') {
        usage(argv[0]);
    }

    open("/tmp/unique", O_CREAT, 0777);
    key_t queue1Key = ftok("/tmp/unique", 1);
    int queueId = msgget(queue1Key, 0777 | IPC_CREAT);
    if (queueId == -1) {
        perror("msgget");
        exit(-1);
    }

    if (argv[1][0] == 's') {
        printf("Inserisci la priorita' del messaggio: ");
        scanf("%ld", &msg.mtype);
        if (msg.mtype < 1 || msg.mtype > 10) {
            printf("Priorita' non valida\n");
            exit(-1);
        }
        printf("Inserisci il messaggio da inviare con priorita' %ld: ",
               msg.mtype);
        scanf("%99s", msg.mtext);
        int res = msgsnd(queueId, &msg, sizeof(msg.mtext), 0);
        if (res == -1) {
            perror("msgsnd");
            exit(-1);
        }
    } else {
        for (int i = 10; i >= 1; i--) {
            if (msgrcv(queueId, &msg, sizeof(msg.mtext), i, IPC_NOWAIT) != -1) {
                printf("Messaggio ricevuto con priorita' %ld: %s\n", msg.mtype,
                       msg.mtext);
                break;
            }
        }
    }

    return 0;
}
