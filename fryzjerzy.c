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

#include "fryzjerzy_cash_machine.h"
#include "fryzjerzy_client.h"
#include "fryzjerzy_money.h"
#include "fryzjerzy_semaphores_helpers.h"

#define SIZE_OF_WAITING_ROOM 2
#define NUM_OF_HAIRDRESSERS 1
#define NUM_OF_CLIENTS 1
#define COST_PER_CUT 10

typedef struct {
    long mtype;
    client client;
} waiting_room_element_t;

typedef struct {
    long client_id;
    money_t change;
} change_msg_t;

#define EMPTY 1
#define FULL 2

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
        set_down(client_done, i);
    }

    cash_machine cash_machine = init();

        exit(1);
    }
    }

    // create queue for client changes
    int change_queue = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (change_queue == -1) {
        perror("Create waiting room");
        exit(1);
    }

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
            if (msgrcv(waiting_room, &element, sizeof(client), EMPTY, 0) == -1) {
                perror("Wait for free seat in waiting room");
                exit(1);
            }
            element.mtype = FULL;
            client client;
            client.id = 0;
            money_t money = {2, 2, 2};
            client.money = money;
            element.client = client;
            if (msgsnd(waiting_room, &element, sizeof(client), 0) == -1) {
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
