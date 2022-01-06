#ifndef FRYZJERZY_MONEY_H
#define FRYZJERZY_MONEY_H

#include <stdbool.h>

typedef struct money {
    int ones;
    int twos;
    int fives;
} money;

money get_change(money available, int amount);

money count_minimum_coins(money available, int amount);

money count_maximum_coins(money available, int amount);

int get_amount(money money);

bool is_change_correct(money change);

bool is_change_incorrect(money change);

#endif