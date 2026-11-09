/*
 * guest.h - Quản lý danh sách khách hàng (danh sách liên kết đơn).
 */
#ifndef GUEST_H
#define GUEST_H

#include "room.h"   /* Dùng chung các mã LIST_OK, LIST_DUPLICATE, ... */

#define GUEST_ID_LEN   12
#define GUEST_NAME_LEN 50
#define ID_CARD_LEN    13   /* CCCD 12 chữ số + '\0' */
#define PHONE_LEN      11   /* SĐT 10 chữ số + '\0' */

#define ID_CARD_DIGITS 12
#define PHONE_DIGITS   10

#define LIST_DUPLICATE_ID_CARD -4

typedef struct {
    char guestId[GUEST_ID_LEN];
    char fullName[GUEST_NAME_LEN];
    char idCard[ID_CARD_LEN];
    char phone[PHONE_LEN];
} Guest;

typedef struct GuestNode {
    Guest data;
    struct GuestNode *next;
} GuestNode;

int        addGuest(GuestNode **head, Guest g);   /* Kiểm tra trùng mã khách và trùng CCCD */
int        deleteGuest(GuestNode **head, const char *guestId);
void       updateGuest(Guest *g, const char *fullName, const char *idCard, const char *phone);
GuestNode *findGuestById(GuestNode *head, const char *guestId);
GuestNode *findGuestByIdCard(GuestNode *head, const char *idCard);
int        searchGuestsByName(const GuestNode *head, const char *keyword); /* In kết quả, trả về số lượng */
int        countGuests(const GuestNode *head);

void printGuestHeader(void);
void printGuestRow(const Guest *g);
void printGuestFooter(void);
void printGuests(const GuestNode *head);
void freeGuestList(GuestNode **head);

#endif /* GUEST_H */
