#include <stdbool.h>
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
#include "fryzjerzy_waiting_room.h"

#define SIZE_OF_WAITING_ROOM 2
#define NUM_OF_HAIRDRESSERS 1
#define NUM_OF_CLIENTS 1
#define COST_PER_CUT 10

typedef struct {
    long client_id;
    money_t change;
} change_msg_t;

#define EMPTY 1
#define FULL 2

int main(int argc, char const *argv[]) {
    waiting_room waiting_room = init_waiting_room(SIZE_OF_WAITING_ROOM);
    cash_machine cash_machine = init_cash_machine(NUM_OF_HAIRDRESSERS);

    // create semaphor for each client
    int client_done = semget(IPC_PRIVATE, NUM_OF_CLIENTS, IPC_CREAT | 0600);
    if (client_done == -1) {
        perror("Creating semaphor for clients");
        exit(1);
    }
    for (int i = 0; i < NUM_OF_CLIENTS; i++) {
        set_down(client_done, i);
    }

    // create queue for client changes
    int change_queue = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (change_queue == -1) {
        perror("Create waiting room");
        exit(1);
    }

    // HAIRDRESSER
    int hairdresser_id = 0;
    int client_id = 0;
    if (fork() == 0) {
        srand(getpid());
        for (int i = 0; i < 100; i++) {
            usleep(rand() % 500000);
            client client = take_client(waiting_room);
            printf("%d Odebrano klienta: %d\n", i, client.money);
            money_t to_pay;
            if (rand() % 2 == 0) {
                to_pay = count_minimum_coins(client.money, COST_PER_CUT);
            } else {
                to_pay = count_maximum_coins(client.money, COST_PER_CUT);
            }
            add_cash(cash_machine, to_pay);
            int to_return = get_amount(to_pay) - COST_PER_CUT;
            usleep(rand() % 500000);
            cash_machine_change(cash_machine, to_return, hairdresser_id);
            up(client_done, client.id);
        }
        exit(0);
    }

    // CLIENT
    if (fork() == 0) {
        srand(getpid());
        for (int i = 0; i < 100; i++) {
            usleep(rand() % 500000);
            wait_for_free_seat(waiting_room);
            client client;
            client.id = client_id;
            money_t money = {2, 2, 2};
            client.money = money;
            take_seat(waiting_room, client);
            down(client_done, client.id);
            printf("Client finished\n");
        }
        exit(0);
    }
    wait(NULL);
    wait(NULL);

    return 0;
}
