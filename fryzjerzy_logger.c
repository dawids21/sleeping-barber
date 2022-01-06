#include "fryzjerzy_logger.h"

#include <stdbool.h>
#include <stdio.h>

bool enabled() {
    return IS_LOGGER_ENABLED == 1 ? true : false;
}

void log_msg(char type, int id, char *msg) {
    if (enabled()) {
        printf("%c%d %s\n", type, id, msg);
    }
}

void log_num(char type, int id, char *msg, int num) {
    if (enabled()) {
        printf("%c%d %s %d\n", type, id, msg, num);
    }
}
