#include "fryzjerzy_cash_machine.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "fryzjerzy_semaphores_helpers.h"

void handle_error() {
    perror("Creating cash machine");
    exit(1);
}

void notify_hairdressers(cash_machine cash_machine) {
    for (int i = 0; i < cash_machine.num_of_hairdressers; i++) {
        set_up(cash_machine.hairdressers_semaphores, i);
    }
}

cash_machine init_cash_machine(int num_of_hairdressers) {
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

    int hairdressers_semaphores = semget(IPC_PRIVATE, num_of_hairdressers, IPC_CREAT | 0600);
    if (hairdressers_semaphores == -1) {
        handle_error();
    }
    for (int i = 0; i < num_of_hairdressers; i++) {
        set_up(hairdressers_semaphores, i);
    }

    cash_machine cash_machine;
    cash_machine.cash = money;
    cash_machine.semaphor = cash_machine_semaphor;
    cash_machine.num_of_hairdressers = num_of_hairdressers;
    cash_machine.hairdressers_semaphores = hairdressers_semaphores;
    return cash_machine;
}

void add_cash(cash_machine cash_machine, money_t to_add) {
    down(cash_machine.semaphor, 0);
    cash_machine.cash->ones += to_add.ones;
    cash_machine.cash->twos += to_add.twos;
    cash_machine.cash->fives += to_add.fives;
    up(cash_machine.semaphor, 0);
    notify_hairdressers(cash_machine);
}

money_t cash_machine_change(cash_machine cash_machine, int amount, int hairdresser) {
    money_t change = {-1, -1, -1};
    while (change.ones == -1 || change.twos == -1 || change.fives == -1) {
        down(cash_machine.hairdressers_semaphores, hairdresser);
        down(cash_machine.semaphor, 0);
        money_t change = get_change(*cash_machine.cash, amount);
        if (change.ones == -1 && change.twos == -1 && change.fives == -1) {
            cash_machine.cash->ones -= change.ones;
            cash_machine.cash->twos -= change.twos;
            cash_machine.cash->fives -= change.fives;
        }
        up(cash_machine.semaphor, 0);
    }
    notify_hairdressers(cash_machine);
    return change;
}