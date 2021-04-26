
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define SIZE 10
int array[SIZE] = { 0 };
pthread_mutex_t raffling    = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t calculating = PTHREAD_MUTEX_INITIALIZER;

void err_exit(const char *errmsg);

void *raffle(void *arg) {
        for(int j = 0; j < 10; ++j) for(int i = 0; i < SIZE; ++i) {
                pthread_mutex_lock(&raffling);
                array[i] = rand() % 1000;
                pthread_mutex_unlock(&calculating);
        }
        pthread_exit(NULL);
}

void *calc_mean(void *arg) {
        pthread_mutex_lock(&calculating);
        int mean;
        int total;
        while(1) {
                pthread_mutex_lock(&calculating);
                mean = 0;
                total = 0;
                for(int i = 0; i < SIZE; ++i) {
                        if (array[i] == -1)
                                goto break_loop;
                        total += array[i];
                }
                printf("mean = %d\n", mean);
                pthread_mutex_lock(&raffling);
        }
break_loop:
        pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
        time_t t;
        srand((unsigned) time(&t));

        pthread_t tid1, tid2;
        pthread_mutex_lock(&calculating);

        pthread_create(&tid1, NULL, raffle, NULL);
        pthread_create(&tid2, NULL, mean,   NULL);

        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);
        return 0;
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
