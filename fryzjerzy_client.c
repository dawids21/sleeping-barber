#include "fryzjerzy_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <unistd.h>

#include "fryzjerzy_logger.h"

typedef struct change_msg {
    long client_id;
    money change;
} change_msg;

int new_change_queue() {
    int change_queue = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (change_queue == -1) {
        perror("Create waiting room");
        exit(1);
    }
    return change_queue;
}

client new_client(int id, int change_queue) {
    client client;
    client.id = id;
    money money;
    money.ones = 0;
    money.twos = 0;
    money.fives = 0;
    client.money = money;
    client.change_queue = change_queue;
    return client;
}

void make_money(client client) {
    usleep(rand() % 500000);
    money earned;
    int random = rand();
    earned.ones = 1;
    earned.twos = 1;
    earned.fives = rand() % 2;
    i_log_num_money("Money earned by client", client.id, earned);
    client.money = add(client.money, earned);
}

void send_change(client client, money change) {
    change_msg change_msg;
    change_msg.client_id = client.id;
    change_msg.change = change;
    if (msgsnd(client.change_queue, &change_msg, sizeof(change_msg.change), 0) == -1) {
        perror("Send client change");
        exit(1);
    }
}

void wait_for_change(client client) {
    change_msg change_msg;
    if (msgrcv(client.change_queue, &change_msg, sizeof(change_msg.change), client.id, 0) == -1) {
        perror("Get change for client");
        exit(1);
    }
    client.money = add(client.money, change_msg.change);
}