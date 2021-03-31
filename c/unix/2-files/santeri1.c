#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



int main(int argc, char *argv[]) {

    char *buf;
    off_t size;
    int file;

     if ((file = open(argv[1], O_RDONLY)) < 0) {
         perror("Tiedoston avaaminen ei onnistununt.");
         exit(EXIT_FAILURE);
     }

    size = lseek(file, 0, SEEK_END);

    if ((buf = (char *)malloc(size)) == NULL) {
        perror("Malloc ei onnistunut.");
        exit(EXIT_FAILURE);
    }

    for(int i=2;i<=size;i++){
        lseek(file, 0, SEEK_SET);
        pread(file,buf,1,size-i);
        write(STDOUT_FILENO,buf,size);
        //en tajua miksi mutta jos tota iffiä ei ole se tupla tulostaa viimeisen kirjaimen kahdesti ja ilman sitä se ei tulosta sitä ollenkaan
    }

    free(buf);
    close(file);
    exit(EXIT_SUCCESS);
    return 1;
}
