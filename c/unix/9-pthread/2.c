#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

void err_exit(const char *errmsg);
typedef unsigned long long int fact_type;

void *factorial(void *arg) {
        unsigned int val = *(unsigned int *) arg;
        fact_type *ret = malloc(sizeof(fact_type));
        *ret = 1;
        for(unsigned int i = 2; i <= val; ++i) {
                *ret *= i;
        }
        pthread_exit((void *) ret);
}

void *sum(void *arg) {
        unsigned int val = *(unsigned int *) arg;
        unsigned int *ret = malloc(sizeof(unsigned int));
        *ret = 0;
        for(unsigned int i = 1; i <= val; ++i) {
                *ret += i;
        }
        pthread_exit((void *) ret);
}


int main(int argc, char *argv[]) {
        pthread_t tid1, tid2;
        unsigned int val, *sum_pointer;
        fact_type *fact_pointer;

        // loop to make sure we get a positive number
        while (1) {
                int tmp; // get an int so we can check if user gave negative number
                printf("Enter a positive number: ");
                int result = scanf("%d", &tmp);

                // clear stdin until a newline is found so stdin can be used again
                while (fgetc(stdin) != '\n') { }

                if (result != 1) { // unsuccessful conversion
                        printf("Cannot convert to number, try again\n");
                        continue;
                }

                if (tmp > 0) {
                        // we know the number is positive so tmp can be assigned to val
                        // if user gives too large number it will overflow to negative
                        // and it would have not gotten into this if clause
                        val = tmp;
                        break;
                }
                else {
                        printf("Program doesn't accept non-positive numbers, try again\n");
                }

        }


        printf("%u\n", val);
        pthread_create(&tid1, NULL, sum,       (void *)&val);
        pthread_create(&tid2, NULL, factorial, (void *)&val);

        pthread_join(tid1, (void *) &sum_pointer);
        printf("sum = %u\n", *sum_pointer);

        pthread_join(tid2, (void *) &fact_pointer);
        printf("factorial = %llu\n", *fact_pointer);

        free(sum_pointer);
        free(fact_pointer);
        return 0;
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
