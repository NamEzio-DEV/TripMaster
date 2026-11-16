#ifndef GUEST_H
#define GUEST_H

#include "room.h"

#define GUEST_ID_LEN   12
#define GUEST_NAME_LEN 50
#define ID_CARD_LEN    13
#define PHONE_LEN      11

#define ID_CARD_MIN_DIGITS 1
#define ID_CARD_MAX_DIGITS (ID_CARD_LEN - 1)
#define PHONE_MIN_DIGITS   1
#define PHONE_MAX_DIGITS   (PHONE_LEN - 1)

#define GUEST_ID_PREFIX "KH"

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
