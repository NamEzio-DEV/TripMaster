#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "booking.h"

const char *bookingStatusName(int status) {
    switch (status) {
        case BK_BOOKED:     return "Da dat";
        case BK_STAYING:    return "Dang o";
        case BK_CHECKEDOUT: return "Da tra";
        case BK_CANCELLED:  return "Huy";
        default:            return "?";
    }
}

const char *bookingErrorMessage(int code) {
    switch (code) {
        case BK_OK:            return "Thanh cong.";
        case BK_ERR_ROOM:      return "Khong tim thay phong.";
        case BK_ERR_GUEST:     return "Khong tim thay khach.";
        case BK_ERR_DATE:      return "Ngay khong hop le (ngay tra phai sau ngay nhan).";
        case BK_ERR_CONFLICT:  return "Phong da co nguoi dat trong khoang ngay nay (trung lich).";
        case BK_ERR_NOT_FOUND: return "Khong tim thay phieu dat.";
        case BK_ERR_STATUS:    return "Trang thai phieu khong cho phep thao tac nay.";
        case BK_ERR_MEMORY:    return "Khong du bo nho.";
        case BK_ERR_TOO_LONG:  return "So dem vuot qua gioi han cho phep.";
        default:               return "Loi khong xac dinh.";
    }
}

static int isActive(int status) {
    return status == BK_BOOKED || status == BK_STAYING;
}

int nightsBetween(Date in, Date out) {
    return (int)(dateToDays(out) - dateToDays(in));
}

/* Hai khoang [in, out) giao nhau neu in1 < out2 va out1 > in2. */
int isDateRangeOverlap(Date newIn, Date newOut, Date oldIn, Date oldOut) {
    return compareDate(newIn, oldOut) < 0 && compareDate(newOut, oldIn) > 0;
}

int isRoomAvailable(const BookingNode *head, const char *roomId, Date in, Date out) {
    for (; head != NULL; head = head->next) {
        const Booking *b = &head->data;
        if (!isActive(b->status)) continue;
        if (!equalsIgnoreCase(b->roomId, roomId)) continue;
        if (isDateRangeOverlap(in, out, b->checkIn, b->checkOut)) return 0;
    }
    return 1;
}

int countAvailable(const RoomNode *p, Date in, Date out, const BookingNode *bk) {
    if (p == NULL) return 0;
    return (isRoomAvailable(bk, p->data.roomId, in, out) ? 1 : 0)
           + countAvailable(p->next, in, out, bk);
}

int listAvailableRooms(const RoomNode *rooms, const BookingNode *bookings, Date in, Date out) {
    int count = 0;
    printRoomHeader();
    for (; rooms != NULL; rooms = rooms->next) {
        if (isRoomAvailable(bookings, rooms->data.roomId, in, out)) {
            printRoomRow(&rooms->data);
            count++;
        }
    }
    printf("+------------+------+-----------------+----------------------------------------------------+\n");
    printf("Co %d phong trong.\n", count);
    return count;
}

int hasActiveBookingForRoom(const BookingNode *head, const char *roomId) {
    for (; head != NULL; head = head->next) {
        if (isActive(head->data.status) && equalsIgnoreCase(head->data.roomId, roomId)) return 1;
    }
    return 0;
}

int hasActiveBookingForGuest(const BookingNode *head, const char *guestId) {
    for (; head != NULL; head = head->next) {
        if (isActive(head->data.status) && equalsIgnoreCase(head->data.guestId, guestId)) return 1;
    }
    return 0;
}

BookingNode *findBookingById(BookingNode *head, int bookingId) {
    for (; head != NULL; head = head->next) {
        if (head->data.bookingId == bookingId) return head;
    }
    return NULL;
}

int nextBookingId(const BookingNode *head) {
    int maxId = 0;
    for (; head != NULL; head = head->next) {
        if (head->data.bookingId > maxId) maxId = head->data.bookingId;
    }
    return maxId + 1;
}

