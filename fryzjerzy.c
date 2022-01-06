#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

#include "fryzjerzy_cash_machine.h"
#include "fryzjerzy_client.h"
#include "fryzjerzy_logger.h"
#include "fryzjerzy_money.h"
#include "fryzjerzy_semaphores_helpers.h"
#include "fryzjerzy_waiting_room.h"

#define SIZE_OF_WAITING_ROOM 2
#define NUM_OF_HAIRDRESSERS 1
#define NUM_OF_CLIENTS 1
#define COST_PER_CUT 10

typedef struct {
    long client_id;
    money change;
} change_msg_t;

#define EMPTY 1
#define FULL 2

int main(int argc, char const *argv[]) {
    waiting_room waiting_room = init_waiting_room(SIZE_OF_WAITING_ROOM);
    cash_machine cash_machine = init_cash_machine(NUM_OF_HAIRDRESSERS);
    int change_queue = new_change_queue();

    // HAIRDRESSER
    log_num("start main", getpid());
    for (int hairdresser_id = 0; hairdresser_id < NUM_OF_HAIRDRESSERS; hairdresser_id++) {
        if (fork() == 0) {
            srand(getpid());
            log_num("start hairdresser", getpid());
            for (int i = 0; i < 100; i++) {
                usleep(rand() % 500000);
                client client = take_client(waiting_room);
                log_num("get client", client.id);
                money to_pay;
                if (rand() % 2 == 0) {
                    to_pay = count_minimum_coins(client.money, COST_PER_CUT);
                } else {
                    to_pay = count_maximum_coins(client.money, COST_PER_CUT);
                }
                add_cash(cash_machine, to_pay);
                log_num("get money in machine for", client.id);
                int to_return = get_amount(to_pay) - COST_PER_CUT;
                usleep(rand() % 500000);
                log_num("finished client", client.id);
                money change = cash_machine_change(cash_machine, to_return, hairdresser_id);
                log_num("get change for", client.id);
                money to_send = subtract(change, to_pay);
                log_money("send change", to_send);
                change_msg_t change_msg;
                change_msg.client_id = client.id;
                change_msg.change = to_send;
                if (msgsnd(client.change_queue, &change_msg, sizeof(change_msg.change), 0) == -1) {
                    perror("Send client change");
                    exit(1);
                }
                log_num("add change to queue for client", client.id);
                log_msg("hairdresser finished");
            }
            exit(0);
        }
    }

    // CLIENT
    for (int client_id = 1; client_id <= NUM_OF_CLIENTS; client_id++) {
        if (fork() == 0) {
            srand(getpid());
            client client = new_client(client_id, change_queue);
            log_num("start client", getpid());
            for (int i = 0; i < 100; i++) {
                while (get_amount(client.money) < COST_PER_CUT) {
                    make_money(client);
                }
                log_num("wait for free seat", client.id);
                wait_for_free_seat(waiting_room);
                log_num("get free seat", client.id);
                take_seat(waiting_room, client);
                log_num("take seat", client.id);
                log_num("wait for change", client.id);
                change_msg_t change_msg;
                if (msgrcv(change_queue, &change_msg, sizeof(change_msg.change), client_id, 0) == -1) {
                    perror("Get change for client");
                    exit(1);
                }
                log_num("get change", client.id);
                client.money.ones += change_msg.change.ones;
                client.money.twos += change_msg.change.twos;
                client.money.fives += change_msg.change.fives;
                log_msg("client finished");
            }
            exit(0);
        }
    }

    for (int i = 0; i < NUM_OF_HAIRDRESSERS + NUM_OF_CLIENTS; i++) {
        wait(NULL);
    }

    return 0;
}
