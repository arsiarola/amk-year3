#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
extern char *optarg;
extern int optind, opterr,optopt;
int
main(int argc, char *argv[]) {
        int opt;
        // Reorder arguments, executable name will be first
        // Actual argument will be last, everything else in between
        while ((opt = getopt(argc, argv, "a:s:m:d:")) > 0) {
                switch (opt) {
                case '?':
                        fprintf(stderr, "Expected argument after options\n");
                        exit(EXIT_FAILURE);
                }
        }
		
        int result = atoi(argv[optind]);
        optind = 1; // reset index back to 1 so we can loop again
        while ((opt = getopt(argc, argv, "a:s:m:d:")) != -1) {
                int value = atoi(optarg);
                switch (opt) {
                case 'a':
                        result += value;
                        break;
                case 's':
                        result -= value;
                        break;
                case 'm':
                        result *= value;
                        break;
                case 'd':
                        if (value == 0) {
                                printf("Not allowed to divide by zero\n");
                                exit(EXIT_FAILURE);
                        }
                        result /= value;
                        break;
                default: /* '?' */ // Shouldn
                        fprintf(stderr,
                                "Usage: %s result [-a n] [-s n] [-m n] [-d n]\n",
                                argv[0]);
                        exit(EXIT_FAILURE);
                }
        }
        printf("%d\n", result);
        exit(EXIT_SUCCESS);
}