double calcAmount(const Booking *b, double pricePerNight, Date actualOut, Invoice *inv) {
    Invoice tmp;
    Invoice *v = (inv != NULL) ? inv : &tmp;
    int late;

    v->nights = nightsBetween(b->checkIn, b->checkOut);
    v->roomCharge = v->nights * pricePerNight;
    v->discount = (v->nights >= LONG_STAY_NIGHTS) ? v->roomCharge * LONG_STAY_DISCOUNT : 0.0;

    late = nightsBetween(b->checkOut, actualOut);
    v->lateDays = late > 0 ? late : 0;
    v->lateFee = v->lateDays * pricePerNight * (1.0 + LATE_FEE_RATE);

    v->total = v->roomCharge - v->discount + v->lateFee;
    return v->total;
}

int createBooking(BookingNode **head, RoomNode *rooms, GuestNode *guests,
                  const char *roomId, const char *guestId,
                  Date in, Date out, int *newId) {
    RoomNode *room;
    GuestNode *guest;
    BookingNode *node, *p;

    room = findRoomById(rooms, roomId);
    if (room == NULL) return BK_ERR_ROOM;
    guest = findGuestById(guests, guestId);
    if (guest == NULL) return BK_ERR_GUEST;
    if (!isValidDate(in) || !isValidDate(out) || compareDate(out, in) <= 0) return BK_ERR_DATE;
    if (nightsBetween(in, out) > MAX_NIGHTS) return BK_ERR_TOO_LONG;
    if (!isRoomAvailable(*head, room->data.roomId, in, out)) return BK_ERR_CONFLICT;

    node = (BookingNode *)malloc(sizeof(BookingNode));
    if (node == NULL) return BK_ERR_MEMORY;

    memset(&node->data, 0, sizeof(Booking));
    node->data.bookingId = nextBookingId(*head);
    strcpy(node->data.roomId, room->data.roomId);
    strcpy(node->data.guestId, guest->data.guestId);
    node->data.checkIn = in;
    node->data.checkOut = out;
    node->data.status = BK_BOOKED;
    node->data.totalAmount = calcAmount(&node->data, room->data.pricePerNight, out, NULL);
    node->next = NULL;

    if (*head == NULL) {
        *head = node;
    } else {
        for (p = *head; p->next != NULL; p = p->next) { }
        p->next = node;
    }
    if (newId != NULL) *newId = node->data.bookingId;
    return BK_OK;
}

int checkIn(BookingNode *head, int bookingId) {
    BookingNode *b = findBookingById(head, bookingId);
    if (b == NULL) return BK_ERR_NOT_FOUND;
    if (b->data.status != BK_BOOKED) return BK_ERR_STATUS;
    b->data.status = BK_STAYING;
    return BK_OK;
}

int checkOut(BookingNode *head, RoomNode *rooms, int bookingId, Date actualOut, Invoice *inv) {
    BookingNode *b = findBookingById(head, bookingId);
    RoomNode *room;
    if (b == NULL) return BK_ERR_NOT_FOUND;
    if (b->data.status != BK_STAYING) return BK_ERR_STATUS;
    if (!isValidDate(actualOut) || compareDate(actualOut, b->data.checkIn) < 0) return BK_ERR_DATE;
    room = findRoomById(rooms, b->data.roomId);
    if (room == NULL) return BK_ERR_ROOM;

    b->data.totalAmount = calcAmount(&b->data, room->data.pricePerNight, actualOut, inv);
    if (compareDate(actualOut, b->data.checkOut) > 0) b->data.checkOut = actualOut;
    b->data.status = BK_CHECKEDOUT;
    return BK_OK;
}

int cancelBooking(BookingNode *head, int bookingId) {
    BookingNode *b = findBookingById(head, bookingId);
    if (b == NULL) return BK_ERR_NOT_FOUND;
    if (b->data.status != BK_BOOKED) return BK_ERR_STATUS;
    b->data.status = BK_CANCELLED;
    b->data.totalAmount = 0.0;
    return BK_OK;
}

