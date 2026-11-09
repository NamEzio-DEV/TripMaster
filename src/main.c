/*
 * main.c - Chương trình Quản lý đặt phòng khách sạn.
 *
 * Cách dùng:  hotel [thu_muc_du_lieu]
 *   Không truyền tham số thì dùng thư mục mặc định DATA_DIR ("data/").
 *
 * main chỉ làm 4 việc: loadAll -> menuLoop -> saveAll -> giải phóng bộ nhớ.
 */
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "room.h"
#include "guest.h"
#include "booking.h"
#include "report.h"
#include "fileio.h"

/* Ba danh sách chính, chỉ dùng trong file này và truyền vào các module */
static RoomNode    *g_rooms = NULL;
static GuestNode   *g_guests = NULL;
static BookingNode *g_bookings = NULL;

/* Lưu ngay sau mỗi thao tác thêm/sửa/xóa để tắt ngang không mất dữ liệu */
static void autoSave(void) {
    if (!saveAll(g_rooms, g_guests, g_bookings)) {
        printf("[!] Luu du lieu that bai. Kiem tra thu muc %s\n", getDataDir());
    }
}

static void printTitle(const char *title) {
    printf("\n============================================================\n");
    printf("  %s\n", title);
    printf("============================================================\n");
}

/* ============================ PHÒNG ============================ */

static void uiAddRoom(void) {
    Room r;
    int rc;
    memset(&r, 0, sizeof(r));
    printTitle("THEM PHONG");
    inputCode("Ma phong (toi da 9 ky tu): ", r.roomId, ROOM_ID_LEN);
    if (findRoomById(g_rooms, r.roomId) != NULL) {
        printf("[!] Ma phong %s da ton tai.\n", r.roomId);
        return;
    }
    r.type = inputInt("Loai phong (1 = Don, 2 = Doi, 3 = VIP): ", ROOM_SINGLE, ROOM_VIP);
    r.pricePerNight = inputDouble("Gia mot dem (VND): ", MIN_ROOM_PRICE, MAX_ROOM_PRICE);
    inputString("Ghi chu (co the de trong): ", r.note, ROOM_NOTE_LEN, 1);

    rc = addRoom(&g_rooms, r);
    if (rc == LIST_OK) {
        printf("Da them phong %s.\n", r.roomId);
        autoSave();
    } else if (rc == LIST_DUPLICATE) {
        printf("[!] Ma phong %s da ton tai.\n", r.roomId);
    } else {
        printf("[!] Khong du bo nho.\n");
    }
}

static void uiUpdateRoom(void) {
    char id[ROOM_ID_LEN];
    RoomNode *node;
    int type;
    double price;
    char note[ROOM_NOTE_LEN];

    printTitle("SUA THONG TIN PHONG");
    inputCode("Ma phong can sua: ", id, ROOM_ID_LEN);
    node = findRoomById(g_rooms, id);
    if (node == NULL) {
        printf("[!] Khong tim thay phong %s.\n", id);
        return;
    }
    printf("(Nhan Enter de giu nguyen gia tri trong ngoac)\n");
    type = inputIntOrKeep("Loai phong (1 = Don, 2 = Doi, 3 = VIP)", ROOM_SINGLE, ROOM_VIP, node->data.type);
    price = inputDoubleOrKeep("Gia mot dem (VND)", MIN_ROOM_PRICE, MAX_ROOM_PRICE, node->data.pricePerNight);
    strcpy(note, node->data.note);
    inputStringOrKeep("Ghi chu", note, ROOM_NOTE_LEN);

    updateRoom(&node->data, type, price, note);
    printf("Da cap nhat phong %s.\n", node->data.roomId);
    autoSave();
}

static void uiDeleteRoom(void) {
    char id[ROOM_ID_LEN];
    printTitle("XOA PHONG");
    inputCode("Ma phong can xoa: ", id, ROOM_ID_LEN);
    if (findRoomById(g_rooms, id) == NULL) {
        printf("[!] Khong tim thay phong %s.\n", id);
        return;
    }
    if (hasActiveBookingForRoom(g_bookings, id)) {
        printf("[!] Khong the xoa: phong %s dang co phieu dat hoac khach dang o.\n", id);
        return;
    }
    if (!inputYesNo("Ban chac chan muon xoa?")) return;
    if (deleteRoom(&g_rooms, id) == LIST_OK) {
        printf("Da xoa phong %s.\n", id);
        autoSave();
    }
}

