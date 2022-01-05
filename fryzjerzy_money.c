#include "fryzjerzy_money.h"

#include <string.h>

money_t get_change(int coins[3], int amount) {
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
            if (used[i - current_value][value] < coins[value]) {
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

    money_t change = {used[amount][0], used[amount][1], used[amount][2]};
    return change;
}