#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <time.h>


#include "random.h"

#define OPT_MIN 0
#define OPT_MAX 1
#define OPT_AMOUNT 2

#define PRINT_OUTPUT 0
#if PRINT_OUTPUT == 1
    #define print(fmt, ...) (printf((fmt), __VA_ARGS__))
#else
    #define print(fmt, ...)
#endif

static void free_ptr_array(int **array, int size) {
    for (int i = 0; i < size; ++i) {
        if (array[i] != NULL) {
            free (array[i]);
        }
    }
}

int main(int argc, char **argv) {
    srand(time(0));
    int opt = 0;
    int ret = 0;
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
            case 'a':
            case 'b':
            case 'c':
                opts[opt - 'a'] = malloc(sizeof(int));
                *(opts[opt - 'a']) = atoi(optarg);
                break;

            default: exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 3; ++i) {
        if (opts[i] == NULL) {
            printf ("--%s option wasnt given, input the value: ", long_options[i].name);
            int value;
            while (scanf("%d", &value) != 1) {
                printf("Error in converting the value, input value for --%s again: ", long_options[i].name);
                scanf("%*s");
            }
            opts[i] = malloc(sizeof(int));
            *(opts[i]) = value;
        }
        printf("%s = %d\n",long_options[i].name, *opts[i]);
    }

    int min    = *opts[OPT_MIN];
    int max    = *opts[OPT_MAX];
    int amount = *opts[OPT_AMOUNT];

    if (max < min) {
        printf("Maximum cannot be lower than minimum, exiting\n");
        ret = 1;
    }

    if (amount <= 0) {
        ret = 1;
        printf("Amount has to be greater than 0, exiting\n");
    }

    if (ret != 0) {
        free_ptr_array(opts, 3);
        return ret;
    }

    const int size = max - min + 1;
    if (amount > size) {
        amount = size;
        printf("The amount of random unique numbers cannot be more than (max - min + 1)\n"
                "Amount changed to %d\n", amount);
    }
    int numbers[size];
    print("Array of numbers between min-max: \n");
    for (int i = min; i <= max; ++i) {
        numbers[i - min] = i;
        print("%d: %d\n", i - min, i);
    }

    shuffle(numbers, size);
    print("Array after shuffle: \n");
    for (int i = 0; i < size; ++i) {
        print("%d: %d\n", i, numbers[i]);
    }

    print("Your random unique numbers: \n");
    for (int i = 0; i < amount; ++i) {
        print("%d\n", numbers[i]);
    }

    free_ptr_array(opts, 3);
    return 0;
}
