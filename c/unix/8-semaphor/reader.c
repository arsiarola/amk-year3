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
        int age, result, cnt;
        char name[NAMELEN];
        cnt = 0;

        while (cnt < MAX_PERSONS) {
                printf("Enter a name: ");
                fgets(name, NAMELEN, stdin);
                name[strcspn(name, "\n")] = '\0'; // replace newline with terminating null
                strncpy(henkilot[cnt].nimi, name, NAMELEN);
                ++cnt;
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
                henkilot[cnt].ika = age;
        }
        return cnt;
}

int main(int argc, char *argv[]) {
        pid_t child;

#include "create_vars.h"
        struct henkilo henkilot[MAX_PERSONS];
        while (1) {
                int cnt = get_details(henkilot);
                printf("henkilot[0].nimi=%s\n", henkilot[0].nimi);
                memcpy((int *)p, &cnt, sizeof(int));
                memcpy((struct henkilo *)((int *)p + 1), henkilot, sizeof(struct henkilo) * cnt);
                for (int i = 0; i < cnt; ++i) {
                        struct henkilo henkilo = *(((struct henkilo *) ((int *)p + 1)) + cnt);
                        printf("Writer received name: %s\n",henkilo.nimi);
                        printf("Writer received age:  %d\n", henkilo.ika);
                }
                sem_post(sem_write);
                sem_wait(sem_read);
        }

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
