#define _POSIX_C_SOURCE  200809L
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>

#include "sem.h"

void err_exit(const char *errmsg);

int main(int argc, char *argv[]) {
        pid_t child;
        int cnt;
#include "create_vars.h"
        struct henkilo *henkilo;
        while (1) {
                sem_wait(sem_write);
                cnt = *(int *) p;
                henkilo = p + sizeof(int);
                for (int i = 0; i < cnt; ++i) {
                        henkilo = (struct henkilo *) henkilo + i;
                        if (henkilo->nimi[0] == '\0') {
                                // break here would only break out of the for loop, so use goto
                                // could also use a bool variable and set it to true
                                // or make the whole while(1) a function and then use return
                                goto exit_loop;
                        }
                        printf("Writer received name: %s\n",henkilo->nimi);
                        printf("Writer received age:  %d\n", henkilo->ika);
                }
                sem_post(sem_read);
        }
exit_loop:
        printf("Empty name received, exiting program\n");

        munmap(p, MSIZE); // poista omasta osoiteavaruudesta
        sem_close(sem_read);
        sem_unlink(SEM_WRITE_NAME);
        sem_unlink(SEM_READ_NAME);
        shm_unlink(MEM_NAME);
        exit(EXIT_SUCCESS);
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
