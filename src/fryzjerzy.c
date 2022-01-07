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

// to simplify the program keep NUM_OF_CLIENTS * CUTS_PER_CLIENT / NUM_OF_HAIRDRESSER as integer

#define SIZE_OF_WAITING_ROOM 3
#define NUM_OF_HAIRDRESSERS 5
#define NUM_OF_CLIENTS 10
#define NUM_OF_CHAIRS 3
#define COST_PER_CUT 30
#define CUTS_PER_CLIENT 5

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
            for (int i = 0; i < NUM_OF_CLIENTS * CUTS_PER_CLIENT / NUM_OF_HAIRDRESSERS; i++) {
                client client = take_client(waiting_room);
                i_log_hairdresser_client("H Take client from waiting room.", hairdresser_id, client.id);
                down(chairs, 0);
                i_log_hairdresser_client("H Found free chair.", hairdresser_id, client.id);
                money to_pay;
                if (rand() % 2 == 0) {
                    to_pay = count_minimum_coins(client.money, COST_PER_CUT);
                } else {
                    to_pay = count_maximum_coins(client.money, COST_PER_CUT);
                }
                i_log_num_money("C Found hairdresser. Pay for service. Client", client.id, to_pay);
                usleep(rand() % 100000);
                i_log_num("H Add money to cash machine. hairdresser", hairdresser_id);
                add_cash(&cash_machine, to_pay);
                int to_return = get_amount(to_pay) - COST_PER_CUT;
                i_log_hairdresser_client("H Start service.", hairdresser_id, client.id);
                usleep(rand() % 5000000);
                i_log_hairdresser_client("H Finish service.", hairdresser_id, client.id);
                up(chairs, 0);
                i_log_num("H Free chair hairdresser", hairdresser_id);
                i_log_num("C Wait for change. Client", client.id);
                d_log_num("finished client", client.id);
                i_log_hairdresser_client("H Wait for change in cash machine", hairdresser_id, client.id);
                money change = cash_machine_change(&cash_machine, to_return, hairdresser_id);
                d_log_num("get change for", client.id);
                money to_send = subtract(change, to_pay);
                d_log_money("send change", to_send);
                usleep(rand() % 100000);
                i_log_hairdresser_client("H Return change to client", hairdresser_id, client.id);
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
            for (int i = 0; i < CUTS_PER_CLIENT; i++) {
                while (get_amount(client.money) < COST_PER_CUT) {
                    make_money(&client);
                }
                usleep(rand() % 500000);
                i_log_num("C Enter waiting room, look for free seats. Client", client.id);
                while (!wait_for_free_seat(waiting_room)) {
                    i_log_num("C No free seats, exit. Client", client.id);
                    usleep(rand() % 500000);
                    make_money(&client);
                    usleep(rand() % 500000);
                    i_log_num("C Enter waiting room, look for free seats. Client", client.id);
                }
                i_log_num("C Get free seat. Wait for hairdresser. Client", client.id);
                take_seat(waiting_room, client);
                d_log_num("take seat", client.id);
                d_log_num("wait for change", client.id);
                wait_for_change(&client);
                i_log_num("C Exit. Client", client_id);
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
