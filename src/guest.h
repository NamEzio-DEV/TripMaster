#ifndef GUEST_H
#define GUEST_H

#include "room.h"

#define GUEST_ID_LEN   12 /* Do dai bo dem ma khach, co tinh ky tu ket thuc chuoi. */
#define GUEST_NAME_LEN 50 /* Do dai bo dem ho ten. */
#define ID_CARD_LEN    13 /* Bo dem CCCD: toi da 12 chu so va mot ky tu ket thuc. */
#define PHONE_LEN      11 /* Bo dem dien thoai: toi da 10 chu so va mot ky tu ket thuc. */

#define ID_CARD_MIN_DIGITS 1
#define ID_CARD_MAX_DIGITS (ID_CARD_LEN - 1)
#define PHONE_MIN_DIGITS   1
#define PHONE_MAX_DIGITS   (PHONE_LEN - 1)

#define GUEST_ID_PREFIX "KH" /* Tien to cua ma khach tu dong, vi du KH001. */

#define LIST_DUPLICATE_ID_CARD -4

typedef struct {
    char guestId[GUEST_ID_LEN];       /* Ma khach do chuong trinh tu tao. */
    char fullName[GUEST_NAME_LEN];    /* Ho va ten day du cua khach. */
    char idCard[ID_CARD_LEN];         /* So CCCD chi gom chu so. */
    char phone[PHONE_LEN];           /* So dien thoai chi gom chu so. */
} Guest;

typedef struct GuestNode {
    Guest data;                       /* Thong tin khach tai nut nay. */
    struct GuestNode *next;           /* Dia chi nut khach tiep theo. */
} GuestNode;

int        addGuest(GuestNode **head, Guest g);
int        deleteGuest(GuestNode **head, const char *guestId);
void       updateGuest(Guest *g, const char *fullName, const char *idCard, const char *phone);
GuestNode *findGuestById(GuestNode *head, const char *guestId);
GuestNode *findGuestByIdCard(GuestNode *head, const char *idCard);
int        searchGuestsByName(const GuestNode *head, const char *keyword);
int        countGuests(const GuestNode *head);

void       generateGuestId(GuestNode *head, char *out, int size);

int        guestMatches(const Guest *g, const char *keyword);

void printGuestHeader(void);
void printGuestRow(const Guest *g);
void printGuestFooter(void);
void printGuests(const GuestNode *head);
void freeGuestList(GuestNode **head);

#endif
