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
#define MAX_PERSONS 5

void err_exit(const char *errmsg);

int get_details(struct henkilo henkilot[MAX_PERSONS]) {
        char name[NAMELEN];
        int age, result, cnt = 0;

        while (cnt < MAX_PERSONS) {
                printf("Enter a name: ");
                if (fgets(name, NAMELEN, stdin) == NULL) {
                        printf("got null\n");
                        henkilot[cnt].nimi[0] = '\0';
                        ++cnt;
                        break;
                }
                name[strcspn(name, "\n")] = '\0'; // replace newline with terminating null
                if (name[0] == '\0') {
                        break;
                }
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

                strncpy(henkilot[cnt].nimi, name, NAMELEN);
                henkilot[cnt].ika = age;
                ++cnt;
        }
        return cnt;
}

int main(int argc, char *argv[]) {
        pid_t child;

#include "create_vars.h"
        struct henkilo henkilot[MAX_PERSONS];
        int cnt;
        struct henkilo *henkilo;
        while (1) {
                cnt = get_details(henkilot);
                for (int i = 0; i < cnt; ++i) {
                        printf("henkilot[%d].nimi: %s\n", i, henkilot[i].nimi);
                        printf("henkilot[%d].ika   %d\n", i, henkilot[i].ika);
                }
                memcpy(p, &cnt, sizeof(int));
                cnt = * (int *) p;
                memcpy(p + sizeof(int), &henkilot, sizeof(struct henkilo) * cnt);
                sem_post(sem_write);
                if (henkilot[cnt-1].nimi[0] == '\0') {
                        printf("found null name\n");
                        break;
                }
                sem_wait(sem_read);
        }

        munmap(p, MSIZE);
        sem_close(sem_read);
        sem_close(sem_write);
        shm_unlink(MEM_NAME);
        sem_unlink(SEM_WRITE_NAME);
        sem_unlink(SEM_READ_NAME);

        exit(EXIT_SUCCESS);
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
