
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define SIZE 5
int array[SIZE] = { 0 };

pthread_mutex_t raffling    = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t calculating = PTHREAD_MUTEX_INITIALIZER;

void err_exit(const char *errmsg);

void *raffle(void *arg) {
        for(int j = 0; j < 10; ++j) {
                pthread_mutex_lock(&raffling);
                for(int i = 0; i < SIZE; ++i) {
                        array[i] = rand() % 1000;
                }
                pthread_mutex_unlock(&calculating);
        }

        // end mark for array
        for(int i = 0; i < SIZE; ++i) {
                array[i] = -1;
        }
        pthread_exit(NULL);
}

void *calc_mean(void *arg) {
        int total;
        while(1) {
                pthread_mutex_lock(&calculating);
                total = 0;
                for(int i = 0; i < SIZE; ++i) {
                        // numbers were supposed to be 0-999, so even if
                        // one of them is negative, end the thread
                        if (array[i] == -1)
                                goto break_loop;

                        total += array[i];
                }
                printf("mean = %d\n", total / SIZE);
                pthread_mutex_unlock(&raffling);
        }
break_loop:

        pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
        pthread_t tid1, tid2;
        pthread_mutex_lock(&calculating); // calculate thread has to wait for numbers first

        srand(time(NULL));

        pthread_create(&tid1, NULL, raffle,    NULL);
        pthread_create(&tid2, NULL, calc_mean, NULL);

        pthread_join(tid1, NULL);
        pthread_join(tid2, NULL);
        return 0;
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
