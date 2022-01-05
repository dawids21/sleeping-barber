#ifndef FRYZJERZY_MONEY_H
#define FRYZJERZY_MONEY_H

typedef struct {
    int ones;
    int twos;
    int fives;
} money_t;

money_t get_change(money_t available, int amount);

money_t count_minimum_coins(money_t available, int amount);

money_t count_maximum_coins(money_t available, int amount);

int get_amount(money_t money);

#endif