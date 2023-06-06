#include <bits/types/sigevent_t.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void *my_fun(void *param) {
    printf("This is a thread that received %d\n", *(int *)param);
    int i = *(int *)param * 2; // Local variable ceases to exist!
    char *str = "Returned string";
    return (void *)str;
}
int main(void) {
    pthread_attr_t attr;
    pthread_t t_id;
    int arg = 10;
    char *retval;
    pthread_create(&t_id, NULL, my_fun, (void *)&arg);
    printf("Executed thread with id %ld\n", t_id);
    sleep(3);
    pthread_join(t_id, (void **)&retval); // A pointer to a void pointer
    printf("retval=%s\n", retval);
}
