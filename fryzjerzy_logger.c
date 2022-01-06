#include "fryzjerzy_logger.h"

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

bool debug() {
    return LOG_LEVEL >= 2;
}

bool info() {
    return LOG_LEVEL >= 1;
}

void d_log(char *msg) {
    if (debug()) {
        printf("%d %s\n", getpid(), msg);
    }
}

void d_log_num(char *msg, int num) {
    if (debug()) {
        printf("%d %s %d\n", getpid(), msg, num);
    }
}

void d_log_money(char *msg, money money) {
    if (debug()) {
        printf("%d %s money: %d %d %d\n", getpid(), msg, money.ones, money.twos, money.fives);
    }
}

void i_log(char *msg) {
    if (info()) {
        printf("%d %s\n", getpid(), msg);
    }
}

void i_log_num(char *msg, int num) {
    if (info()) {
        printf("%d %s %d\n", getpid(), msg, num);
    }
}

void i_log_money(char *msg, money money) {
    if (info()) {
        printf("%d %s money: %d %d %d\n", getpid(), msg, money.ones, money.twos, money.fives);
    }
}

void i_log_num_money(char *msg, int num, money money) {
    if (info()) {
        printf("%d %s %d money: %d %d %d\n", getpid(), msg, num, money.ones, money.twos, money.fives);
    }
}