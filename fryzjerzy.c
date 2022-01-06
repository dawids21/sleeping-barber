#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

#include "fryzjerzy_cash_machine.h"
#include "fryzjerzy_client.h"
#include "fryzjerzy_logger.h"
#include "fryzjerzy_money.h"
#include "fryzjerzy_semaphores_helpers.h"
#include "fryzjerzy_waiting_room.h"

#define SIZE_OF_WAITING_ROOM 1
#define NUM_OF_HAIRDRESSERS 1
#define NUM_OF_CLIENTS 3
#define NUM_OF_CHAIRS 1
#define COST_PER_CUT 30

#define EMPTY 1
#define FULL 2

int init_chairs(int num_of_chairs) {
    int chairs = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if (chairs == -1) {
        perror("Creating chairs");
        exit(1);
    }
    if (semctl(chairs, 0, SETVAL, num_of_chairs) == -1) {
        perror("Creating chairs");
        exit(1);
    }
    return chairs;
}

int main(int argc, char const *argv[]) {
    waiting_room waiting_room = init_waiting_room(SIZE_OF_WAITING_ROOM);
    cash_machine cash_machine = init_cash_machine(NUM_OF_HAIRDRESSERS);
    int change_queue = new_change_queue();
    int chairs = init_chairs(NUM_OF_CHAIRS);

    // HAIRDRESSER
    d_log_num("start main", getpid());
    for (int hairdresser_id = 0; hairdresser_id < NUM_OF_HAIRDRESSERS; hairdresser_id++) {
        if (fork() == 0) {
            srand(getpid());
            d_log_num("start hairdresser", getpid());
            for (int i = 0; i < 100; i++) {
                usleep(rand() % 500000);
                client client = take_client(waiting_room);
                d_log_num("get client", client.id);
                money to_pay;
                if (rand() % 2 == 0) {
                    to_pay = count_minimum_coins(client.money, COST_PER_CUT);
                } else {
                    to_pay = count_maximum_coins(client.money, COST_PER_CUT);
                }
                add_cash(cash_machine, to_pay);
                d_log_num("get money in machine for", client.id);
                int to_return = get_amount(to_pay) - COST_PER_CUT;
                down(chairs, 0);
                usleep(rand() % 500000);
                up(chairs, 0);
                d_log_num("finished client", client.id);
                money change = cash_machine_change(cash_machine, to_return, hairdresser_id);
                d_log_num("get change for", client.id);
                money to_send = subtract(change, to_pay);
                d_log_money("send change", to_send);
                send_change(client, to_send);
                d_log_num("add change to queue for client", client.id);
                d_log("hairdresser finished");
            }
            exit(0);
        }
    }

    // CLIENT
    for (int client_id = 1; client_id <= NUM_OF_CLIENTS; client_id++) {
        if (fork() == 0) {
            srand(getpid());
            client client = new_client(client_id, change_queue);
            d_log_num("start client", getpid());
            for (int i = 0; i < 100; i++) {
                while (get_amount(client.money) < COST_PER_CUT) {
                    make_money(client);
                }
                d_log_num("wait for free seat", client.id);
                while (!wait_for_free_seat(waiting_room)) {
                    make_money(client);
                }
                d_log_num("get free seat", client.id);
                take_seat(waiting_room, client);
                d_log_num("take seat", client.id);
                d_log_num("wait for change", client.id);
                wait_for_change(client);
                d_log_num("get change", client.id);
                d_log("client finished");
            }
            exit(0);
        }
    }

    for (int i = 0; i < NUM_OF_HAIRDRESSERS + NUM_OF_CLIENTS; i++) {
        wait(NULL);
    }

    return 0;
}
