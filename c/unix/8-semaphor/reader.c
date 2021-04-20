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

#include "create_vars.h"
        char name[NAMELEN];
        int age, result;
        while (1) {
                printf("Enter a name: ");
                fgets(name, NAMELEN, stdin);
                name[strcspn(name, "\n")] = '\0'; // replace newline with terminating null
                strcpy(henkilo.nimi, name);
                if (name[0] == '\0')
                        break;

                // loop to make sure we get a number
                while (1) {
                        printf("Enter age: ");
                        result = scanf("%d", &age); // result is number of successful conversions
                        // clear stdin until a newline is found so stdin can be used again
                        while (fgetc(stdin) != '\n') { }
                        if (result == 1)
                                break;

                        printf("Unable to get a number, try again\n");
                }
                henkilo.ika = age;
                memcpy((struct henkilo *) p, &henkilo, sizeof(struct henkilo));
                sem_post(sem_write);
                sem_wait(sem_read);
        }
        memcpy((struct henkilo *) p, &henkilo, sizeof(struct henkilo));
        sem_post(sem_write);

        munmap(p, MSIZE);
        sem_close(sem_read);
        sem_close(sem_write);
        sem_unlink(SEM_WRITE_NAME);
        sem_unlink(SEM_READ_NAME);

        exit(EXIT_SUCCESS);
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
