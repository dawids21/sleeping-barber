#include "fryzjerzy_semaphores_helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>

void up(int semid, int semnum) {
    struct sembuf buf = {semnum, 1, 0};
    if (semop(semid, &buf, 1) == -1) {
        perror("Semaphor up");
        exit(1);
    }
}

void down(int semid, int semnum) {
    struct sembuf buf = {semnum, -1, 0};
    if (semop(semid, &buf, 1) == -1) {
        perror("Semaphor up");
        exit(1);
    }
}

void set_up(int semid, int semnum) {
    if (semctl(semid, semnum, SETVAL, 1) == -1) {
        perror("Set value 1 for semaphor");
        exit(1);
    }
}

void set_down(int semid, int semnum) {
    if (semctl(semid, semnum, SETVAL, 0) == -1) {
        perror("Set value 0 for semaphor");
        exit(1);
    }
}