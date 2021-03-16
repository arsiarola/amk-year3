#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[], char *envp[]) {
        if (argc != 2) {
                fprintf(stderr, "Program takes one argument, usage:\n"
                                "%s argument", argv[0]);
                exit(EXIT_FAILURE);
        }
        char arg = getenv(argv[1]);
        if (arg != NULL) {
                printf("%s\n", arg);
        }
        else {
                printf("%s=EI OLE ASETETTU\n", argv[1]);
                printf("Printing all environment variables:\n");
                for (int i = 0; envp[i] != NULL; ++i) {
                        printf("%s\n", envp[i]);
                }
        }

        exit(EXIT_SUCCESS);
}
