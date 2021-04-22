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
#define MAX_PERSONS (MSIZE / sizeof(struct henkilo) - sizeof(int))

int get_details(struct henkilo henkilot[MAX_PERSONS]);
void err_exit(const char *errmsg);

int main(int argc, char *argv[]) {
        pid_t child;
        struct henkilo henkilot[MAX_PERSONS];
        int cnt;
#include "create_vars.h"

        printf("memory  size    = %d\n", MSIZE);
        printf("henkilo size    = %d\n", sizeof(struct henkilo));
        printf("int     size    = %d\n", sizeof(int));
        printf("maximum persons = %d\n", MAX_PERSONS);

        while (1) {
                cnt = get_details(henkilot);

                // if user tries to send empty list, dont send it
                if (cnt < 1) continue;

                memcpy(p, &cnt, sizeof(int));
                memcpy(p + sizeof(int), &henkilot, sizeof(struct henkilo) * cnt);
                sem_post(sem_write);

                // if last element is empty name that is the indication that user wants to stop
                if (henkilot[cnt-1].nimi[0] == '\0') {
                        printf("found null name\n");
                        break;
                }

                sem_wait(sem_read);
        }

        printf("Exiting program\n");

#include "close_vars.h"

        exit(EXIT_SUCCESS);
}

int get_details(struct henkilo henkilot[MAX_PERSONS]) {
        char name[NAMELEN];
        int age, result, cnt = 0;

        printf("Send current name list by giving empty name (empty list won't be sent)\n");
        printf("Stop the prgram with Ctrl-D\n");
        while (cnt < MAX_PERSONS) {
                printf("Enter a name : ");

                // if user presses Ctrl-D put last element as empty name
                if (fgets(name, NAMELEN, stdin) == NULL) {
                        henkilot[cnt].nimi[0] = '\0';
                        ++cnt;
                        break;
                }

                name[strcspn(name, "\n")] = '\0'; // replace newline with terminating null

                // User gave empty string so dont append it to the list but just break the loop
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

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
