/*
 * report.h - Thống kê, báo cáo.
 */
#ifndef REPORT_H
#define REPORT_H

#include "room.h"
#include "guest.h"
#include "booking.h"

/* Doanh thu (các phiếu Đã trả) có ngày trả thuộc tháng/năm cho trước. Đệ quy. */
double sumRevenue(const BookingNode *p, int month, int year);

void revenueByMonth(const BookingNode *bookings, int year);
void revenueByRoom(const RoomNode *rooms, const BookingNode *bookings);
void reportAvailableToday(const RoomNode *rooms, const BookingNode *bookings);
void listCurrentGuests(const BookingNode *bookings, GuestNode *guests);

#endif /* REPORT_H */
