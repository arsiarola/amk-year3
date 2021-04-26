#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

#define SIZE 90
int squares[SIZE];

void err_exit(const char *errmsg);
void *square_add(void *arg) {
        int i = *(int *) arg;
        int val = i * i;
        squares[i] = val;
        pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
        pthread_t tid[SIZE];

        for(int i = 0; i < SIZE; i++) {
                squares[i] = i;
                pthread_create(&tid[i], NULL, square_add, (void *)&squares[i]);
        }

        for(int i = 0; i < SIZE; i++) {
                pthread_join(tid[i], NULL);
                printf("%d^2 = %d\n", i, squares[i]);
        }
        return 0;
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
