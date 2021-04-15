#include <stdio.h> 
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

struct alkio {
        char *teksti;
        struct alkio *ed; 
};
struct alkio *pino = NULL; // osoittaa pinon päällimmäiseen alkioon
int push(char *teksti) { // vie pinon päällimmäiseksi 
        struct alkio *uusi;
        if ((uusi=(struct alkio *)malloc(sizeof(struct alkio))) == NULL) { 
                perror("push: tilanvaraus uudelle alkiolle ei onnistunut.");
                return -1;
        } 
        uusi->teksti = teksti;
        uusi->ed = pino;
        pino = uusi; // osoittaa taas päällimmäiseen alkioon
        return 0;
}

int pop(char **teksti) { // poista pinon päällimmäinen, palauta teksti parametrissa
        struct alkio *poistettava; 
        if (pino == NULL) return -1;
        *teksti = pino->teksti; 
        poistettava = pino;
        pino = pino->ed;
        free(poistettava); 
        poistettava = NULL;
        return 0;
}


#define LINES 3
int main(int argc, char *argv[]) {
        char *line;
        size_t lines = 0;
        size_t linecap = 0;
        ssize_t linelen;
        while(lines < LINES) {
                line = NULL;
                // getline will malloc itself
                if ((linelen = getline(&line, &linecap, stdin)) == -1)
                        ;
                /* err_exit("getline error"); // remember memory clean */
                printf("%s\n", line);
                push(line);
                ++lines;
        }
        struct alkio *nykyinen = pino;
        /* while (nykyinen != NULL) { */
        /*         printf("%s", nykyinen->teksti); */
        /*         nykyinen = nykyinen->ed; */
        /* } */

        char *teksti;
        while (pop(&teksti) != -1) {
                printf("%s", teksti);
                free(teksti);
        }
        exit(EXIT_SUCCESS);
}
