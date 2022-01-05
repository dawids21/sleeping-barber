#ifndef FRYZJERZY_CHANGE_H
#define FRYZJERZY_CHANGE_H

typedef struct {
    int ones;
    int twos;
    int fives;
} money_t;

money_t get_change(int coins[3], int amount);

#endif