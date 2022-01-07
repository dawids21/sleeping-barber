#if !defined(FRYZJERZY_CLIENT_H)
#define FRYZJERZY_CLIENT_H

#include "fryzjerzy_money.h"

typedef struct client {
    int id;
    int change_queue;
    money money;
} client;

int new_change_queue();

client new_client(int id, int change_queue);

void make_money(client *client);

void send_change(client client, money change);

void wait_for_change(client client);

#endif  // FRYZJERZY_CLIENT_H
