#include "stdio.h"
#include "stdlib.h"

int main(int argc, char *argv[]) {
    printf("Ohjelman nimi on %s, argc = %d\n", argv[0], argc);
    if (argc <= 1) {
        printf("Et antanut komentorivillä yhtään argumenttia\n");
        exit (EXIT_FAILURE);
    }
    else {
        printf("Annoit ohjelman nimen perään %d komentoriviargumenttia\n", argc - 1);
        for (int i = argc - 1; i >= 0; --i) {
            printf("argv[%d] = %s\n", i, argv[i]);
        }
    }
    exit (EXIT_SUCCESS);
}
