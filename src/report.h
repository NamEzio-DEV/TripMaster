#ifndef REPORT_H
#define REPORT_H

#include "room.h"
#include "guest.h"
#include "booking.h"

double sumRevenue(const BookingNode *p, int month, int year);

void revenueByMonth(const BookingNode *bookings, int year);
void revenueByRoom(const RoomNode *rooms, const BookingNode *bookings);
void reportAvailableToday(const RoomNode *rooms, const BookingNode *bookings);
void listCurrentGuests(const BookingNode *bookings, GuestNode *guests);

#endif
