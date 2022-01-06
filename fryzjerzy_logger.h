#if !defined(FRYZJERZY_LOGGER_H)
#define FRYZJERZY_LOGGER_H

#include "fryzjerzy_money.h"

#define LOG_LEVEL 1

void d_log(char *msg);

void d_log_num(char *msg, int num);

void d_log_money(char *msg, money money);

void i_log(char *msg);

void i_log_num(char *msg, int num);

void i_log_money(char *msg, money money);

#endif  // FRYZJERZY_LOGGER_H
