#if !defined(FRYZJERZY_CLIENT_H)
#define FRYZJERZY_CLIENT_H

#include "fryzjerzy_money.h"

typedef struct client {
    int id;
    int change_queue;
    money money;
} client;

int new_change_queue();

#endif  // FRYZJERZY_CLIENT_H
