#include "fryzjerzy_client.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <unistd.h>

int new_change_queue() {
    int change_queue = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (change_queue == -1) {
        perror("Create waiting room");
        exit(1);
    }
}

client new_client(int id, int change_queue) {
    client client;
    client.id = id;
    money money;
    money.ones = 0;
    money.twos = 0;
    money.fives = 0;
}

void make_money(client client) {
    usleep(rand() % 500000);
    money earned;
    int random = rand();
    earned.ones = random % 6;
    earned.twos = random % 4;
    earned.fives = random % 2;
    client.money = add(client.money, earned);
}