#ifndef FRYZJERZY_MONEY_H
#define FRYZJERZY_MONEY_H

typedef struct {
    int ones;
    int twos;
    int fives;
} money_t;

money_t get_change(money_t available, int amount);

#endif