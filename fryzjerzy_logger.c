#include "fryzjerzy_logger.h"

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

bool enabled() {
    return IS_LOGGER_ENABLED == 1 ? true : false;
}

void log_msg(char *msg) {
    if (enabled()) {
        printf("%d %s\n", getpid(), msg);
    }
}

void log_num(char *msg, int num) {
    if (enabled()) {
        printf("%d %s %d\n", getpid(), msg, num);
    }
}

void log_money(char *msg, money_t money) {
    if (enabled()) {
        printf("%d %s money: %d %d %d\n", getpid(), msg, money.ones, money.twos, money.fives);
    }
}
