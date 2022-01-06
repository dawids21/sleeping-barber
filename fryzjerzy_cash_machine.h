#ifndef FRYZJERZY_CASH_MACHINE_H
#define FRYZJERZY_CASH_MACHINE_H

#include "fryzjerzy_money.h"

typedef struct cash_machine {
    money_t *cash;
    int semaphor;
} cash_machine;

cash_machine init();

void add_cash(cash_machine cash_machine, money_t to_add);

money_t change(cash_machine cash_machine, int amount);

#endif