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
        if (sem == SEM_FAILED)
                err_exit("sem_open: opening semaphore failed");

        if ((fd = shm_open(MEM_NAME, O_RDWR | O_CREAT, 0600)) < 0)
                err_exit("shm_open: opening shared memory failed");

        shm_unlink(MEM_NAME); // processes that havent closed shared memory can still use it

        ftruncate(fd, MSIZE);
        void *p = mmap(NULL, MSIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        if (p == MAP_FAILED)
                err_exit("mmap: ongelma alueen liittämisessä osoiteavaruuteen");

        char name[NAMELEN];
        int age, result;
        printf("Enter a name: ");
        fgets(name, NAMELEN, stdin);
        name[strcspn(name, "\n")] = '\0'; // replace newline with terminating null

        // loop to make sure we get a number
        while (1) {
                printf("Enter age: ");
                result = scanf("%d", &age); // result is number of successful conversions
                if (result == 1)
                        break;

                printf("Unable to get a number, try again\n");
                // clear stdin until a newline is found so we can use sscanf again
                while (fgetc(stdin) != '\n') { }
        }

        printf("name=%s\n", name);
        printf("age =%d\n", age);
        strcpy(henkilo.nimi, name);
        henkilo.ika = age;
        memcpy((struct henkilo *) p, &henkilo, sizeof(struct henkilo));
        sem_post(sem);

        munmap(p, MSIZE);
        sem_close(sem);
        sem_unlink(SEM_NAME); // processes that havent closed semaphore can still use it

        exit(EXIT_SUCCESS);
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
