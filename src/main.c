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

/* Tìm gần đúng theo mã phòng: nhập "2" sẽ ra mọi phòng có chữ "2" trong mã */
static void uiFindRoom(void) {
    char key[ROOM_ID_LEN];
    RoomNode *p;
    int count;

    inputString("Nhap ma phong (hoac mot phan ma): ", key, ROOM_ID_LEN, 0);
    count = printRoomsIf(g_rooms, filterRoomByIdContains, key);
    if (count == 0) {
        printf("[!] Khong co phong nao co ma chua \"%s\".\n", key);
        return;
    }
    for (p = g_rooms; p != NULL; p = p->next) {
        if (filterRoomByIdContains(&p->data, key)) {
            printf("  - %-10s: %s\n", p->data.roomId,
                   hasActiveBookingForRoom(g_bookings, p->data.roomId)
                       ? "dang co phieu dat / khach o" : "khong co phieu dat nao dang hieu luc");
        }
    }
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

/*
 * Nhập thông tin và thêm một khách mới. Mã khách được sinh tự động.
 * Trả về node vừa thêm, hoặc NULL nếu người dùng bỏ ngang / lỗi.
 */
static GuestNode *createGuestInteractive(void) {
    Guest g;
    GuestNode *other;
    int rc;

    memset(&g, 0, sizeof(g));
    generateGuestId(g_guests, g.guestId, GUEST_ID_LEN);
    printf("Ma khach (tu dong): %s\n", g.guestId);
    inputString("Ho ten: ", g.fullName, GUEST_NAME_LEN, 0);
    for (;;) {
        inputDigits("CCCD (chi gom chu so, toi da 12): ", g.idCard, ID_CARD_MIN_DIGITS, ID_CARD_MAX_DIGITS);
        other = findGuestByIdCard(g_guests, g.idCard);
        if (other == NULL) break;
        printf("[!] CCCD %s da duoc dang ky cho khach %s - %s.\n",
               g.idCard, other->data.guestId, other->data.fullName);
        if (!inputYesNo("Nhap CCCD khac?")) return NULL;
    }
    inputDigits("So dien thoai (chi gom chu so, toi da 10): ", g.phone, PHONE_MIN_DIGITS, PHONE_MAX_DIGITS);

    rc = addGuest(&g_guests, g);
    if (rc != LIST_OK) {
        printf("[!] %s\n", rc == LIST_DUPLICATE_ID_CARD ? "CCCD da ton tai."
                         : rc == LIST_DUPLICATE ? "Ma khach da ton tai." : "Khong du bo nho.");
        return NULL;
    }
    printf("Da them khach %s - %s.\n", g.guestId, g.fullName);
    autoSave();
    return findGuestById(g_guests, g.guestId);
}

static void uiAddGuest(void) {
    printTitle("THEM KHACH");
    createGuestInteractive();
}

/* In các khách khớp từ khóa kèm số thứ tự (STT), trả về số khách tìm thấy */
static int printGuestMatches(const char *key) {
    const GuestNode *p;
    int stt = 0;
    printf("+-----+-------------+--------------------------------+--------------+------------+\n");
    printf("| STT | %-11s | %-30s | %-12s | %-10s |\n", "Ma khach", "Ho ten", "CCCD", "SDT");
    printf("+-----+-------------+--------------------------------+--------------+------------+\n");
    for (p = g_guests; p != NULL; p = p->next) {
        if (guestMatches(&p->data, key)) {
            stt++;
            printf("| %3d | %-11s | %-30.30s | %-12s | %-10s |\n", stt,
                   p->data.guestId, p->data.fullName, p->data.idCard, p->data.phone);
        }
    }
    printf("+-----+-------------+--------------------------------+--------------+------------+\n");
    return stt;
}

static GuestNode *nthGuestMatch(const char *key, int n) {
    GuestNode *p;
    for (p = g_guests; p != NULL; p = p->next) {
        if (guestMatches(&p->data, key) && --n == 0) return p;
    }
    return NULL;
}

/*
 * Chọn khách mà không cần nhớ mã: tìm theo tên / CCCD / SĐT / mã (gần đúng),
 * rồi chọn theo STT. allowCreate = 1 thì cho phép thêm khách mới ngay tại đây.
 * Trả về NULL nếu người dùng hủy.
 */
static GuestNode *pickGuest(int allowCreate) {
    char key[GUEST_NAME_LEN];
    GuestNode *created;
    int count, choice, addOption;

    if (g_guests == NULL && !allowCreate) {
        printf("[!] Chua co khach nao.\n");
        return NULL;
    }
    for (;;) {
        if (g_guests == NULL) {
            printf("Chua co khach nao trong he thong.\n");
            count = 0;
            key[0] = '\0';
        } else {
            inputString("Tim khach (ten, CCCD, SDT hoac ma; Enter = xem tat ca): ", key, GUEST_NAME_LEN, 1);
            count = printGuestMatches(key);
            if (count == 0) printf("Khong tim thay khach phu hop voi \"%s\".\n", key);
        }

        addOption = allowCreate ? count + 1 : -2;
        if (count > 0) printf("  1..%d: chon khach theo STT\n", count);
        if (allowCreate) printf("  %d: them khach moi\n", addOption);
        printf("  0: tim lai\n -1: huy\n");
        choice = inputInt("Chon: ", -1, allowCreate ? addOption : count);

        if (choice == -1) return NULL;
        if (choice == 0) continue;
        if (choice <= count) return nthGuestMatch(key, choice);

        printTitle("THEM KHACH MOI");
        created = createGuestInteractive();
        if (created != NULL) return created;
    }
}

static void uiUpdateGuest(void) {
    char name[GUEST_NAME_LEN], idCard[ID_CARD_LEN], phone[PHONE_LEN];
    GuestNode *node, *other;

    printTitle("SUA THONG TIN KHACH");
    node = pickGuest(0);
    if (node == NULL) return;
    strcpy(name, node->data.fullName);
    strcpy(idCard, node->data.idCard);
    strcpy(phone, node->data.phone);

    printf("(Nhan Enter de giu nguyen gia tri trong ngoac)\n");
    inputStringOrKeep("Ho ten", name, GUEST_NAME_LEN);
    for (;;) {
        inputDigitsOrKeep("CCCD (chi gom chu so, toi da 12)", idCard, ID_CARD_MIN_DIGITS, ID_CARD_MAX_DIGITS);
        other = findGuestByIdCard(g_guests, idCard);
        if (other == NULL || other == node) break;
        printf("[!] CCCD nay da thuoc ve khach %s.\n", other->data.guestId);
        strcpy(idCard, node->data.idCard);
    }
    inputDigitsOrKeep("So dien thoai (chi gom chu so, toi da 10)", phone, PHONE_MIN_DIGITS, PHONE_MAX_DIGITS);

    updateGuest(&node->data, name, idCard, phone);
    printf("Da cap nhat khach %s.\n", node->data.guestId);
    autoSave();
}

static void uiDeleteGuest(void) {
    char id[GUEST_ID_LEN];
    GuestNode *node;
    printTitle("XOA KHACH");
    node = pickGuest(0);
    if (node == NULL) return;
    strcpy(id, node->data.guestId);  /* Sao chép trước vì node sẽ bị free */
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
    inputDigits("CCCD can tim: ", idCard, ID_CARD_MIN_DIGITS, ID_CARD_MAX_DIGITS);
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
    GuestNode *node;
    int count;
    node = pickGuest(0);
    if (node == NULL) return;
    printf("Lich su dat phong cua %s - %s (moi nhat truoc):\n", node->data.guestId, node->data.fullName);
    printBookingHeader();
    count = printHistoryReverse(g_bookings, node->data.guestId);
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

/*
 * Luồng đặt phòng: nhập ngày -> chọn phòng trống -> chọn khách (tìm theo
 * tên/CCCD/SĐT, chọn theo STT, hoặc thêm khách mới ngay) -> xác nhận.
 * Không bắt người dùng phải nhớ mã khách.
 */
static void uiCreateBooking(void) {
    char roomId[ROOM_ID_LEN];
    GuestNode *guest;
    RoomNode *room;
    Date in, out;
    int rc, newId, nights;
    Booking preview;
    Invoice inv;

    printTitle("DAT PHONG");
    if (g_rooms == NULL) {
        printf("[!] Can co it nhat mot phong truoc khi dat phong.\n");
        return;
    }

    /* Bước 1: khoảng ngày */
    inputDateRange(&in, &out);
    if (compareDate(in, getToday()) < 0 &&
        !inputYesNo("Ngay nhan phong da qua. Van tiep tuc?")) {
        return;
    }

    /* Bước 2: chọn phòng trong danh sách phòng trống */
    if (listAvailableRooms(g_rooms, g_bookings, in, out) == 0) {
        printf("[!] Khong con phong trong trong khoang ngay nay.\n");
        return;
    }
    for (;;) {
        inputCode("Chon ma phong: ", roomId, ROOM_ID_LEN);
        room = findRoomById(g_rooms, roomId);
        if (room == NULL) {
            printf("[!] Khong tim thay phong %s.\n", roomId);
        } else if (!isRoomAvailable(g_bookings, room->data.roomId, in, out)) {
            printf("[!] Phong %s da co nguoi dat trong khoang ngay nay.\n", room->data.roomId);
        } else {
            break;
        }
        if (!inputYesNo("Chon phong khac?")) return;
    }

    /* Bước 3: chọn khách */
    printf("\n--- Chon khach dat phong ---\n");
    guest = pickGuest(1);
    if (guest == NULL) {
        printf("Da huy dat phong.\n");
        return;
    }
    printf("Khach: %s - %s - CCCD %s\n", guest->data.guestId, guest->data.fullName, guest->data.idCard);

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

    rc = createBooking(&g_bookings, g_rooms, g_guests, room->data.roomId, guest->data.guestId,
                       in, out, &newId);
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
