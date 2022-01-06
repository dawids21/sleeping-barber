#ifndef FRYZJERZY_MONEY_H
#define FRYZJERZY_MONEY_H

#include <stdbool.h>

typedef struct {
    int ones;
    int twos;
    int fives;
} money_t;

money_t get_change(money_t available, int amount);

money_t count_minimum_coins(money_t available, int amount);

money_t count_maximum_coins(money_t available, int amount);

int get_amount(money_t money);

bool is_change_correct(money_t change);

bool is_change_incorrect(money_t change);

#endif