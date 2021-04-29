#define _POSIX_C_SOURCE  200809L
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#define SIZE 10000
int array[SIZE] = { 0 };

pthread_mutex_t  first_raffle_done = PTHREAD_MUTEX_INITIALIZER;
pthread_rwlock_t rwlock            = PTHREAD_RWLOCK_INITIALIZER;

void err_exit(const char *errmsg);

void *raffle(void *arg) {
        for(int j = 0; j < 2; ++j) {
                pthread_rwlock_wrlock(&rwlock);
                for(int i = 0; i < SIZE; ++i) {
                        array[i] = rand() % 1000;
                }
                printf("\n");
                pthread_rwlock_unlock(&rwlock);
                pthread_mutex_unlock(&first_raffle_done); // starts the reader threads creation
                sleep(2);
        }

        // end mark for array
        pthread_rwlock_wrlock(&rwlock);
        for(int i = 0; i < SIZE; ++i) {
                array[i] = -1;
        }
        pthread_rwlock_unlock(&rwlock);
        pthread_exit(NULL);
}

void *calc_mean(void *arg) {
        int total;
        float mean, prev_mean = -1; // prev value as -1 at start so it will be different first time
        while (1) {
                pthread_rwlock_rdlock(&rwlock);
                if (array[0] == -1) {
                        pthread_rwlock_unlock(&rwlock);
                        break;
                }

                total = 0;
                for (int i = 0; i < SIZE; ++i) {
                        total += array[i];
                }

                mean = total / (float)SIZE;
                if (mean != prev_mean) {
                        printf("mean = %f\n", mean);
                        prev_mean = mean;
                }

                pthread_rwlock_unlock(&rwlock);
                sleep(1);
        }

        pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
        pthread_t writer, readers[5];
        srand(time(NULL));

        pthread_create(&writer, NULL, raffle,    NULL);

        // reader threads need to wait for the first numbers to be initalised
        pthread_mutex_lock(&first_raffle_done);

        for (int i = 0; i < 5; ++i) {
                pthread_create(&readers[i], NULL, calc_mean, NULL);
        }

        pthread_join(writer, NULL);
        for (int i = 0; i < 5; ++i) {
                pthread_join(readers[i], NULL);
        }
        return 0;
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
