#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <string.h>

#define SIZE 9
float squares[SIZE];

void err_exit(const char *errmsg);

void *square_add(void *arg) {
        sleep(1);
        int i = (int) arg;
        squares[i] = sqrt(i);
        pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
        // ainut tapa miten keksin että toimisi 100% ajasta oli memcpy
        // avulla. Toimii vaikka threadissa on sleeppi ennen arg hakemista
        // Joka ei toimisi jos välitettäisiin i:n osoite (void *)&i

        pthread_t tid[SIZE];
        for(int i = 0; i < SIZE; ++i) {
                /* memcpy(&squares[i], &i, sizeof(int)); */
                /* pthread_create(&tid[i], NULL, square_add, (void *)&squares[i]); */
                pthread_create(&tid[i], NULL, square_add, (void *)i);
        }

        for(int i = 0; i < SIZE; i++) {
                pthread_join(tid[i], NULL);
                printf("√%d = %f\n", i, squares[i]);
        }

        return 0;
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
