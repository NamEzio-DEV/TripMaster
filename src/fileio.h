#ifndef FILEIO_H
#define FILEIO_H

#include "room.h"
#include "guest.h"
#include "booking.h"

#define DATA_DIR      "data/"
#define ROOMS_FILE    "rooms.dat"
#define GUESTS_FILE   "guests.dat"
#define BOOKINGS_FILE "bookings.dat"
#define DIR_LEN       200
#define PATH_LEN      260

void        setDataDir(const char *dir);
const char *getDataDir(void);

int saveRooms(const RoomNode *head);
int loadRooms(RoomNode **head);
int saveGuests(const GuestNode *head);
int loadGuests(GuestNode **head);
int saveBookings(const BookingNode *head);
int loadBookings(BookingNode **head);

int saveAll(const RoomNode *rooms, const GuestNode *guests, const BookingNode *bookings);

int loadAll(RoomNode **rooms, GuestNode **guests, BookingNode **bookings);

#endif
