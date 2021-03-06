#include "fryzjerzy_waiting_room.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>

#include "fryzjerzy_logger.h"

#define EMPTY_SEAT 1
#define FULL_SEAT 2

typedef struct waiting_room_seat {
    long is_full;
    client client;
} waiting_room_seat;

waiting_room_seat empty_seat() {
    waiting_room_seat seat;
    seat.is_full = EMPTY_SEAT;
    return seat;
}

waiting_room init_waiting_room(int size) {
    int seats = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (seats == -1) {
        perror("Creating waiting room");
        exit(1);
    }
    for (int i = 0; i < size; i++) {
        waiting_room_seat seat = empty_seat();
        if (msgsnd(seats, &seat, sizeof(seat.client), 0) == -1) {
            perror("Creating waiting room");
            exit(1);
        }
    }

    waiting_room waiting_room;
    waiting_room.seats = seats;
    d_log("create waiting room");
    return waiting_room;
}

client take_client(waiting_room waiting_room) {
    waiting_room_seat seat;
    if (msgrcv(waiting_room.seats, &seat, sizeof(seat.client), FULL_SEAT, 0) == -1) {
        perror("Take client from waiting room");
        exit(1);
    }
    d_log("took client");
    waiting_room_seat empty = empty_seat();
    if (msgsnd(waiting_room.seats, &empty, sizeof(empty.client), 0) == -1) {
        perror("Free seat in waiting room");
        exit(1);
    }
    d_log("free seat");

    return seat.client;
}

bool wait_for_free_seat(waiting_room waiting_room) {
    waiting_room_seat seat;
    d_log("wait for free seat");
    if (msgrcv(waiting_room.seats, &seat, sizeof(seat.client), EMPTY_SEAT, IPC_NOWAIT) == -1) {
        if (errno == ENOMSG) {
            return false;
        }
        perror("Wait for free seat in waiting room");
        exit(1);
    }
    d_log("get free seat");
    return true;
}

void take_seat(waiting_room waiting_room, client client) {
    waiting_room_seat seat;
    seat.is_full = FULL_SEAT;
    seat.client = client;
    if (msgsnd(waiting_room.seats, &seat, sizeof(seat.client), 0) == -1) {
        perror("Take seat in waiting room");
        exit(1);
    }
    d_log_num("took free seat", client.id);
}