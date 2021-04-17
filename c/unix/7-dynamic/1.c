#define _POSIX_C_SOURCE  200809L
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define LINES 3

int push(char *teksti);
int pop(char **teksti);
void free_print();
void err_exit(const char *errmsg);

struct alkio {
        char *teksti;
        struct alkio *ed;
};
struct alkio *pino = NULL; // osoittaa pinon päällimmäiseen alkioon

int main(int argc, char *argv[]) {
        char *line;
        size_t lines = 0;
        size_t linecap = 0;
        ssize_t linelen;
        while(lines < LINES) {
                line = NULL;
                // getline will malloc itself
                if ((linelen = getline(&line, &linecap, stdin)) == -1) {
                        free_print(); // memory clean
                        err_exit("getline error");
                }
                printf("%s\n", line);
                push(line);
                ++lines;
        }

        free_print();
        exit(EXIT_SUCCESS);
}

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

// voitaisiin tehdä free muuttujaan teksti tässä funktiossa,
// mutta kun alkuperäisessä esimerkissä palautettiin
// data tästä funktiosta, joten tehdään niin
// pointterin palautus funktiosta vaatii tupla pointterin käyttämistä argumenttina
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

void free_print() {
        char *teksti;
        while (pop(&teksti) != -1) {
                printf("%s", teksti);
                // free can be used on NULL items no problem
                free(teksti);
        }
}

void err_exit(const char *errmsg) {
        perror(errmsg);
        exit(EXIT_FAILURE);
}
