#ifndef BOOKING_H
#define BOOKING_H

#include "utils.h"
#include "room.h"
#include "guest.h"

/* Trang thai phieu */
#define BK_BOOKED     0
#define BK_STAYING    1
#define BK_CHECKEDOUT 2
#define BK_CANCELLED  3

/* Chinh sach tinh tien va gioi han */
#define LONG_STAY_NIGHTS   7
#define LONG_STAY_DISCOUNT 0.10
#define LATE_FEE_RATE      0.50
#define MAX_NIGHTS         365

/* Ma loi nghiep vu */
#define BK_OK               0
#define BK_ERR_ROOM        -1
#define BK_ERR_GUEST       -2
#define BK_ERR_DATE        -3
#define BK_ERR_CONFLICT    -4
#define BK_ERR_NOT_FOUND   -5
#define BK_ERR_STATUS      -6
#define BK_ERR_MEMORY      -7
#define BK_ERR_TOO_LONG    -8

typedef struct {
    int    bookingId;
    char   roomId[ROOM_ID_LEN];
    char   guestId[GUEST_ID_LEN];
    Date   checkIn, checkOut;
    int    status;
    double totalAmount;
} Booking;

typedef struct BookingNode {
    Booking data;
    struct BookingNode *next;
} BookingNode;

typedef struct {
    int    nights;
    double roomCharge;
    double discount;
    int    lateDays;
    double lateFee;
    double total;
} Invoice;

const char *bookingStatusName(int status);
const char *bookingErrorMessage(int code);

int  nightsBetween(Date in, Date out);
int  isDateRangeOverlap(Date newIn, Date newOut, Date oldIn, Date oldOut);
int  isRoomAvailable(const BookingNode *head, const char *roomId, Date in, Date out);
int  countAvailable(const RoomNode *p, Date in, Date out, const BookingNode *bk);
int  listAvailableRooms(const RoomNode *rooms, const BookingNode *bookings, Date in, Date out);

int  hasActiveBookingForRoom(const BookingNode *head, const char *roomId);
int  hasActiveBookingForGuest(const BookingNode *head, const char *guestId);

BookingNode *findBookingById(BookingNode *head, int bookingId);
int  nextBookingId(const BookingNode *head);

int  createBooking(BookingNode **head, RoomNode *rooms, GuestNode *guests,
                   const char *roomId, const char *guestId,
                   Date in, Date out, int *newId);
int  checkIn(BookingNode *head, int bookingId);
int  checkOut(BookingNode *head, RoomNode *rooms, int bookingId, Date actualOut, Invoice *inv);
int  cancelBooking(BookingNode *head, int bookingId);

double calcAmount(const Booking *b, double pricePerNight, Date actualOut, Invoice *inv);
void   printInvoice(const Booking *b, const Room *r, const Guest *g, const Invoice *inv);

void printBookingHeader(void);
void printBookingRow(const Booking *b);
void printBookingFooter(void);
int  printBookings(const BookingNode *head, int statusFilter);
int  printHistoryReverse(const BookingNode *p, const char *guestId);
void freeBookingList(BookingNode **head);

#endif
