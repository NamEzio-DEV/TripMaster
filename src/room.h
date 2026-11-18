#ifndef ROOM_H
#define ROOM_H

#define ROOM_ID_LEN   10 /* Do dai bo dem ma phong, tinh ca ky tu ket thuc chuoi. */
#define ROOM_NOTE_LEN 50 /* Do dai bo dem ghi chu phong, tinh ca ky tu ket thuc chuoi. */

#define ROOM_SINGLE 1 /* Ma loai phong don. */
#define ROOM_DOUBLE 2 /* Ma loai phong doi. */
#define ROOM_VIP    3 /* Ma loai phong VIP. */

#define MIN_ROOM_PRICE 1.0
#define MAX_ROOM_PRICE 1000000000.0

typedef struct {
    char   roomId[ROOM_ID_LEN];       /* Ma phong duy nhat. */
    int    type;                      /* Loai phong Don, Doi hoac VIP. */
    double pricePerNight;             /* Gia mot dem, tinh bang VND. */
    char   note[ROOM_NOTE_LEN];       /* Ghi chu tuy chon ve phong. */
} Room;

typedef struct RoomNode {
    Room data;                        /* Du lieu cua phong tai nut nay. */
    struct RoomNode *next;            /* Dia chi nut phong tiep theo. */
} RoomNode;

#define LIST_OK         0
#define LIST_DUPLICATE -1
#define LIST_NOT_FOUND -2
#define LIST_NO_MEMORY -3

typedef int (*RoomCompareFn)(const Room *a, const Room *b);
typedef int (*RoomFilterFn)(const Room *r, const void *ctx);

typedef struct {
    double minPrice;
    double maxPrice;
} PriceRange;

const char *roomTypeName(int type);

int       addRoom(RoomNode **head, Room r);
int       deleteRoom(RoomNode **head, const char *roomId);
void      updateRoom(Room *r, int type, double pricePerNight, const char *note);
RoomNode *findRoomById(RoomNode *head, const char *roomId);
int       countRooms(const RoomNode *head);

void sortRooms(RoomNode *head, RoomCompareFn cmp);
int  cmpRoomById(const Room *a, const Room *b);
int  cmpRoomByPriceAsc(const Room *a, const Room *b);
int  cmpRoomByPriceDesc(const Room *a, const Room *b);
int  cmpRoomByTypeThenPrice(const Room *a, const Room *b);

int filterRoomByType(const Room *r, const void *ctx);
int filterRoomByPrice(const Room *r, const void *ctx);
int filterRoomByIdContains(const Room *r, const void *ctx);
int printRoomsIf(const RoomNode *head, RoomFilterFn pred, const void *ctx);

void printRoomHeader(void);
void printRoomRow(const Room *r);
void printRooms(const RoomNode *head);
void freeRoomList(RoomNode **head);

#endif
