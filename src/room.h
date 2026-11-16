#ifndef ROOM_H
#define ROOM_H

#define ROOM_ID_LEN   10
#define ROOM_NOTE_LEN 50

#define ROOM_SINGLE 1
#define ROOM_DOUBLE 2
#define ROOM_VIP    3

#define MIN_ROOM_PRICE 1.0
#define MAX_ROOM_PRICE 1000000000.0

typedef struct {
    char   roomId[ROOM_ID_LEN];
    int    type;
    double pricePerNight;
    char   note[ROOM_NOTE_LEN];
} Room;

typedef struct RoomNode {
    Room data;
    struct RoomNode *next;
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
