/*
 * room.h - Quản lý danh sách phòng (danh sách liên kết đơn).
 */
#ifndef ROOM_H
#define ROOM_H

#define ROOM_ID_LEN   10
#define ROOM_NOTE_LEN 50

#define ROOM_SINGLE 1  /* Phòng đơn */
#define ROOM_DOUBLE 2  /* Phòng đôi */
#define ROOM_VIP    3  /* Phòng VIP */

#define MIN_ROOM_PRICE 1.0
#define MAX_ROOM_PRICE 1000000000.0

/* Phần dữ liệu: đây là phần được ghi xuống file */
typedef struct {
    char   roomId[ROOM_ID_LEN];
    int    type;                 /* 1 = Đơn, 2 = Đôi, 3 = VIP */
    double pricePerNight;
    char   note[ROOM_NOTE_LEN];
} Room;

/* Node danh sách: chỉ tồn tại trong RAM, không ghi con trỏ next ra file */
typedef struct RoomNode {
    Room data;
    struct RoomNode *next;
} RoomNode;

/* Kết quả trả về của các thao tác thêm/xóa */
#define LIST_OK         0
#define LIST_DUPLICATE -1
#define LIST_NOT_FOUND -2
#define LIST_NO_MEMORY -3

/* Kiểu con trỏ hàm dùng cho sắp xếp và lọc */
typedef int (*RoomCompareFn)(const Room *a, const Room *b);
typedef int (*RoomFilterFn)(const Room *r, const void *ctx);

typedef struct {
    double minPrice;
    double maxPrice;
} PriceRange;

const char *roomTypeName(int type);

int       addRoom(RoomNode **head, Room r);              /* Thêm vào cuối, kiểm tra trùng mã */
int       deleteRoom(RoomNode **head, const char *roomId);
void      updateRoom(Room *r, int type, double pricePerNight, const char *note);
RoomNode *findRoomById(RoomNode *head, const char *roomId);
int       countRooms(const RoomNode *head);

/* Sắp xếp nhiều tiêu chí bằng con trỏ hàm */
void sortRooms(RoomNode *head, RoomCompareFn cmp);
int  cmpRoomById(const Room *a, const Room *b);
int  cmpRoomByPriceAsc(const Room *a, const Room *b);
int  cmpRoomByPriceDesc(const Room *a, const Room *b);
int  cmpRoomByTypeThenPrice(const Room *a, const Room *b);

/* Lọc: in các phòng thỏa điều kiện, trả về số phòng in ra */
int filterRoomByType(const Room *r, const void *ctx);      /* ctx: const int* */
int filterRoomByPrice(const Room *r, const void *ctx);     /* ctx: const PriceRange* */
int filterRoomByIdContains(const Room *r, const void *ctx); /* ctx: const char* (một phần mã phòng) */
int printRoomsIf(const RoomNode *head, RoomFilterFn pred, const void *ctx);

void printRoomHeader(void);
void printRoomRow(const Room *r);
void printRooms(const RoomNode *head);
void freeRoomList(RoomNode **head);

#endif /* ROOM_H */
