#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/types.h>

#define MAX_LEN 32
#define SILENT 1
#define VERBOSE 0

pid_t target_pid;

struct msg_buffer {
    long mtype;
    char mtext[MAX_LEN + 1];
};

void usage(char *prog) {
    printf("Usage: %s <name> <action> [<value>] <pid>", prog);
    exit(EXIT_FAILURE);
}

void check_args(int argc, char **argv) {
    if (argc <= 2)
        usage(argv[0]);

    char *action = argv[2];

    int isPut = strcmp(action, "put") == 0;
    int isMov = strcmp(action, "mov") == 0;
    int need5args = isPut || isMov;

    if (argc == 4 && !need5args)
        target_pid = atoi(argv[3]);
    else if (argc == 5 && need5args)
        target_pid = atoi(argv[4]);
    else
        usage(argv[0]);

    if (target_pid <= 0)
        usage(argv[0]);
}

int signal_process(int resExt) {
    int res = -1;
    if (resExt != -1)
        res = kill(target_pid, SIGUSR1);
    else
        kill(target_pid, SIGUSR2);
    exit(res);
}

int action_new(char *name, int silent) {
    creat(name, 0777);
    key_t queueKey = ftok(name, 1);
    int queueId = msgget(queueKey, 0777);
    if (queueId == -1) {
        queueId = msgget(queueKey, 0777 | IPC_CREAT);
    } else if (!silent) {
        printf("%d\n", queueId);
    }
    if (queueId == -1)
        signal_process(-1);
    return queueId;
}

int action_put(int queueId, char *value) {
    struct msg_buffer msg;
    msg.mtype = 1;
    strncpy(msg.mtext, value, MAX_LEN);
    return msgsnd(queueId, &msg, sizeof(msg.mtext), 0);
}

int action_get(int queueId, char *msgExt) {
    struct msg_buffer msg;
    int res = msgrcv(queueId, &msg, sizeof(msg.mtext), 0, IPC_NOWAIT);
    if (res != -1) {
        printf("%s\n", msg.mtext);
        if (msgExt != NULL)
            strcpy(msgExt, msg.mtext);
    }
    return res;
}

int action_del(char *name) {
    key_t queueKey = ftok(name, 1);
    if (queueKey == -1)
        return -1;
    int queueId = msgget(queueKey, 0777);
    if (queueId == -1) {
        return -1;
    }
    return msgctl(queueId, IPC_RMID, NULL);
}

int action_emp(int queueId) {
    int count = 0;
    while (action_get(queueId, NULL) != -1)
        count++;
    return count == 0 ? -1 : 0;
}

int action_mov(char *srcName, int queueIdDst) {
    key_t queueKeySrc = ftok(srcName, 1);
    if (queueKeySrc == -1)
        return -1;
    int queueIdSrc = msgget(queueKeySrc, 0777);
    if (queueIdSrc == -1) {
        return -1;
    }
    char text[MAX_LEN + 1];
    int moved = 0;
    int res = 0;
    while (action_get(queueIdSrc, text) != -1) {
        res |= action_put(queueIdDst, text);
        moved++;
    }
    printf("%d\n", moved);
    res |= msgctl(queueIdSrc, IPC_RMID, NULL);
    return res;
}

int main(int argc, char **argv) {
    check_args(argc, argv);

    char *name = argv[1];
    char *action = argv[2];

    int res;
    if (strcmp(action, "new") == 0) {
        res = action_new(name, VERBOSE);
    } else if (strcmp(action, "put") == 0) {
        int queueId = action_new(name, SILENT);
        res = action_put(queueId, argv[3]);
    } else if (strcmp(action, "get") == 0) {
        int queueId = action_new(name, SILENT);
        res = action_get(queueId, NULL);
    } else if (strcmp(action, "del") == 0) {
        res = action_del(name);
    } else if (strcmp(action, "emp") == 0) {
        int queueId = action_new(name, SILENT);
        res = action_emp(queueId);
    } else if (strcmp(action, "mov") == 0) {
        char *srcName = name;
        char *dstName = argv[3];
        int queueIdDst = action_new(dstName, SILENT);
        res = action_mov(srcName, queueIdDst);
    } else {
        fprintf(stderr, "Invalid action: %s", action);
        usage(argv[0]);
    }

    signal_process(res);
}
