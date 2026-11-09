/*
 * fileio.h - Đọc/ghi dữ liệu ra file nhị phân (.dat).
 *
 * Định dạng mỗi file: [int số bản ghi][bản ghi 1][bản ghi 2]...
 * Chỉ ghi phần data (Room/Guest/Booking), không ghi con trỏ next.
 */
#ifndef FILEIO_H
#define FILEIO_H

#include "room.h"
#include "guest.h"
#include "booking.h"

#define DATA_DIR      "data/"
#define ROOMS_FILE    "rooms.dat"
#define GUESTS_FILE   "guests.dat"
#define BOOKINGS_FILE "bookings.dat"
#define DIR_LEN       200   /* Độ dài tối đa đường dẫn thư mục dữ liệu */
#define PATH_LEN      260   /* Thư mục + tên file + ".tmp" luôn vừa */

/* Đổi thư mục dữ liệu (lấy từ đối dòng lệnh). Mặc định là DATA_DIR. */
void        setDataDir(const char *dir);
const char *getDataDir(void);

int saveRooms(const RoomNode *head);
int loadRooms(RoomNode **head);
int saveGuests(const GuestNode *head);
int loadGuests(GuestNode **head);
int saveBookings(const BookingNode *head);
int loadBookings(BookingNode **head);

/* Trả về 1 nếu thành công */
int saveAll(const RoomNode *rooms, const GuestNode *guests, const BookingNode *bookings);
/* Trả về số bản ghi đọc được; file chưa có thì coi như danh sách rỗng */
int loadAll(RoomNode **rooms, GuestNode **guests, BookingNode **bookings);

#endif /* FILEIO_H */