static void uiFindRoom(void) {
    char id[ROOM_ID_LEN];
    RoomNode *node;
    inputCode("Ma phong can tim: ", id, ROOM_ID_LEN);
    node = findRoomById(g_rooms, id);
    if (node == NULL) {
        printf("[!] Khong tim thay phong %s.\n", id);
        return;
    }
    printRoomHeader();
    printRoomRow(&node->data);
    printf("+------------+------+-----------------+----------------------------------------------------+\n");
    printf("Phong %s %s.\n", node->data.roomId,
           hasActiveBookingForRoom(g_bookings, node->data.roomId) ? "dang co phieu dat / khach o" : "khong co phieu dat nao dang hieu luc");
}

static void uiFilterRoomByType(void) {
    int type = inputInt("Loai phong (1 = Don, 2 = Doi, 3 = VIP): ", ROOM_SINGLE, ROOM_VIP);
    printRoomsIf(g_rooms, filterRoomByType, &type);
}

static void uiFilterRoomByPrice(void) {
    PriceRange range;
    range.minPrice = inputDouble("Gia tu (VND): ", 0, MAX_ROOM_PRICE);
    range.maxPrice = inputDouble("Gia den (VND): ", range.minPrice, MAX_ROOM_PRICE);
    printRoomsIf(g_rooms, filterRoomByPrice, &range);
}

static void uiSortRooms(void) {
    /* Bảng con trỏ hàm: chọn tiêu chí theo chỉ số */
    static const RoomCompareFn comparators[] = {
        cmpRoomById, cmpRoomByPriceAsc, cmpRoomByPriceDesc, cmpRoomByTypeThenPrice
    };
    int choice;
    printf("1. Theo ma phong\n2. Theo gia tang dan\n3. Theo gia giam dan\n4. Theo loai, roi theo gia\n");
    choice = inputInt("Chon tieu chi: ", 1, 4);
    sortRooms(g_rooms, comparators[choice - 1]);
    printRooms(g_rooms);
    autoSave();  /* Giữ thứ tự đã sắp xếp cho lần chạy sau */
}

