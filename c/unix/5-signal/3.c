#define _POSIX_C_SOURCE  200809L
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

#define CHILD_COUNT 3

void sig_handler(int sig);
void err_exit(const char *errmsg);
void child_work(int num);

// Reduce  this variable when recieves SIGCHLD
// Better way than static variable?
static int count = CHILD_COUNT;

int main(int argc, char *argv[]) {
        struct sigaction act;
        int fd[2];

        act.sa_handler = sig_handler;
        act.sa_flags = 0;
        sigemptyset(&act.sa_mask); // yhtään ei estetä, kaikki signaalit kelpuutetaan
        sigaction(SIGCHLD,  &act, NULL);

        for (int i = 0; i < CHILD_COUNT; ++i) {
                pid_t c;
                if ((c = fork()) == 0) {
                        child_work(i+1);
                        exit(EXIT_SUCCESS);
                }
                else if (c < 0){
                        err_exit("child1 fork failed");
                }

        }

        for (int i = 0 ; count > 0; ++i) {
                printf("main: %d\n", i);
                sleep(1);
        }
        return 0;
}

void child_work(int num) {
        sleep(num);
        printf("%s, %ds\n",        num == 1 ?
                        "Eka"    : num == 2 ?
                        "Toka"   : num == 3 ?
                        "Kolmas" : "",
                        num);
        exit(EXIT_SUCCESS);
}

void sig_handler(int sig) {
        --count;
        char msg[128];
        snprintf(msg, 128, "SIGCHLD vastaanotettu\n");
        write(STDOUT_FILENO, msg, strlen(msg));
}


void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
