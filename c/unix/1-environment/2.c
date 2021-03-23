#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define VAL "EI OLE ASETETTU"

int main(int argc, char *argv[], char *envp[]) {
        if (argc != 2) {
                fprintf(stderr, "Program exactly one argument, usage:\n"
                        "%s argument", argv[0]);
                exit(EXIT_FAILURE);
        }
        char *arg = getenv(argv[1]);
        if (arg != NULL) {
                printf("%s=%s\n", argv[1], arg);
                exit(EXIT_SUCCESS);
        }
        size_t len = strlen(argv[1]) + strlen(VAL) + 2; // + 2 '=' symbol and '\0'
        char var[len + 1];
        snprintf(var, len, "%s=%s", argv[1], VAL);
        putenv(var);
        // Use environ to get also environment variables created in this executable
        extern char **environ;
        char **ptr = environ;
        for ( ; *ptr != NULL; ++ptr) {
                printf("%s\n", *ptr);
        }
        exit(EXIT_SUCCESS);
}
