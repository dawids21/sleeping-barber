#include "fryzjerzy_money.h"

#include <string.h>

#include "fryzjerzy_logger.h"

money get_change(money available, int amount) {
    int available_coins[3] = {available.ones, available.twos, available.fives};
    int values[3] = {1, 2, 5};
    int min_num_of_coins[amount + 1];
    int used[amount + 1][3];
    for (int i = 0; i < amount + 1; i++) {
        min_num_of_coins[i] = -1;
        used[i][0] = 0;
        used[i][1] = 0;
        used[i][2] = 0;
    }
    min_num_of_coins[0] = 0;
    for (int i = 1; i < amount + 1; i++) {
        int min = -1;
        int coin = -1;
        int max_value;
        if (i >= 5) {
            max_value = 2;
        } else if (i >= 2) {
            max_value = 1;
        } else {
            max_value = 0;
        }

        for (int value = 0; value <= max_value; value++) {
            int current_value = values[value];
            if (used[i - current_value][value] < available_coins[value]) {
                if (min_num_of_coins[i - current_value] != -1 && (min == -1 || min_num_of_coins[i - current_value] + 1 < min)) {
                    min = min_num_of_coins[i - current_value] + 1;
                    coin = value;
                }
            }
        }

        if (min == -1) {
            continue;
        }

        min_num_of_coins[i] = min;
        memcpy(used[i], used[i - values[coin]], sizeof(int) * 3);
        used[i][coin]++;
    }

    if (min_num_of_coins[amount] == -1) {
        money change = {-1, -1, -1};
        return change;
    }

    money change = {used[amount][0], used[amount][1], used[amount][2]};
    return change;
}

money count_minimum_coins(money available, int amount) {
    money result = {0, 0, 0};

    for (int i = 0; i < available.fives; i++) {
        amount -= 5;
        result.fives += 1;
        if (amount <= 0) {
            break;
        }
    }

    if (amount <= 0) {
        return result;
    }

    for (int i = 0; i < available.twos; i++) {
        amount -= 2;
        result.twos += 1;
        if (amount <= 0) {
            break;
        }
    }

    if (amount <= 0) {
        return result;
    }

    for (int i = 0; i < available.ones; i++) {
        amount -= 1;
        result.ones += 1;
        if (amount <= 0) {
            break;
        }
    }
    return result;
}

money count_maximum_coins(money available, int amount) {
    money result = {0, 0, 0};

    for (int i = 0; i < available.ones; i++) {
        amount -= 1;
        result.ones += 1;
        if (amount <= 0) {
            break;
        }
    }

    if (amount <= 0) {
        return result;
    }

    for (int i = 0; i < available.twos; i++) {
        amount -= 2;
        result.twos += 1;
        if (amount <= 0) {
            break;
        }
    }

    if (amount <= 0) {
        return result;
    }

    for (int i = 0; i < available.fives; i++) {
        amount -= 5;
        result.fives += 1;
        if (amount <= 0) {
            break;
        }
    }

    return result;
}

int get_amount(money money) {
    return money.ones + 2 * money.twos + 5 * money.fives;
}

bool is_change_correct(money change) {
    return change.ones != -1 && change.twos != -1 && change.fives != -1;
}

bool is_change_incorrect(money change) {
    log_money("check if incorrect", change);
    return change.ones == -1 || change.twos == -1 || change.fives == -1;
}

money subtract(money first, money second) {
    money result;
    result.ones = first.ones - second.ones;
    result.twos = first.twos - second.twos;
    result.fives = first.fives - second.fives;
    return result;
}

money add(money first, money second) {
    money result;
    result.ones = first.ones + second.ones;
    result.twos = first.twos + second.twos;
    result.fives = first.fives + second.fives;
    return result;
}