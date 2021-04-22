#ifndef SEM_H_
#define SEM_H_

#define MSIZE 4096
#define NAMELEN 80
#define SEM_WRITE_NAME "/sem_write_8-3"
#define SEM_READ_NAME  "/sem_read_8-3"
#define MEM_NAME "/mem123123"

struct henkilo {
        char nimi[NAMELEN];
        int ika;
};

#endif /* SEM_H_ */
