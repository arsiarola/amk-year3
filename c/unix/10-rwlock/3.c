#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

// from man pthread_cancel
#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define MILLION 1000000
static int x = 0;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;

// As a test here are two functions created with a macro but with different increment amounts,
// since the functions were same expect for decrement/increment.
//  -1 and 1 can be passed to get -- and ++ effect
// Works as an experiment, the traditional way of defining functions are also at the bottom of the file

// Also now just realised could have just passed the amount by the void *arg
// function argument when creating the thread...
// But cool to know that this was also possible

#define CREATE_FUN(fun_name, amount) \
        void *fun_name(void *arg) { \
                int s  = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL); \
                if (s != 0) \
                handle_error_en(s, "pthread_setcancelstate"); \
                while(1) { \
                        usleep(rand() % MILLION); \
                        pthread_mutex_lock(&mutex); \
                        x += amount; \
                        pthread_cond_signal(&cond); \
                        pthread_mutex_unlock(&mutex); \
                } \
                pthread_exit(NULL); \
        }

CREATE_FUN(increment,  1)
CREATE_FUN(decrement, -1)

void *reset(void *arg) {
        for (int i = 0; i < 3; ++i) {
                pthread_mutex_lock(&mutex);
                while (-2 <= x && x <= 2) {
                        // printf("x = %d\n", x);
                        pthread_cond_wait(&cond, &mutex);
                }
                printf("x = %d, over the limit\n", x);
                x = 0;
                pthread_mutex_unlock(&mutex);
        }
        pthread_exit(NULL);

}

int main(int argc, char *argv[]) {
        pthread_t inc_thr, dec_thr, reset_thr;
        srand(time(NULL));

        pthread_create(&reset_thr, NULL, reset, NULL);
        pthread_create(&inc_thr, NULL, increment, NULL);
        pthread_create(&dec_thr, NULL, decrement, NULL);

        pthread_join(reset_thr, NULL);

        pthread_cancel(inc_thr);
        pthread_cancel(dec_thr);
        return 0;
}

/*

void increment() {
        int s  = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        if (s != 0)
                handle_error_en(s, "pthread_setcancelstate");
        while(1) {
                usleep(rand() % MILLION);
                pthread_mutex_lock(&mutex);
                ++x;
                pthread_cond_signal(&cond);
                pthread_mutex_unlock(&mutex);
        }
        pthread_exit(NULL);
}

void decrement() {
        int s  = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        if (s != 0)
                handle_error_en(s, "pthread_setcancelstate");
        while(1) {
                usleep(rand() % MILLION);
                pthread_mutex_lock(&mutex);
                --x;
                pthread_cond_signal(&cond);
                pthread_mutex_unlock(&mutex);
        }
        pthread_exit(NULL);
}

*/
