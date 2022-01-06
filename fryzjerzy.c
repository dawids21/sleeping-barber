#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
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
            money_t change = cash_machine_change(cash_machine, to_return, hairdresser_id);
            change_msg_t change_msg;
            change_msg.client_id = client.id;
            change_msg.change = change;
            if (msgsnd(change_queue, &change_msg, sizeof(change_msg.change), 0) == -1) {
                perror("Send client change");
                exit(1);
            }
        }
        exit(0);
    }

    // CLIENT
    if (fork() == 0) {
        srand(getpid());
        client client;
        client.id = client_id;
        money_t money = {2, 2, 2};
        client.money = money;
        for (int i = 0; i < 100; i++) {
            usleep(rand() % 500000);
            wait_for_free_seat(waiting_room);
            take_seat(waiting_room, client);
            change_msg_t change_msg;
            if (msgrcv(change_queue, &change_msg, sizeof(change_msg.change), client_id, 0) == -1) {
                perror("Get change for client");
                exit(1);
            }
            client.money.ones += change_msg.change.ones;
            client.money.twos += change_msg.change.twos;
            client.money.fives += change_msg.change.fives;
            printf("Client finished\n");
        }
        exit(0);
    }
    wait(NULL);
    wait(NULL);

    return 0;
}
