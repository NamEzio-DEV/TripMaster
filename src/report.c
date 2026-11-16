#include <stdio.h>
#include "report.h"

/* De quy cong doanh thu theo tung phieu. */
double sumRevenue(const BookingNode *p, int month, int year) {
    double here = 0.0;
    if (p == NULL) return 0.0;
    if (p->data.status == BK_CHECKEDOUT &&
        p->data.checkOut.month == month && p->data.checkOut.year == year) {
        here = p->data.totalAmount;
    }
    return here + sumRevenue(p->next, month, year);
}

void revenueByMonth(const BookingNode *bookings, int year) {
    int m;
    double total = 0.0, v;
    printf("\nDOANH THU THEO THANG - NAM %d (tinh theo ngay tra phong)\n", year);
    printf("+-------+---------------------+\n");
    printf("| Thang |     Doanh thu (VND) |\n");
    printf("+-------+---------------------+\n");
    for (m = 1; m <= 12; m++) {
        v = sumRevenue(bookings, m, year);
        total += v;
        printf("| %5d | %19.0f |\n", m, v);
    }
    printf("+-------+---------------------+\n");
    printf("| Tong  | %19.0f |\n", total);
    printf("+-------+---------------------+\n");
}

void revenueByRoom(const RoomNode *rooms, const BookingNode *bookings) {
    const RoomNode *r;
    const BookingNode *b;
    double total = 0.0;

    printf("\nDOANH THU THEO PHONG (cac phieu da tra)\n");
    printf("+------------+------+----------+---------------------+\n");
    printf("| Ma phong   | Loai | So phieu |     Doanh thu (VND) |\n");
    printf("+------------+------+----------+---------------------+\n");
    for (r = rooms; r != NULL; r = r->next) {
        double sum = 0.0;
        int count = 0;
        for (b = bookings; b != NULL; b = b->next) {
            if (b->data.status == BK_CHECKEDOUT && equalsIgnoreCase(b->data.roomId, r->data.roomId)) {
                sum += b->data.totalAmount;
                count++;
            }
        }
        total += sum;
        printf("| %-10s | %-4s | %8d | %19.0f |\n", r->data.roomId, roomTypeName(r->data.type), count, sum);
    }
    printf("+------------+------+----------+---------------------+\n");
    printf("| %-10s |      |          | %19.0f |\n", "Tong", total);
    printf("+------------+------+----------+---------------------+\n");
}

void reportAvailableToday(const RoomNode *rooms, const BookingNode *bookings) {
    Date today = getToday();
    Date tomorrow = addDays(today, 1);
    char buf[DATE_STR_LEN];
    int total = countRooms(rooms);
    int freeRooms = countAvailable(rooms, today, tomorrow, bookings);

    formatDate(today, buf, sizeof(buf));
    printf("\nHom nay %s: %d / %d phong con trong (dem nay).\n", buf, freeRooms, total);
    if (freeRooms > 0) listAvailableRooms(rooms, bookings, today, tomorrow);
}

void listCurrentGuests(const BookingNode *bookings, GuestNode *guests) {
    const BookingNode *b;
    char in[DATE_STR_LEN], out[DATE_STR_LEN];
    int count = 0;

    printf("\nDANH SACH KHACH DANG O\n");
    printf("+------------+-------------+--------------------------------+--------------+------------+------------+\n");
    printf("| %-10s | %-11s | %-30s | %-12s | %-10s | %-10s |\n",
           "Phong", "Ma khach", "Ho ten", "CCCD", "Ngay nhan", "Ngay tra");
    printf("+------------+-------------+--------------------------------+--------------+------------+------------+\n");
    for (b = bookings; b != NULL; b = b->next) {
        GuestNode *g;
        if (b->data.status != BK_STAYING) continue;
        g = findGuestById(guests, b->data.guestId);
        formatDate(b->data.checkIn, in, sizeof(in));
        formatDate(b->data.checkOut, out, sizeof(out));
        printf("| %-10s | %-11s | %-30.30s | %-12s | %-10s | %-10s |\n",
               b->data.roomId, b->data.guestId,
               g != NULL ? g->data.fullName : "(khong ro)",
               g != NULL ? g->data.idCard : "",
               in, out);
        count++;
    }
    printf("+------------+-------------+--------------------------------+--------------+------------+------------+\n");
    printf("Tong: %d phong dang co khach.\n", count);
}
