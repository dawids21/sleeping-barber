#include "fryzjerzy_cash_machine.h"

#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "fryzjerzy_semaphores_helpers.h"

void handle_error() {
    perror("Creating cash machine");
    exit(1);
}

cash_machine init() {
    int money_id = shmget(IPC_PRIVATE, sizeof(money_t), 0);
    if (money_id == -1) {
        handle_error();
    }
    money_t *money = (money_t *)shmat(money_id, NULL, 0);
    if (money == NULL) {
        handle_error();
    }
    money->ones = 3;
    money->twos = 3;
    money->fives = 3;

    int cash_machine_semaphor = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if (cash_machine_semaphor == -1) {
        handle_error();
    }

    set_up(cash_machine_semaphor, 0);

    cash_machine cash_machine;
    cash_machine.cash = money;
    cash_machine.semaphor = cash_machine_semaphor;
    return cash_machine;
}

void add_cash(cash_machine cash_machine, money_t to_add) {
    down(cash_machine.semaphor, 0);
    cash_machine.cash->ones += to_add.ones;
    cash_machine.cash->twos += to_add.twos;
    cash_machine.cash->fives += to_add.fives;
    up(cash_machine.semaphor, 0);
}