#if !defined(FRYZJERZY_SEMAPHORES_HELPERS_H)
#define FRYZJERZY_SEMAPHORES_HELPERS_H

void up(int semid, int semnum);

void down(int semid, int semnum);

void set_up(int semid, int semnum);

void set_down(int semid, int semnum);

#endif  // FRYZJERZY_SEMAPHORES_HELPERS_H
