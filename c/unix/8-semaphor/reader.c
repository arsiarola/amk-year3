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
        int fd;
        struct henkilo henkilo;

        sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0600, 1);
        if (sem == SEM_FAILED) {
                err_exit("sem_open: opening semaphore failed");
        }
        sem_unlink(SEM_NAME); // processes that havent closed semaphore can still use it
        sem_wait(sem);

        if ((fd = shm_open(MEM_NAME, O_RDWR | O_CREAT, 0600)) < 0) {
                err_exit("shm_open: opening shared memory failed");
        }

        ftruncate(fd, MSIZE);
        void *p = mmap(NULL, MSIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

        if (p == MAP_FAILED) {
                err_exit("mmap: ongelma alueen liittämisessä osoiteavaruuteen");
        }

        sem_wait(sem);
        printf("CHILD received name: %s\n"
                        "CHILD received age:  %d\n",
                        ((struct henkilo *)p)->nimi, ((struct henkilo *)p)->ika);

        munmap(p, MSIZE); // poista omasta osoiteavaruudesta
        sem_close(sem);
        shm_unlink(MEM_NAME); // processes that havent closed shared memory can still use it
        sleep(5);
        exit(EXIT_SUCCESS);
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
