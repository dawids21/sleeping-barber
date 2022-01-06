#if !defined(FRYZJERZY_CLIENT_H)
#define FRYZJERZY_CLIENT_H

#include "fryzjerzy_money.h"

typedef struct client {
    int id;
    money_t money;
} client;

#endif  // FRYZJERZY_CLIENT_H
