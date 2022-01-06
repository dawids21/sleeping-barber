#ifndef FRYZJERZY_CASH_MACHINE_H
#define FRYZJERZY_CASH_MACHINE_H

#include "fryzjerzy_money.h"

typedef struct cash_machine {
    money_t *cash;
    int semaphor;
    int num_of_hairdressers;
    int hairdressers_semaphores;
} cash_machine;

cash_machine init_cash_machine(int num_of_hairdressers);

void add_cash(cash_machine cash_machine, money_t to_add);

money_t cash_machine_change(cash_machine cash_machine, int amount, int hairdresser);

#endif