munmap(p, MSIZE);
shm_unlink(MEM_NAME);

sem_close(sem_read);
sem_close(sem_write);
sem_unlink(SEM_WRITE_NAME);
sem_unlink(SEM_READ_NAME);


