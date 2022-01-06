#if !defined(FRYZJERZY_LOGGER_H)
#define FRYZJERZY_LOGGER_H

#define IS_LOGGER_ENABLED 1

void log_msg(char type, int id, char *msg);

void log_num(char type, int id, char *msg, int num);

#endif  // FRYZJERZY_LOGGER_H
