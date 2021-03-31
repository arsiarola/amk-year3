#define _XOPEN_SOURCE  500
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h> // read
#include <string.h>

enum options { Print = '1', Sum = '2', Overwrite = '3' };

int print_file(int fd);
int user_values_to_file(int fd, const char *fname);
int get_file_sum(int fd);
int write_sum(int fd);

int main(int argc, char *argv[]) {
        int fd;
        char *fname, *buf;
        char opt;
        // if nothing changes err variable it will be zero at the end
        // meaning success
        int err = 0;
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
                exit(EXIT_FAILURE);
        }
        else if (fd < 0 && errno == EEXIST) {
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
                         opt != Sum);
        }
        if (fd >= 0) close(fd);

        int flags = O_RDWR;
        if (opt == Overwrite) flags |= O_TRUNC;

        if ((fd = open(fname, flags)) < 0) {
                perror(NULL);
                exit(EXIT_FAILURE);
        }

        if (opt == Sum) {
                if (err = write_sum(fd))
                        goto close_file;
        }

        else if (opt == Overwrite) {
                if (err = user_values_to_file(fd, fname)) {
                        goto close_file;
                }
        }

        // Print was required at the end so this covers it
        // but with opt == Print we can also just print if we want to
        err = print_file(fd);

close_file:
        close(fd);

        if (err) {
                fprintf(stderr, "%s", strerror(err));
                exit(EXIT_FAILURE);
        }
        else {
                exit(EXIT_SUCCESS);
        }
}

int print_file(int fd) {
        off_t size = lseek(fd, 0, SEEK_END);
        char *buf = malloc(size + 1);
        if (buf == NULL) {
                fprintf(stderr,
                        "Memory allocation fail : %s",
                        strerror(errno));
                return errno;
        }
        int ret = 0;
        lseek(fd, 0, SEEK_SET);
        while (read(fd, buf, size) > 0) {
                size_t len = strlen(buf);
                if ((write(STDOUT_FILENO, buf, len)) != len) {
                        fprintf(stderr,
                                "Could not write all chars to stdout");
                        ret = errno;
                        break;
                }
        }
        free(buf);
        return ret;
}

int get_file_sum(int fd) {
        int n;
        int value;
        int sum = 0;
        char c[1];
        size_t size = 32;
        char buf[size];
        strncpy(buf, "", 1);
        while ((n = read(fd, c, 1)) > 0 && strlen(buf) < size) {
                // strcmp returns 0 if same
                if (strcmp(c, "\n") == 0) {
                        value = atoi(buf);
                        sum += value;
                        strncpy(buf, "", 1);
                }
                else {
                        strncat(buf, c, size - strlen(buf));
                }
        }
        return sum;
}

int write_sum(int fd) {
        char *buf;
        int size, sum, ret;
        size = lseek(fd, 0, SEEK_END);;

        lseek(fd, 0, SEEK_SET);
        buf = malloc(sizeof(char) * size + 1);
        if (buf == NULL) {
                fprintf(stderr, "Memory allocation fail");
                return errno;
        }
        sum = get_file_sum(fd);
        snprintf(buf, size, "sum=%d", sum);
        ret = write(fd, buf, strlen(buf)) == strlen(buf) ? 0 : -1;
        free(buf);
        return ret;
}

int user_values_to_file(int fd, const char *fname) {
        int n;
        int value;
        size_t size = 32;
        char buf[size];
        printf("Enter values separeted by newline to be added in %s file\n"
               "Ctrl-d finishes the program\n", fname);
        while ((n = read(STDIN_FILENO, buf, size)) > 0) {
                value = atoi(buf);
                printf("value=%d\n", value);
                snprintf(buf, size, "%d\n", value);
                int len = strlen(buf);
                if ((write(fd, buf, len)) != len) {
                        fprintf(stderr,
                                "Could not write all chars to file %s",
                                fname);
                        return errno;
                }
        }
        return 0;
}
