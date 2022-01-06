#if !defined(FRYZJERZY_LOGGER_H)
#define FRYZJERZY_LOGGER_H

#include "fryzjerzy_money.h"

#define IS_LOGGER_ENABLED 1

void log_msg(char *msg);

void log_num(char *msg, int num);

void log_money(char *msg, money money);

#endif  // FRYZJERZY_LOGGER_H
