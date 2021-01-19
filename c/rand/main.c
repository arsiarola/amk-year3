#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>

#define OPT_MIN 0
#define OPT_MAX 1
#define OPT_AMOUNT 2

int main(int argc, char **argv) {
    int opt= 0;
    int *opts[3] = { NULL, NULL, NULL };

    static struct option long_options[] = {
        {"min"   , required_argument , 0 ,'a' } ,
        {"max"   , required_argument , 0 ,'b' } ,
        {"amount", required_argument , 0 ,'c' } ,
        {0       , 0                 , 0 ,0   }
    };

    int long_index = 0;

    while ((opt = getopt_long_only(argc, argv,"",
                    long_options, &long_index )) != -1) {
        switch (opt) {
            case 'a': *(opts[OPT_MIN]) = atoi(optarg);
                      break;
            case 'b': *(opts[OPT_MAX]) = atoi(optarg);
                      break;
            case 'c': *(opts[OPT_AMOUNT]) = atoi(optarg);
                      break;

            default: exit(EXIT_FAILURE);
        }
    }

    /* for (int i = 0; i < 3; ++i) { */
    /*     if (opts[i] == NULL) { */
    /*         printf ("--%s option must be given a value\n", long_options[i].name); */
    /*         return 1; */
    /*     } */
    /*     printf("%s = %d\n",long_options[i].name, *opts[i]); */
    /* } */

    return 0;
}