void printInvoice(const Booking *b, const Room *r, const Guest *g, const Invoice *inv) {
    char in[DATE_STR_LEN], out[DATE_STR_LEN];
    formatDate(b->checkIn, in, sizeof(in));
    formatDate(b->checkOut, out, sizeof(out));

    printf("\n==================== HOA DON THANH TOAN ====================\n");
    printf("  Ma phieu      : %d\n", b->bookingId);
    printf("  Khach hang    : %s (%s)\n", g != NULL ? g->fullName : "(da xoa)", b->guestId);
    if (g != NULL) printf("  CCCD / SDT    : %s / %s\n", g->idCard, g->phone);
    printf("  Phong         : %s - %s\n", b->roomId, r != NULL ? roomTypeName(r->type) : "?");
    printf("  Ngay nhan     : %s\n", in);
    printf("  Ngay tra      : %s\n", out);
    printf("------------------------------------------------------------\n");
    printf("  So dem dat    : %d x %.0f = %15.0f VND\n",
           inv->nights, r != NULL ? r->pricePerNight : 0.0, inv->roomCharge);
    if (inv->discount > 0)
        printf("  Giam gia %2.0f%% (>= %d dem)       : -%14.0f VND\n",
               LONG_STAY_DISCOUNT * 100, LONG_STAY_NIGHTS, inv->discount);
    if (inv->lateDays > 0)
        printf("  Tra muon %d ngay (phu thu %.0f%%)  : +%14.0f VND\n",
               inv->lateDays, LATE_FEE_RATE * 100, inv->lateFee);
    printf("------------------------------------------------------------\n");
    printf("  TONG CONG                       : %15.0f VND\n", inv->total);
    printf("============================================================\n");
}

void printBookingHeader(void) {
    printf("+-------+------------+-------------+------------+------------+-------+----------+-----------------+\n");
    printf("| %-5s | %-10s | %-11s | %-10s | %-10s | %5s | %-8s | %15s |\n",
           "Ma", "Phong", "Ma khach", "Ngay nhan", "Ngay tra", "Dem", "T.thai", "So tien (VND)");
    printf("+-------+------------+-------------+------------+------------+-------+----------+-----------------+\n");
}

void printBookingRow(const Booking *b) {
    char in[DATE_STR_LEN], out[DATE_STR_LEN];
    formatDate(b->checkIn, in, sizeof(in));
    formatDate(b->checkOut, out, sizeof(out));
    printf("| %5d | %-10s | %-11s | %-10s | %-10s | %5d | %-8s | %15.0f |\n",
           b->bookingId, b->roomId, b->guestId, in, out,
           nightsBetween(b->checkIn, b->checkOut), bookingStatusName(b->status), b->totalAmount);
}

void printBookingFooter(void) {
    printf("+-------+------------+-------------+------------+------------+-------+----------+-----------------+\n");
}

int printBookings(const BookingNode *head, int statusFilter) {
    int count = 0;
    printBookingHeader();
    for (; head != NULL; head = head->next) {
        if (statusFilter < 0 || head->data.status == statusFilter) {
            printBookingRow(&head->data);
            count++;
        }
    }
    printBookingFooter();
    printf("Tong: %d phieu.\n", count);
    return count;
}

/* In khi quay lui de phieu moi nhat hien truoc. */
int printHistoryReverse(const BookingNode *p, const char *guestId) {
    int count;
    if (p == NULL) return 0;
    count = printHistoryReverse(p->next, guestId);
    if (equalsIgnoreCase(p->data.guestId, guestId)) {
        printBookingRow(&p->data);
        count++;
    }
    return count;
}

void freeBookingList(BookingNode **head) {
    BookingNode *p = *head, *next;
    while (p != NULL) {
        next = p->next;
        free(p);
        p = next;
    }
    *head = NULL;
}
