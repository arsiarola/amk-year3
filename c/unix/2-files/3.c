#define _XOPEN_SOURCE  500
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h> // read
#include <string.h>

#define BUFSIZE 512
char buf[BUFSIZE];

enum options { Print = '1', Sum = '2', Overwrite = '3' };

int print_file(int fd);
int user_values_to_file(int fd, const char *fname);
int get_file_sum(int fd);

int main(int argc, char *argv[]) {
        int fd;
        char *fname;
        if (argc != 2) {
                fprintf(stderr,
                        "Program takes one argument, output filename\n"
                        "Usage: %s [FILE]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
        fname = argv[1];

        // First opening is just for checking if the file exists
        fd = open(fname, O_CREAT | O_EXCL | O_RDWR);
        if (fd < 0 && errno != EEXIST) {
                perror("Error in opening file");
                exit(EXIT_FAILURE);
        }
        char opt;
        if (fd < 0 && errno == EEXIST) {
                fprintf(stderr,
                        "File %s exists, Choose option by entering "
                        "corresponding number and pressing enter:\n"
                        "%c. Print the file\n"
                        "%c. Calculate the sum of existing numbers at the end of the file\n"
                        "%c. Overwrite existing file\n",
                        fname, Print, Sum, Overwrite);
                do {
                        opt = getchar();
                } while (opt != Print &&
                         opt != Overwrite &&
                         opt != Sum
                        );
        }
        if (fd >= 0) close(fd);

        int flags = O_RDWR;
        if (opt == Overwrite) flags |= O_TRUNC;
        if ((fd = open(fname, flags)) < 0) {
                perror("Error in open file");
                exit(EXIT_FAILURE);
        }

        if (opt == Sum) {
                int sum = get_file_sum(fd);
                lseek(fd, 0, SEEK_END);
                snprintf(buf, BUFSIZE, "sum=%d", sum);
                if ((write(fd, buf, strlen(buf))) != strlen(buf)) {
                        perror("Could not write all chars to file");
                        close(fd);
                        exit(EXIT_FAILURE);
                }
        }

        else if (opt == Overwrite) {
                if (user_values_to_file(fd, fname) != 0) {
                        close(fd);
                        exit(EXIT_FAILURE);
                }
        }

        // Print was required at the end so this covers if (opt == Print)
        print_file(fd);
        close(fd);
        exit(EXIT_SUCCESS);
}

int print_file(int fd) {
        int n;
        lseek(fd, 0, SEEK_SET);
        while ((n = read(fd, buf, BUFSIZE)) > 0) {
                int len = strlen(buf);
                if ((write(STDOUT_FILENO, buf, len)) != len) {
                        perror("Could not write all chars to stdout");
                        return errno;
                }
        }
        return 0;
}

int get_file_sum(int fd) {
        int n;
        int value;
        int sum = 0;
        char c[1];
        strncpy(buf, "", 1);
        while ((n = read(fd, c, 1)) > 0 && strlen(buf) < BUFSIZE) {
                // strcmp returns 0 if same
                if (strcmp(c, "\n") == 0) {
                        value = atoi(buf);
                        sum += value;
                        strncpy(buf, "", 1);
                }
                else {
                        strncat(buf, c, BUFSIZE - strlen(buf));
                }
        }
        return sum;
}

int user_values_to_file(int fd, const char *fname) {
        int n;
        int value;
        printf("Enter values separeted by newline to be added in %s file\n"
               "Ctrl-d finishes the program\n", fname);
        while ((n = read(STDIN_FILENO, buf, BUFSIZE)) > 0) {
                value = atoi(buf);
                printf("value=%d\n", value);
                snprintf(buf, BUFSIZE, "%d\n", value);
                int len = strlen(buf);
                if ((write(fd, buf, len)) != len) {
                        perror("Could not write all chars to file");
                        return errno;
                }
        }
        return 0;
}
