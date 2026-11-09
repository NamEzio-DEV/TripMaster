/*
 * room.c - Cài đặt các thao tác trên danh sách phòng.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "room.h"
#include "utils.h"

const char *roomTypeName(int type) {
    switch (type) {
        case ROOM_SINGLE: return "Don";
        case ROOM_DOUBLE: return "Doi";
        case ROOM_VIP:    return "VIP";
        default:          return "?";
    }
}

RoomNode *findRoomById(RoomNode *head, const char *roomId) {
    RoomNode *p;
    for (p = head; p != NULL; p = p->next) {
        if (equalsIgnoreCase(p->data.roomId, roomId)) return p;
    }
    return NULL;
}

/* Thêm phòng vào cuối danh sách để giữ đúng thứ tự khi ghi/đọc file */
int addRoom(RoomNode **head, Room r) {
    RoomNode *node, *p;
    if (findRoomById(*head, r.roomId) != NULL) return LIST_DUPLICATE;

    node = (RoomNode *)malloc(sizeof(RoomNode));
    if (node == NULL) return LIST_NO_MEMORY;
    node->data = r;
    node->next = NULL;

    if (*head == NULL) {
        *head = node;
    } else {
        for (p = *head; p->next != NULL; p = p->next) { }
        p->next = node;
    }
    return LIST_OK;
}

/*
 * Xóa phòng theo mã. Dùng con trỏ tới con trỏ (RoomNode **) nên xử lý
 * chung được cả ba trường hợp: node đầu, node giữa và node cuối.
 * Việc kiểm tra "phòng còn đặt chưa trả" do tầng nghiệp vụ (booking) làm.
 */
int deleteRoom(RoomNode **head, const char *roomId) {
    RoomNode **pp = head;
    while (*pp != NULL) {
        if (equalsIgnoreCase((*pp)->data.roomId, roomId)) {
            RoomNode *victim = *pp;
            *pp = victim->next;
            free(victim);
            return LIST_OK;
        }
        pp = &(*pp)->next;
    }
    return LIST_NOT_FOUND;
}

void updateRoom(Room *r, int type, double pricePerNight, const char *note) {
    r->type = type;
    r->pricePerNight = pricePerNight;
    strncpy(r->note, note, ROOM_NOTE_LEN - 1);
    r->note[ROOM_NOTE_LEN - 1] = '\0';
}

int countRooms(const RoomNode *head) {
    int n = 0;
    for (; head != NULL; head = head->next) n++;
    return n;
}

/* ---------- Sắp xếp ---------- */

int cmpRoomById(const Room *a, const Room *b) {
    return strcmp(a->roomId, b->roomId);
}

int cmpRoomByPriceAsc(const Room *a, const Room *b) {
    if (a->pricePerNight < b->pricePerNight) return -1;
    if (a->pricePerNight > b->pricePerNight) return 1;
    return cmpRoomById(a, b);
}

int cmpRoomByPriceDesc(const Room *a, const Room *b) {
    return cmpRoomByPriceAsc(b, a);
}

int cmpRoomByTypeThenPrice(const Room *a, const Room *b) {
    if (a->type != b->type) return a->type - b->type;
    return cmpRoomByPriceAsc(a, b);
}

/*
 * Sắp xếp chọn (selection sort) trên danh sách liên kết.
 * Chỉ hoán đổi phần data, không cần nối lại con trỏ next.
 * Tiêu chí sắp xếp được truyền vào qua con trỏ hàm cmp.
 */
void sortRooms(RoomNode *head, RoomCompareFn cmp) {
    RoomNode *i, *j, *best;
    Room tmp;
    for (i = head; i != NULL; i = i->next) {
        best = i;
        for (j = i->next; j != NULL; j = j->next) {
            if (cmp(&j->data, &best->data) < 0) best = j;
        }
        if (best != i) {
            tmp = i->data;
            i->data = best->data;
            best->data = tmp;
        }
    }
}

/* ---------- Lọc ---------- */

int filterRoomByType(const Room *r, const void *ctx) {
    return r->type == *(const int *)ctx;
}

int filterRoomByPrice(const Room *r, const void *ctx) {
    const PriceRange *range = (const PriceRange *)ctx;
    return r->pricePerNight >= range->minPrice && r->pricePerNight <= range->maxPrice;
}

/* ---------- In ---------- */

void printRoomHeader(void) {
    printf("+------------+------+-----------------+----------------------------------------------------+\n");
    printf("| %-10s | %-4s | %15s | %-50s |\n", "Ma phong", "Loai", "Gia/dem (VND)", "Ghi chu");
    printf("+------------+------+-----------------+----------------------------------------------------+\n");
}

void printRoomRow(const Room *r) {
    printf("| %-10s | %-4s | %15.0f | %-50s |\n",
           r->roomId, roomTypeName(r->type), r->pricePerNight, r->note);
}

static void printRoomFooter(void) {
    printf("+------------+------+-----------------+----------------------------------------------------+\n");
}

int printRoomsIf(const RoomNode *head, RoomFilterFn pred, const void *ctx) {
    int count = 0;
    printRoomHeader();
    for (; head != NULL; head = head->next) {
        if (pred == NULL || pred(&head->data, ctx)) {
            printRoomRow(&head->data);
            count++;
        }
    }
    printRoomFooter();
    printf("Tong: %d phong.\n", count);
    return count;
}

void printRooms(const RoomNode *head) {
    printRoomsIf(head, NULL, NULL);
}

void freeRoomList(RoomNode **head) {
    RoomNode *p = *head, *next;
    while (p != NULL) {
        next = p->next;
        free(p);
        p = next;
    }
    *head = NULL;
}