static void menuRoom(void) {
    int choice;
    do {
        printTitle("QUAN LY PHONG");
        printf("1. Danh sach phong\n");
        printf("2. Them phong\n");
        printf("3. Sua phong\n");
        printf("4. Xoa phong\n");
        printf("5. Tim phong theo ma\n");
        printf("6. Loc phong theo loai\n");
        printf("7. Loc phong theo khoang gia\n");
        printf("8. Sap xep phong\n");
        printf("0. Quay lai\n");
        choice = inputInt("Chon: ", 0, 8);
        switch (choice) {
            case 1: printRooms(g_rooms); break;
            case 2: uiAddRoom(); break;
            case 3: uiUpdateRoom(); break;
            case 4: uiDeleteRoom(); break;
            case 5: uiFindRoom(); break;
            case 6: uiFilterRoomByType(); break;
            case 7: uiFilterRoomByPrice(); break;
            case 8: uiSortRooms(); break;
        }
        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

/* ============================ KHÁCH ============================ */

static void uiAddGuest(void) {
    Guest g;
    int rc;
    memset(&g, 0, sizeof(g));
    printTitle("THEM KHACH");
    inputCode("Ma khach (toi da 11 ky tu): ", g.guestId, GUEST_ID_LEN);
    if (findGuestById(g_guests, g.guestId) != NULL) {
        printf("[!] Ma khach %s da ton tai.\n", g.guestId);
        return;
    }
    inputString("Ho ten: ", g.fullName, GUEST_NAME_LEN, 0);
    inputDigits("CCCD (12 chu so): ", g.idCard, ID_CARD_DIGITS);
    if (findGuestByIdCard(g_guests, g.idCard) != NULL) {
        printf("[!] CCCD %s da duoc dang ky cho khach khac.\n", g.idCard);
        return;
    }
    inputDigits("So dien thoai (10 chu so): ", g.phone, PHONE_DIGITS);

    rc = addGuest(&g_guests, g);
    if (rc == LIST_OK) {
        printf("Da them khach %s.\n", g.guestId);
        autoSave();
    } else if (rc == LIST_DUPLICATE) {
        printf("[!] Ma khach da ton tai.\n");
    } else if (rc == LIST_DUPLICATE_ID_CARD) {
        printf("[!] CCCD da ton tai.\n");
    } else {
        printf("[!] Khong du bo nho.\n");
    }
}

static void uiUpdateGuest(void) {
    char id[GUEST_ID_LEN];
    char name[GUEST_NAME_LEN], idCard[ID_CARD_LEN], phone[PHONE_LEN];
    GuestNode *node, *other;

    printTitle("SUA THONG TIN KHACH");
    inputCode("Ma khach can sua: ", id, GUEST_ID_LEN);
    node = findGuestById(g_guests, id);
    if (node == NULL) {
        printf("[!] Khong tim thay khach %s.\n", id);
        return;
    }
    strcpy(name, node->data.fullName);
    strcpy(idCard, node->data.idCard);
    strcpy(phone, node->data.phone);

    printf("(Nhan Enter de giu nguyen gia tri trong ngoac)\n");
    inputStringOrKeep("Ho ten", name, GUEST_NAME_LEN);
    for (;;) {
        inputDigitsOrKeep("CCCD (12 chu so)", idCard, ID_CARD_DIGITS);
        other = findGuestByIdCard(g_guests, idCard);
        if (other == NULL || other == node) break;
        printf("[!] CCCD nay da thuoc ve khach %s.\n", other->data.guestId);
        strcpy(idCard, node->data.idCard);
    }
    inputDigitsOrKeep("So dien thoai (10 chu so)", phone, PHONE_DIGITS);

    updateGuest(&node->data, name, idCard, phone);
    printf("Da cap nhat khach %s.\n", node->data.guestId);
    autoSave();
}

static void uiDeleteGuest(void) {
    char id[GUEST_ID_LEN];
    printTitle("XOA KHACH");
    inputCode("Ma khach can xoa: ", id, GUEST_ID_LEN);
    if (findGuestById(g_guests, id) == NULL) {
        printf("[!] Khong tim thay khach %s.\n", id);
        return;
    }
    if (hasActiveBookingForGuest(g_bookings, id)) {
        printf("[!] Khong the xoa: khach %s dang co phieu dat hoac dang o.\n", id);
        return;
    }
    if (!inputYesNo("Ban chac chan muon xoa?")) return;
    if (deleteGuest(&g_guests, id) == LIST_OK) {
        printf("Da xoa khach %s.\n", id);
        autoSave();
    }
}

static void uiFindGuestByIdCard(void) {
    char idCard[ID_CARD_LEN];
    GuestNode *node;
    inputDigits("CCCD can tim (12 chu so): ", idCard, ID_CARD_DIGITS);
    node = findGuestByIdCard(g_guests, idCard);
    if (node == NULL) {
        printf("[!] Khong co khach nao co CCCD %s.\n", idCard);
        return;
    }
    printGuestHeader();
    printGuestRow(&node->data);
    printGuestFooter();
}

static void uiSearchGuestByName(void) {
    char key[GUEST_NAME_LEN];
    inputString("Nhap ten (hoac mot phan ten): ", key, GUEST_NAME_LEN, 0);
    searchGuestsByName(g_guests, key);
}

static void uiGuestHistory(void) {
    char id[GUEST_ID_LEN];
    int count;
    inputCode("Ma khach: ", id, GUEST_ID_LEN);
    if (findGuestById(g_guests, id) == NULL) {
        printf("[!] Khong tim thay khach %s (van hien thi lich su neu con).\n", id);
    }
    printf("Lich su dat phong cua %s (moi nhat truoc):\n", id);
    printBookingHeader();
    count = printHistoryReverse(g_bookings, id);
    printBookingFooter();
    printf("Tong: %d phieu.\n", count);
}

static void menuGuest(void) {
    int choice;
    do {
        printTitle("QUAN LY KHACH");
        printf("1. Danh sach khach\n");
        printf("2. Them khach\n");
        printf("3. Sua khach\n");
        printf("4. Xoa khach\n");
        printf("5. Tim khach theo CCCD\n");
        printf("6. Tim khach theo ten\n");
        printf("7. Lich su dat phong cua khach\n");
        printf("0. Quay lai\n");
        choice = inputInt("Chon: ", 0, 7);
        switch (choice) {
            case 1: printGuests(g_guests); break;
            case 2: uiAddGuest(); break;
            case 3: uiUpdateGuest(); break;
            case 4: uiDeleteGuest(); break;
            case 5: uiFindGuestByIdCard(); break;
            case 6: uiSearchGuestByName(); break;
            case 7: uiGuestHistory(); break;
        }
        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

/* ========================= ĐẶT PHÒNG ========================= */

/* Nhập cặp ngày [nhận, trả) và kiểm tra ngày trả > ngày nhận */
static void inputDateRange(Date *in, Date *out) {
    for (;;) {
        *in = inputDate("Ngay nhan phong (dd/mm/yyyy): ");
        *out = inputDate("Ngay tra phong  (dd/mm/yyyy): ");
        if (compareDate(*out, *in) <= 0) {
            printf("  [!] Ngay tra phai sau ngay nhan. Nhap lai.\n");
            continue;
        }
        if (nightsBetween(*in, *out) > MAX_NIGHTS) {
            printf("  [!] Toi da %d dem cho mot lan dat. Nhap lai.\n", MAX_NIGHTS);
            continue;
        }
        return;
    }
}

static void uiCheckAvailability(void) {
    Date in, out;
    printTitle("KIEM TRA PHONG TRONG");
    inputDateRange(&in, &out);
    printf("So dem: %d\n", nightsBetween(in, out));
    listAvailableRooms(g_rooms, g_bookings, in, out);
}

static void uiCreateBooking(void) {
    char guestId[GUEST_ID_LEN], roomId[ROOM_ID_LEN];
    GuestNode *guest;
    RoomNode *room;
    Date in, out;
    int rc, newId, nights;
    Booking preview;
    Invoice inv;

    printTitle("DAT PHONG");
    if (g_rooms == NULL || g_guests == NULL) {
        printf("[!] Can co it nhat mot phong va mot khach truoc khi dat phong.\n");
        return;
    }

    inputCode("Ma khach: ", guestId, GUEST_ID_LEN);
    guest = findGuestById(g_guests, guestId);
    if (guest == NULL) {
        printf("[!] Khong tim thay khach %s. Hay them khach truoc.\n", guestId);
        return;
    }
    printf("Khach: %s - CCCD %s\n", guest->data.fullName, guest->data.idCard);

    inputDateRange(&in, &out);
    if (compareDate(in, getToday()) < 0 &&
        !inputYesNo("Ngay nhan phong da qua. Van tiep tuc?")) {
        return;
    }

    if (listAvailableRooms(g_rooms, g_bookings, in, out) == 0) {
        printf("[!] Khong con phong trong trong khoang ngay nay.\n");
        return;
    }
    inputCode("Chon ma phong: ", roomId, ROOM_ID_LEN);
    room = findRoomById(g_rooms, roomId);
    if (room == NULL) {
        printf("[!] Khong tim thay phong %s.\n", roomId);
        return;
    }

    /* Tạm tính để khách xác nhận */
    memset(&preview, 0, sizeof(preview));
    preview.checkIn = in;
    preview.checkOut = out;
    calcAmount(&preview, room->data.pricePerNight, out, &inv);
    nights = inv.nights;
    printf("Phong %s (%s), %d dem x %.0f VND", room->data.roomId, roomTypeName(room->data.type),
           nights, room->data.pricePerNight);
    if (inv.discount > 0) printf(", giam %.0f VND", inv.discount);
    printf(" => Tam tinh: %.0f VND\n", inv.total);
    if (!inputYesNo("Xac nhan dat phong?")) return;

    rc = createBooking(&g_bookings, g_rooms, g_guests, roomId, guestId, in, out, &newId);
    if (rc == BK_OK) {
        printf("Dat phong thanh cong. Ma phieu: %d\n", newId);
        autoSave();
    } else {
        printf("[!] %s\n", bookingErrorMessage(rc));
    }
}

static void uiCheckIn(void) {
    int id, rc;
    BookingNode *b;
    Date today = getToday();

    printTitle("NHAN PHONG (CHECK-IN)");
    if (printBookings(g_bookings, BK_BOOKED) == 0) return;
    id = inputInt("Ma phieu: ", 1, 2147483647);
    b = findBookingById(g_bookings, id);
    if (b == NULL) {
        printf("[!] %s\n", bookingErrorMessage(BK_ERR_NOT_FOUND));
        return;
    }
    if (b->data.status == BK_BOOKED) {
        if (compareDate(today, b->data.checkIn) < 0 &&
            !inputYesNo("Chua den ngay nhan phong. Van nhan phong som?")) {
            return;
        }
        if (compareDate(today, b->data.checkOut) >= 0 &&
            !inputYesNo("Da qua ngay tra phong cua phieu. Van nhan phong?")) {
            return;
        }
    }
    rc = checkIn(g_bookings, id);
    if (rc == BK_OK) {
        printf("Phieu %d: khach da nhan phong %s.\n", id, b->data.roomId);
        autoSave();
    } else {
        printf("[!] %s (trang thai hien tai: %s)\n", bookingErrorMessage(rc), bookingStatusName(b->data.status));
    }
}

static void uiCheckOut(void) {
    int id, rc;
    BookingNode *b;
    RoomNode *room;
    GuestNode *guest;
    Date actual;
    Invoice inv;

    printTitle("TRA PHONG (CHECK-OUT)");
    if (printBookings(g_bookings, BK_STAYING) == 0) return;
    id = inputInt("Ma phieu: ", 1, 2147483647);
    b = findBookingById(g_bookings, id);
    if (b == NULL) {
        printf("[!] %s\n", bookingErrorMessage(BK_ERR_NOT_FOUND));
        return;
    }
    if (b->data.status != BK_STAYING) {
        printf("[!] %s (trang thai hien tai: %s)\n", bookingErrorMessage(BK_ERR_STATUS),
               bookingStatusName(b->data.status));
        return;
    }
    actual = inputDateOrKeep("Ngay tra thuc te", getToday());
    while (compareDate(actual, b->data.checkIn) < 0) {
        printf("  [!] Ngay tra khong duoc truoc ngay nhan phong.\n");
        actual = inputDateOrKeep("Ngay tra thuc te", b->data.checkOut);
    }
    /* Trả muộn: kiểm tra khoảng ở thêm [ngày trả dự kiến, ngày trả thực tế)
     * có đè lên phiếu đặt khác của cùng phòng hay không */
    if (compareDate(actual, b->data.checkOut) > 0 &&
        !isRoomAvailable(g_bookings, b->data.roomId, b->data.checkOut, actual)) {
        printf("[!] Canh bao: khoang tra muon trung voi phieu dat khac cua phong %s.\n", b->data.roomId);
        if (!inputYesNo("Van tra phong voi ngay nay?")) return;
    }

    rc = checkOut(g_bookings, g_rooms, id, actual, &inv);
    if (rc != BK_OK) {
        printf("[!] %s\n", bookingErrorMessage(rc));
        return;
    }
    room = findRoomById(g_rooms, b->data.roomId);
    guest = findGuestById(g_guests, b->data.guestId);
    printInvoice(&b->data, room != NULL ? &room->data : NULL, guest != NULL ? &guest->data : NULL, &inv);
    autoSave();
}

static void uiCancelBooking(void) {
    int id, rc;
    BookingNode *b;
    printTitle("HUY DAT PHONG");
    if (printBookings(g_bookings, BK_BOOKED) == 0) return;
    id = inputInt("Ma phieu can huy: ", 1, 2147483647);
    b = findBookingById(g_bookings, id);
    if (b == NULL) {
        printf("[!] %s\n", bookingErrorMessage(BK_ERR_NOT_FOUND));
        return;
    }
    if (b->data.status != BK_BOOKED) {
        printf("[!] Chi huy duoc phieu o trang thai 'Da dat' (hien tai: %s).\n",
               bookingStatusName(b->data.status));
        return;
    }
    if (!inputYesNo("Ban chac chan muon huy phieu nay?")) return;
    rc = cancelBooking(g_bookings, id);
    if (rc == BK_OK) {
        printf("Da huy phieu %d.\n", id);
        autoSave();
    } else {
        printf("[!] %s\n", bookingErrorMessage(rc));
    }
}

static void uiListBookings(void) {
    int f;
    printf("Loc theo trang thai: -1 = Tat ca, 0 = Da dat, 1 = Dang o, 2 = Da tra, 3 = Huy\n");
    f = inputInt("Chon: ", -1, 3);
    printBookings(g_bookings, f);
}

static void menuBooking(void) {
    int choice;
    do {
        printTitle("DAT PHONG / TRA PHONG");
        printf("1. Danh sach phieu dat\n");
        printf("2. Kiem tra phong trong theo khoang ngay\n");
        printf("3. Dat phong\n");
        printf("4. Nhan phong (check-in)\n");
        printf("5. Tra phong (check-out, in hoa don)\n");
        printf("6. Huy dat phong\n");
        printf("0. Quay lai\n");
        choice = inputInt("Chon: ", 0, 6);
        switch (choice) {
            case 1: uiListBookings(); break;
            case 2: uiCheckAvailability(); break;
            case 3: uiCreateBooking(); break;
            case 4: uiCheckIn(); break;
            case 5: uiCheckOut(); break;
            case 6: uiCancelBooking(); break;
        }
        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

/* ========================== THỐNG KÊ ========================== */

static void menuReport(void) {
    int choice, year;
    do {
        printTitle("THONG KE");
        printf("1. Doanh thu theo thang\n");
        printf("2. Doanh thu theo phong\n");
        printf("3. So phong trong hom nay\n");
        printf("4. Danh sach khach dang o\n");
        printf("0. Quay lai\n");
        choice = inputInt("Chon: ", 0, 4);
        switch (choice) {
            case 1:
                year = inputIntOrKeep("Nam", MIN_YEAR, MAX_YEAR, getToday().year);
                revenueByMonth(g_bookings, year);
                break;
            case 2: revenueByRoom(g_rooms, g_bookings); break;
            case 3: reportAvailableToday(g_rooms, g_bookings); break;
            case 4: listCurrentGuests(g_bookings, g_guests); break;
        }
        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

/* ========================== MENU CHÍNH ========================== */

static void menuLoop(void) {
    int choice;
    char today[DATE_STR_LEN];
    do {
        formatDate(getToday(), today, sizeof(today));
        printTitle("QUAN LY DAT PHONG KHACH SAN");
        printf("  Hom nay: %s | %d phong | %d khach\n", today, countRooms(g_rooms), countGuests(g_guests));
        printf("------------------------------------------------------------\n");
        printf("1. Quan ly phong\n");
        printf("2. Quan ly khach\n");
        printf("3. Dat phong / Tra phong\n");
        printf("4. Thong ke\n");
        printf("5. Luu du lieu\n");
        printf("0. Thoat\n");
        choice = inputInt("Chon: ", 0, 5);
        switch (choice) {
            case 1: menuRoom(); break;
            case 2: menuGuest(); break;
            case 3: menuBooking(); break;
            case 4: menuReport(); break;
            case 5:
                if (saveAll(g_rooms, g_guests, g_bookings)) printf("Da luu du lieu vao %s\n", getDataDir());
                pauseScreen();
                break;
        }
    } while (choice != 0);
}

int main(int argc, char *argv[]) {
    if (argc >= 2) setDataDir(argv[1]);

    loadAll(&g_rooms, &g_guests, &g_bookings);
    menuLoop();

    if (saveAll(g_rooms, g_guests, g_bookings)) {
        printf("Da luu du lieu. Tam biet!\n");
    }
    freeRoomList(&g_rooms);
    freeGuestList(&g_guests);
    freeBookingList(&g_bookings);
    return 0;
}
