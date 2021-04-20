#ifndef CREATE_VARS_H_
#define CREATE_VARS_H_
sem_t *sem_read = sem_open(SEM_READ_NAME, O_CREAT, 0660, 0);
if (sem_read == SEM_FAILED) {
        err_exit("sem_open: opening read semaphore failed");
}

sem_t *sem_write = sem_open(SEM_WRITE_NAME, O_CREAT, 0660, 0);
if (sem_write == SEM_FAILED) {
        err_exit("sem_open: opening write semaphore failed");
}

if ((fd = shm_open(MEM_NAME, O_RDWR | O_CREAT, 0660)) < 0) {
        err_exit("shm_open: opening shared memory failed");
}
ftruncate(fd, MSIZE);

void *p = mmap(NULL, MSIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
if (p == MAP_FAILED) {
        err_exit("mmap: ongelma alueen liittämisessä osoiteavaruuteen");
}
#endif /* CREATE_VARS_H_ */
