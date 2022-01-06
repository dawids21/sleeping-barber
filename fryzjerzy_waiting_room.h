#if !defined(FRYZJERZY_WAITING_ROOM_H)
#define FRYZJERZY_WAITING_ROOM_H

#include "fryzjerzy_client.h"

typedef struct waiting_room {
    int seats;
} waiting_room;

waiting_room init_waiting_room(int size);

client take_client(waiting_room waiting_room);

void wait_for_free_seat(waiting_room waiting_room);

void take_seat(waiting_room waiting_room, client client);

#endif  // FRYZJERZY_WAITING_ROOM_H
