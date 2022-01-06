#include "fryzjerzy_cash_machine.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "fryzjerzy_logger.h"
#include "fryzjerzy_semaphores_helpers.h"

void handle_error() {
    perror("Creating cash machine");
    exit(1);
}

void notify_hairdressers(cash_machine cash_machine) {
    log_msg("notify hairdressers");
    for (int i = 0; i < cash_machine.num_of_hairdressers; i++) {
        set_up(cash_machine.hairdressers_semaphores, i);
    }
}

cash_machine init_cash_machine(int num_of_hairdressers) {
    int money_id = shmget(IPC_PRIVATE, sizeof(money), IPC_CREAT | 0600);
    if (money_id == -1) {
        handle_error();
    }
    money *cash = (money *)shmat(money_id, NULL, 0);
    if (cash == (void *)-1) {
        printf("%d", errno);
        handle_error();
    }
    cash->ones = 3;
    cash->twos = 3;
    cash->fives = 3;

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
    cash_machine.cash = cash;
    cash_machine.semaphor = cash_machine_semaphor;
    cash_machine.num_of_hairdressers = num_of_hairdressers;
    cash_machine.hairdressers_semaphores = hairdressers_semaphores;
    log_msg("create cash machine");
    return cash_machine;
}

void add_cash(cash_machine cash_machine, money to_add) {
    log_money("wait for cash machine to add cash", to_add);
    down(cash_machine.semaphor, 0);
    cash_machine.cash->ones += to_add.ones;
    cash_machine.cash->twos += to_add.twos;
    cash_machine.cash->fives += to_add.fives;
    log_msg("add cash to machine");
    up(cash_machine.semaphor, 0);
    notify_hairdressers(cash_machine);
}

money cash_machine_change(cash_machine cash_machine, int amount, int hairdresser) {
    money change = {-1, -1, -1};
    while (is_change_incorrect(change)) {
        log_num("check for change hairdresser:", hairdresser);
        down(cash_machine.hairdressers_semaphores, hairdresser);
        log_num("try to change hairdresser:", hairdresser);
        down(cash_machine.semaphor, 0);
        change = get_change(*cash_machine.cash, amount);
        log_money("got change", change);
        if (is_change_correct(change)) {
            log_msg("got correct change");
            cash_machine.cash->ones -= change.ones;
            cash_machine.cash->twos -= change.twos;
            cash_machine.cash->fives -= change.fives;
        }
        up(cash_machine.semaphor, 0);
    }
    notify_hairdressers(cash_machine);
    return change;
}