/*
 * booking.h - Nghiệp vụ đặt phòng, nhận phòng, trả phòng, tính tiền.
 */
#ifndef BOOKING_H
#define BOOKING_H

#include "utils.h"
#include "room.h"
#include "guest.h"

/* Trạng thái phiếu đặt */
#define BK_BOOKED     0   /* Đã đặt */
#define BK_STAYING    1   /* Đang ở */
#define BK_CHECKEDOUT 2   /* Đã trả */
#define BK_CANCELLED  3   /* Hủy */

/* Chính sách tính tiền */
#define LONG_STAY_NIGHTS   7      /* Ở từ 7 đêm trở lên được giảm giá */
#define LONG_STAY_DISCOUNT 0.10   /* Giảm 10% tiền phòng */
#define LATE_FEE_RATE      0.50   /* Mỗi ngày trả muộn tính 1 đêm + phụ thu 50% */
#define MAX_NIGHTS         365    /* Giới hạn số đêm một lần đặt */

/* Mã lỗi nghiệp vụ */
#define BK_OK               0
#define BK_ERR_ROOM        -1   /* Không có phòng */
#define BK_ERR_GUEST       -2   /* Không có khách */
#define BK_ERR_DATE        -3   /* Ngày không hợp lệ hoặc ngày trả <= ngày nhận */
#define BK_ERR_CONFLICT    -4   /* Trùng lịch */
#define BK_ERR_NOT_FOUND   -5   /* Không có phiếu */
#define BK_ERR_STATUS      -6   /* Trạng thái phiếu không cho phép thao tác */
#define BK_ERR_MEMORY      -7
#define BK_ERR_TOO_LONG    -8   /* Vượt quá MAX_NIGHTS */

typedef struct {
    int    bookingId;
    char   roomId[ROOM_ID_LEN];
    char   guestId[GUEST_ID_LEN];
    Date   checkIn, checkOut;
    int    status;          /* 0 = Đã đặt, 1 = Đang ở, 2 = Đã trả, 3 = Hủy */
    double totalAmount;     /* Tạm tính khi đặt, số tiền thực khi trả phòng */
} Booking;

typedef struct BookingNode {
    Booking data;
    struct BookingNode *next;
} BookingNode;

/* Chi tiết hóa đơn (chỉ dùng trong RAM để in) */
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
int  countAvailable(const RoomNode *p, Date in, Date out, const BookingNode *bk); /* Đệ quy */
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
int  printBookings(const BookingNode *head, int statusFilter);   /* -1 = tất cả */
int  printHistoryReverse(const BookingNode *p, const char *guestId); /* Đệ quy, mới nhất trước */
void freeBookingList(BookingNode **head);

#endif /* BOOKING_H */
