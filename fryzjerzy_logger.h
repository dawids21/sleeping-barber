#if !defined(FRYZJERZY_LOGGER_H)
#define FRYZJERZY_LOGGER_H

#include "fryzjerzy_money.h"

#define OFF 0
#define INFO 1
#define DEBUG 2

#define LOG_LEVEL INFO

void d_log(char *msg);

void d_log_num(char *msg, int num);

void d_log_money(char *msg, money money);

void i_log(char *msg);

void i_log_num(char *msg, int num);

void i_log_money(char *msg, money money);

void i_log_num_money(char *msg, int num, money money);

void i_log_hairdresser_client(char *msg, int haidresser, int client);

void i_log_hairdresser_client_money(char *msg, int haidresser, int client, money money);

#endif  // FRYZJERZY_LOGGER_H
