/*
 * guest.c - Cài đặt các thao tác trên danh sách khách hàng.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "guest.h"
#include "utils.h"

GuestNode *findGuestById(GuestNode *head, const char *guestId) {
    GuestNode *p;
    for (p = head; p != NULL; p = p->next) {
        if (equalsIgnoreCase(p->data.guestId, guestId)) return p;
    }
    return NULL;
}

GuestNode *findGuestByIdCard(GuestNode *head, const char *idCard) {
    GuestNode *p;
    for (p = head; p != NULL; p = p->next) {
        if (strcmp(p->data.idCard, idCard) == 0) return p;
    }
    return NULL;
}

int addGuest(GuestNode **head, Guest g) {
    GuestNode *node, *p;
    if (findGuestById(*head, g.guestId) != NULL) return LIST_DUPLICATE;
    if (findGuestByIdCard(*head, g.idCard) != NULL) return LIST_DUPLICATE_ID_CARD;

    node = (GuestNode *)malloc(sizeof(GuestNode));
    if (node == NULL) return LIST_NO_MEMORY;
    node->data = g;
    node->next = NULL;

    if (*head == NULL) {
        *head = node;
    } else {
        for (p = *head; p->next != NULL; p = p->next) { }
        p->next = node;
    }
    return LIST_OK;
}

int deleteGuest(GuestNode **head, const char *guestId) {
    GuestNode **pp = head;
    while (*pp != NULL) {
        if (equalsIgnoreCase((*pp)->data.guestId, guestId)) {
            GuestNode *victim = *pp;
            *pp = victim->next;
            free(victim);
            return LIST_OK;
        }
        pp = &(*pp)->next;
    }
    return LIST_NOT_FOUND;
}

void updateGuest(Guest *g, const char *fullName, const char *idCard, const char *phone) {
    strncpy(g->fullName, fullName, GUEST_NAME_LEN - 1);
    g->fullName[GUEST_NAME_LEN - 1] = '\0';
    strncpy(g->idCard, idCard, ID_CARD_LEN - 1);
    g->idCard[ID_CARD_LEN - 1] = '\0';
    strncpy(g->phone, phone, PHONE_LEN - 1);
    g->phone[PHONE_LEN - 1] = '\0';
}

int countGuests(const GuestNode *head) {
    int n = 0;
    for (; head != NULL; head = head->next) n++;
    return n;
}

void generateGuestId(GuestNode *head, char *out, int size) {
    const GuestNode *p;
    size_t prefixLen = strlen(GUEST_ID_PREFIX);
    long maxNo = 0;

    /* Tìm số thứ tự lớn nhất trong các mã dạng KHxxx */
    for (p = head; p != NULL; p = p->next) {
        const char *id = p->data.guestId;
        if (strlen(id) > prefixLen && strncmp(id, GUEST_ID_PREFIX, prefixLen) == 0 &&
            isDigitsOnly(id + prefixLen)) {
            long n = strtol(id + prefixLen, NULL, 10);
            if (n > maxNo) maxNo = n;
        }
    }
    /* Vòng lặp phòng trường hợp mã đã bị dùng (dữ liệu nhập tay trước đây) */
    do {
        maxNo++;
        snprintf(out, (size_t)size, "%s%03ld", GUEST_ID_PREFIX, maxNo);
    } while (findGuestById(head, out) != NULL);
}

int guestMatches(const Guest *g, const char *keyword) {
    return containsIgnoreCase(g->guestId, keyword) ||
           containsIgnoreCase(g->fullName, keyword) ||
           containsIgnoreCase(g->idCard, keyword) ||
           containsIgnoreCase(g->phone, keyword);
}

void printGuestHeader(void) {
    printf("+-------------+----------------------------------------------------+--------------+------------+\n");
    printf("| %-11s | %-50s | %-12s | %-10s |\n", "Ma khach", "Ho ten", "CCCD", "SDT");
    printf("+-------------+----------------------------------------------------+--------------+------------+\n");
}

void printGuestRow(const Guest *g) {
    printf("| %-11s | %-50s | %-12s | %-10s |\n", g->guestId, g->fullName, g->idCard, g->phone);
}

void printGuestFooter(void) {
    printf("+-------------+----------------------------------------------------+--------------+------------+\n");
}

int searchGuestsByName(const GuestNode *head, const char *keyword) {
    int count = 0;
    printGuestHeader();
    for (; head != NULL; head = head->next) {
        if (containsIgnoreCase(head->data.fullName, keyword)) {
            printGuestRow(&head->data);
            count++;
        }
    }
    printGuestFooter();
    printf("Tim thay: %d khach.\n", count);
    return count;
}

void printGuests(const GuestNode *head) {
    int count = 0;
    printGuestHeader();
    for (; head != NULL; head = head->next) {
        printGuestRow(&head->data);
        count++;
    }
    printGuestFooter();
    printf("Tong: %d khach.\n", count);
}

void freeGuestList(GuestNode **head) {
    GuestNode *p = *head, *next;
    while (p != NULL) {
        next = p->next;
        free(p);
        p = next;
    }
    *head = NULL;
}
