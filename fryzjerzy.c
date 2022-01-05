#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "fryzjerzy_money.h"

#define SIZE_OF_WAITING_ROOM 2
#define NUM_OF_CLIENTS 1

typedef struct {
    int id;
    money_t money;
} client_t;

typedef struct {
    long mtype;
    client_t client;
} waiting_room_element_t;

#define EMPTY 1
#define FULL 2

void up(int semid, int semnum) {
    struct sembuf buf = {semnum, 1, 0};
    if (semop(semid, &buf, 1) == -1) {
        perror("Podniesienie semafora");
        exit(1);
    }
}

void down(int semid, int semnum) {
    struct sembuf buf = {semnum, -1, 0};
    if (semop(semid, &buf, 1) == -1) {
        perror("Opuszczenie semafora");
        exit(1);
    }
}

int main(int argc, char const *argv[]) {
    // create waiting room
    int waiting_room = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (waiting_room == -1) {
        perror("Create waiting room");
        exit(1);
    }
    for (int i = 0; i < SIZE_OF_WAITING_ROOM; i++) {
        waiting_room_element_t element;
        element.mtype = EMPTY;
        if (msgsnd(waiting_room, &element, sizeof(element.client), 0) == -1) {
            perror("Initialize waiting room");
            exit(1);
        }
    }

    // create semaphor for each client
    int client_done = semget(IPC_PRIVATE, NUM_OF_CLIENTS, IPC_CREAT | 0600);
    if (client_done == -1) {
        perror("Creating semaphor for clients");
        exit(1);
    }
    for (int i = 0; i < NUM_OF_CLIENTS; i++) {
        if (semctl(client_done, i, SETVAL, 0) == -1) {
            perror("Set value for clients semaphores");
            exit(1);
        }
    }

    // create cash machine
    int cash_machine_id = shmget(IPC_PRIVATE, sizeof(money_t), 0);
    if (cash_machine_id == -1) {
        perror("Creating cash machine");
        exit(1);
    }
    money_t *cash_machine = (money_t *)shmat(cash_machine_id, NULL, 0);
    if (cash_machine == NULL) {
        perror("Attaching cash machine");
        exit(1);
    }
    cash_machine->ones = 3;
    cash_machine->twos = 3;
    cash_machine->fives = 3;

    // HAIRDRESSER
    if (fork() == 0) {
        srand(getpid());
        for (int i = 0; i < 100; i++) {
            usleep(rand() % 500000);
            waiting_room_element_t element;
            if (msgrcv(waiting_room, &element, sizeof(client_t), FULL, 0) == -1) {
                perror("Take element from waiting room");
                exit(1);
            }
            printf("%d Odebrano klienta: %d\n", i, element.client.money);
            element.mtype = EMPTY;
            if (msgsnd(waiting_room, &element, sizeof(client_t), 0) == -1) {
                perror("Free seat in waiting room");
                exit(1);
            }
            usleep(rand() % 500000);
            up(client_done, element.client.id);
        }
        exit(0);
    }

    // CLIENT
    if (fork() == 0) {
        srand(getpid());
        for (int i = 0; i < 100; i++) {
            usleep(rand() % 500000);
            waiting_room_element_t element;
            if (msgrcv(waiting_room, &element, sizeof(client_t), EMPTY, 0) == -1) {
                perror("Wait for free seat in waiting room");
                exit(1);
            }
            element.mtype = FULL;
            client_t client;
            client.id = 0;
            money_t money = {2, 2, 2};
            client.money = money;
            element.client = client;
            if (msgsnd(waiting_room, &element, sizeof(client_t), 0) == -1) {
                perror("Add new client waiting room");
                exit(1);
            }
            down(client_done, client.id);
            printf("Client finished\n");
        }
        exit(0);
    }
    wait(NULL);
    wait(NULL);

    return 0;
}
