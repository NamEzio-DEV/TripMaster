#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "room.h"
#include "guest.h"
#include "booking.h"
#include "report.h"
#include "fileio.h"

static RoomNode    *g_rooms = NULL;    // Dau danh sach lien ket chua tat ca phong trong RAM.
static GuestNode   *g_guests = NULL;   // Dau danh sach lien ket chua tat ca khach trong RAM.
static BookingNode *g_bookings = NULL; // Dau danh sach lien ket chua tat ca phieu dat trong RAM.

/* Luu ca ba danh sach sau thao tac thay doi va bao loi neu khong ghi duoc. */
static void autoSave(void) {
    // Kiểm tra điều kiện [!saveAll(g_rooms, g_guests, g_bookings)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (!saveAll(g_rooms, g_guests, g_bookings)) {
        // Hiển thị nội dung [[!] Luu du lieu that bai. Kiem tra thu muc %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Luu du lieu that bai. Kiem tra thu muc %s\n", getDataDir());
    }
}

static void printTitle(const char *title) {
    // Hiển thị nội dung [============================================================] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("\n============================================================\n");
    // Hiển thị nội dung [  %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("  %s\n", title);
    // Hiển thị nội dung [============================================================] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("============================================================\n");
}

/* Hoi thong tin phong, kiem tra ma trung, them vao danh sach va tu dong luu. */
static void uiAddRoom(void) {
    // Khai báo biến [r] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Room r;
    // Khai báo biến [rc] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int rc;
    // Đặt toàn bộ byte của cấu trúc về 0 để bắt đầu với dữ liệu sạch.
    memset(&r, 0, sizeof(r));
    // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
    printTitle("THEM PHONG");
    // Gọi hàm [inputCode] để đọc mã từ bàn phím, kiểm tra độ dài và chuẩn hóa chữ hoa.
    inputCode("Ma phong (toi da 9 ky tu): ", r.roomId, ROOM_ID_LEN);
    // Kiểm tra điều kiện [findRoomById(g_rooms, r.roomId) != NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (findRoomById(g_rooms, r.roomId) != NULL) {
        // Hiển thị nội dung [[!] Ma phong %s da ton tai.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Ma phong %s da ton tai.\n", r.roomId);
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Ghi [số người dùng nhập sau khi được kiểm tra khoảng hợp lệ] vào trường [r.type] để lưu thông tin của bản ghi.
    r.type = inputInt("Loai phong (1 = Don, 2 = Doi, 3 = VIP): ", ROOM_SINGLE, ROOM_VIP);
    // Ghi [giá người dùng nhập sau khi được kiểm tra khoảng hợp lệ] vào trường [r.pricePerNight] để lưu thông tin của bản ghi.
    r.pricePerNight = inputDouble("Gia mot dem (VND): ", MIN_ROOM_PRICE, MAX_ROOM_PRICE);
    // Gọi hàm [inputString] để đọc chuỗi, kiểm tra độ dài và cho phép/ràng buộc chuỗi rỗng theo tham số cuối.
    inputString("Ghi chu (co the de trong): ", r.note, ROOM_NOTE_LEN, 1);

    // Lưu [mã kết quả thêm phòng: thành công, trùng mã hoặc hết bộ nhớ] vào biến [rc] để các lệnh sau sử dụng.
    rc = addRoom(&g_rooms, r);
    // Kiểm tra điều kiện [rc == LIST_OK]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (rc == LIST_OK) {
        // Hiển thị nội dung [Da them phong %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("Da them phong %s.\n", r.roomId);
        // Gọi hàm [autoSave] để ghi các danh sách hiện tại xuống những tệp dữ liệu.
        autoSave();
    } else if (rc == LIST_DUPLICATE) {
        // Hiển thị nội dung [[!] Ma phong %s da ton tai.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Ma phong %s da ton tai.\n", r.roomId);
    } else {
        // Hiển thị nội dung [[!] Khong du bo nho.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Khong du bo nho.\n");
    }
}

/* Tim phong theo ma; Enter giu gia tri cu, cac gia tri moi duoc cap nhat va luu. */
static void uiUpdateRoom(void) {
    // Khai báo biến [id] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char id[ROOM_ID_LEN];
    RoomNode *node;
    // Khai báo biến [type] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int type;
    // Khai báo biến [price] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    double price;
    // Khai báo biến [note] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char note[ROOM_NOTE_LEN];

    // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
    printTitle("SUA THONG TIN PHONG");
    // Gọi hàm [inputCode] để đọc mã từ bàn phím, kiểm tra độ dài và chuẩn hóa chữ hoa.
    inputCode("Ma phong can sua: ", id, ROOM_ID_LEN);
    // Lưu [nút phòng có mã cần tìm, hoặc NULL nếu không tồn tại] vào biến [node] để các lệnh sau sử dụng.
    node = findRoomById(g_rooms, id);
    // Kiểm tra điều kiện [node == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (node == NULL) {
        // Hiển thị nội dung [[!] Khong tim thay phong %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Khong tim thay phong %s.\n", id);
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Hiển thị nội dung [(Nhan Enter de giu nguyen gia tri trong ngoac)] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("(Nhan Enter de giu nguyen gia tri trong ngoac)\n");
    // Lưu [giá trị biểu thức [inputIntOrKeep("Loai phong (1 = Don, 2 = Doi, 3 = VIP)", ROOM_SINGLE, ROOM_VIP, node->data.type)]] vào biến [type] để các lệnh sau sử dụng.
    type = inputIntOrKeep("Loai phong (1 = Don, 2 = Doi, 3 = VIP)", ROOM_SINGLE, ROOM_VIP, node->data.type);
    // Lưu [giá trị biểu thức [inputDoubleOrKeep("Gia mot dem (VND)", MIN_ROOM_PRICE, MAX_ROOM_PRICE, node->data.pricePerNight)]] vào biến [price] để các lệnh sau sử dụng.
    price = inputDoubleOrKeep("Gia mot dem (VND)", MIN_ROOM_PRICE, MAX_ROOM_PRICE, node->data.pricePerNight);
    // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
    strcpy(note, node->data.note);
    // Gọi hàm [inputStringOrKeep] để cho sửa chuỗi; nhấn Enter thì giữ nguyên giá trị hiện tại.
    inputStringOrKeep("Ghi chu", note, ROOM_NOTE_LEN);

    // Gọi hàm [updateRoom] để chép loại, giá và ghi chú mới vào bản ghi phòng.
    updateRoom(&node->data, type, price, note);
    // Hiển thị nội dung [Da cap nhat phong %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Da cap nhat phong %s.\n", node->data.roomId);
    // Gọi hàm [autoSave] để ghi các danh sách hiện tại xuống những tệp dữ liệu.
    autoSave();
}

/* Chi xoa phong khi khong co phieu dang dat/dang o va nguoi dung xac nhan. */
static void uiDeleteRoom(void) {
    // Khai báo biến [id] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char id[ROOM_ID_LEN];
    // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
    printTitle("XOA PHONG");
    // Gọi hàm [inputCode] để đọc mã từ bàn phím, kiểm tra độ dài và chuẩn hóa chữ hoa.
    inputCode("Ma phong can xoa: ", id, ROOM_ID_LEN);
    // Kiểm tra điều kiện [findRoomById(g_rooms, id) == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (findRoomById(g_rooms, id) == NULL) {
        // Hiển thị nội dung [[!] Khong tim thay phong %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Khong tim thay phong %s.\n", id);
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Kiểm tra điều kiện [hasActiveBookingForRoom(g_bookings, id)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (hasActiveBookingForRoom(g_bookings, id)) {
        // Hiển thị nội dung [[!] Khong the xoa: phong %s dang co phieu dat hoac khach dang o.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Khong the xoa: phong %s dang co phieu dat hoac khach dang o.\n", id);
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Kiểm tra điều kiện [!inputYesNo("Ban chac chan muon xoa?")]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (!inputYesNo("Ban chac chan muon xoa?")) return;
    // Kiểm tra điều kiện [deleteRoom(&g_rooms, id) == LIST_OK]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (deleteRoom(&g_rooms, id) == LIST_OK) {
        // Hiển thị nội dung [Da xoa phong %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("Da xoa phong %s.\n", id);
        // Gọi hàm [autoSave] để ghi các danh sách hiện tại xuống những tệp dữ liệu.
        autoSave();
    }
}

static void uiFindRoom(void) {
    // Khai báo biến [key] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char key[ROOM_ID_LEN];
    RoomNode *p;
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count;

    // Gọi hàm [inputString] để đọc chuỗi, kiểm tra độ dài và cho phép/ràng buộc chuỗi rỗng theo tham số cuối.
    inputString("Nhap ma phong (hoac mot phan ma): ", key, ROOM_ID_LEN, 0);
    // Lưu [giá trị biểu thức [printRoomsIf(g_rooms, filterRoomByIdContains, key)]] vào biến [count] để các lệnh sau sử dụng.
    count = printRoomsIf(g_rooms, filterRoomByIdContains, key);
    // Kiểm tra điều kiện [count == 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (count == 0) {
        // Hiển thị nội dung [[!] Khong co phong nao co ma chua \] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Khong co phong nao co ma chua \"%s\".\n", key);
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Lặp theo ba phần [p = g_rooms; p != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (p = g_rooms; p != NULL; p = p->next) {
        // Kiểm tra điều kiện [filterRoomByIdContains(&p->data, key)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (filterRoomByIdContains(&p->data, key)) {
            // Hiển thị nội dung [  - %-10s: %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  - %-10s: %s\n", p->data.roomId,
                   // Gọi hàm [hasActiveBookingForRoom] để kiểm tra phòng có phiếu đang đặt hoặc khách đang ở không.
                   hasActiveBookingForRoom(g_bookings, p->data.roomId)
                       ? "dang co phieu dat / khach o" : "khong co phieu dat nao dang hieu luc");
        }
    }
}

static void uiFilterRoomByType(void) {
    // Khai báo biến [type] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int type = inputInt("Loai phong (1 = Don, 2 = Doi, 3 = VIP): ", ROOM_SINGLE, ROOM_VIP);
    // Gọi hàm [printRoomsIf] để lọc danh sách phòng theo điều kiện rồi in các phòng phù hợp.
    printRoomsIf(g_rooms, filterRoomByType, &type);
}

static void uiFilterRoomByPrice(void) {
    PriceRange range;
    // Ghi [giá người dùng nhập sau khi được kiểm tra khoảng hợp lệ] vào trường [range.minPrice] để lưu thông tin của bản ghi.
    range.minPrice = inputDouble("Gia tu (VND): ", 0, MAX_ROOM_PRICE);
    // Ghi [giá người dùng nhập sau khi được kiểm tra khoảng hợp lệ] vào trường [range.maxPrice] để lưu thông tin của bản ghi.
    range.maxPrice = inputDouble("Gia den (VND): ", range.minPrice, MAX_ROOM_PRICE);
    // Gọi hàm [printRoomsIf] để lọc danh sách phòng theo điều kiện rồi in các phòng phù hợp.
    printRoomsIf(g_rooms, filterRoomByPrice, &range);
}

static void uiSortRooms(void) {

    static const RoomCompareFn comparators[] = {
        cmpRoomById, cmpRoomByPriceAsc, cmpRoomByPriceDesc, cmpRoomByTypeThenPrice
    };
    // Khai báo biến [choice] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int choice;
    // Hiển thị nội dung [1. Theo ma phong2. Theo gia tang dan3. Theo gia giam dan4. Theo loai, roi theo gia] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("1. Theo ma phong\n2. Theo gia tang dan\n3. Theo gia giam dan\n4. Theo loai, roi theo gia\n");
    // Lưu [số nguyên người dùng nhập sau khi kiểm tra giới hạn] vào biến [choice] để các lệnh sau sử dụng.
    choice = inputInt("Chon tieu chi: ", 1, 4);
    // Gọi hàm [sortRooms] để sắp xếp các nút phòng theo hàm so sánh đã chọn.
    sortRooms(g_rooms, comparators[choice - 1]);
    // Gọi hàm [printRooms] để in các phòng trong danh sách.
    printRooms(g_rooms);
    // Gọi hàm [autoSave] để ghi các danh sách hiện tại xuống những tệp dữ liệu.
    autoSave();
}

static void menuRoom(void) {
    // Khai báo biến [choice] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int choice;
    do {
        // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
        printTitle("QUAN LY PHONG");
        // Hiển thị nội dung [1. Danh sach phong] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("1. Danh sach phong\n");
        // Hiển thị nội dung [2. Them phong] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("2. Them phong\n");
        // Hiển thị nội dung [3. Sua phong] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("3. Sua phong\n");
        // Hiển thị nội dung [4. Xoa phong] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("4. Xoa phong\n");
        // Hiển thị nội dung [5. Tim phong theo ma] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("5. Tim phong theo ma\n");
        // Hiển thị nội dung [6. Loc phong theo loai] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("6. Loc phong theo loai\n");
        // Hiển thị nội dung [7. Loc phong theo khoang gia] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("7. Loc phong theo khoang gia\n");
        // Hiển thị nội dung [8. Sap xep phong] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("8. Sap xep phong\n");
        // Hiển thị nội dung [0. Quay lai] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("0. Quay lai\n");
        // Lưu [số nguyên người dùng nhập sau khi kiểm tra giới hạn] vào biến [choice] để các lệnh sau sử dụng.
        choice = inputInt("Chon: ", 0, 8);
        // Đọc giá trị [choice] để chọn một nhánh case tương ứng bên dưới.
        switch (choice) {
            // Đây là nhánh xử lý khi giá trị switch bằng [1].
            case 1: printRooms(g_rooms); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [2].
            case 2: uiAddRoom(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [3].
            case 3: uiUpdateRoom(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [4].
            case 4: uiDeleteRoom(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [5].
            case 5: uiFindRoom(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [6].
            case 6: uiFilterRoomByType(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [7].
            case 7: uiFilterRoomByPrice(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [8].
            case 8: uiSortRooms(); break;
        }
        // Kiểm tra điều kiện [choice != 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

/* Nhap va kiem tra thong tin khach, tu sinh ma, them khach moi va luu du lieu. */
static GuestNode *createGuestInteractive(void) {
    // Khai báo biến [g] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Guest g;
    GuestNode *other;
    // Khai báo biến [rc] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int rc;

    // Đặt toàn bộ byte của cấu trúc về 0 để bắt đầu với dữ liệu sạch.
    memset(&g, 0, sizeof(g));
    // Gọi hàm [generateGuestId] để tạo mã khách kế tiếp chưa bị sử dụng.
    generateGuestId(g_guests, g.guestId, GUEST_ID_LEN);
    // Hiển thị nội dung [Ma khach (tu dong): %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Ma khach (tu dong): %s\n", g.guestId);
    // Gọi hàm [inputString] để đọc chuỗi, kiểm tra độ dài và cho phép/ràng buộc chuỗi rỗng theo tham số cuối.
    inputString("Ho ten: ", g.fullName, GUEST_NAME_LEN, 0);
    // Lặp cho đến khi một nhánh bên trong chọn phòng hợp lệ hoặc người dùng hủy.
    for (;;) {
        // Gọi hàm [inputDigits] để đọc chuỗi chỉ gồm chữ số và giới hạn độ dài.
        inputDigits("CCCD (chi gom chu so, toi da 12): ", g.idCard, ID_CARD_MIN_DIGITS, ID_CARD_MAX_DIGITS);
        // Lưu [giá trị biểu thức [findGuestByIdCard(g_guests, g.idCard)]] vào biến [other] để các lệnh sau sử dụng.
        other = findGuestByIdCard(g_guests, g.idCard);
        // Kiểm tra điều kiện [other == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (other == NULL) break;
        // Hiển thị nội dung [[!] CCCD %s da duoc dang ky cho khach %s - %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] CCCD %s da duoc dang ky cho khach %s - %s.\n",
               g.idCard, other->data.guestId, other->data.fullName);
        // Kiểm tra điều kiện [!inputYesNo("Nhap CCCD khac?")]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!inputYesNo("Nhap CCCD khac?")) return NULL;
    }
    // Gọi hàm [inputDigits] để đọc chuỗi chỉ gồm chữ số và giới hạn độ dài.
    inputDigits("So dien thoai (chi gom chu so, toi da 10): ", g.phone, PHONE_MIN_DIGITS, PHONE_MAX_DIGITS);

    // Lưu [mã kết quả thêm khách] vào biến [rc] để các lệnh sau sử dụng.
    rc = addGuest(&g_guests, g);
    // Kiểm tra điều kiện [rc != LIST_OK]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (rc != LIST_OK) {
        // Hiển thị nội dung [[!] %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] %s\n", rc == LIST_DUPLICATE_ID_CARD ? "CCCD da ton tai."
                         : rc == LIST_DUPLICATE ? "Ma khach da ton tai." : "Khong du bo nho.");
        // Báo rằng không tìm thấy dữ liệu hoặc không thể trả về một đối tượng.
        return NULL;
    }
    // Hiển thị nội dung [Da them khach %s - %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Da them khach %s - %s.\n", g.guestId, g.fullName);
    // Gọi hàm [autoSave] để ghi các danh sách hiện tại xuống những tệp dữ liệu.
    autoSave();
    // Trả giá trị [findGuestById(g_guests, g.guestId)] về cho hàm đã gọi dòng này.
    return findGuestById(g_guests, g.guestId);
}

static void uiAddGuest(void) {
    // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
    printTitle("THEM KHACH");
    // Gọi hàm [createGuestInteractive] để hỏi người dùng thông tin và thêm một khách mới.
    createGuestInteractive();
}

static int printGuestMatches(const char *key) {
    const GuestNode *p;
    // Khai báo biến [stt] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int stt = 0;
    // Hiển thị nội dung [+-----+-------------+--------------------------------+--------------+------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-----+-------------+--------------------------------+--------------+------------+\n");
    // Hiển thị nội dung [| STT | %-11s | %-30s | %-12s | %-10s |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("| STT | %-11s | %-30s | %-12s | %-10s |\n", "Ma khach", "Ho ten", "CCCD", "SDT");
    // Hiển thị nội dung [+-----+-------------+--------------------------------+--------------+------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-----+-------------+--------------------------------+--------------+------------+\n");
    // Lặp theo ba phần [p = g_guests; p != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (p = g_guests; p != NULL; p = p->next) {
        // Kiểm tra điều kiện [guestMatches(&p->data, key)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (guestMatches(&p->data, key)) {
            // Cập nhật biến [stt] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
            stt++;
            // Hiển thị nội dung [| %3d | %-11s | %-30.30s | %-12s | %-10s |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("| %3d | %-11s | %-30.30s | %-12s | %-10s |\n", stt,
                   p->data.guestId, p->data.fullName, p->data.idCard, p->data.phone);
        }
    }
    // Hiển thị nội dung [+-----+-------------+--------------------------------+--------------+------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-----+-------------+--------------------------------+--------------+------------+\n");
    // Trả giá trị [stt] về cho hàm đã gọi dòng này.
    return stt;
}

static GuestNode *nthGuestMatch(const char *key, int n) {
    GuestNode *p;
    // Lặp theo ba phần [p = g_guests; p != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (p = g_guests; p != NULL; p = p->next) {
        // Kiểm tra điều kiện [guestMatches(&p->data, key) && --n == 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (guestMatches(&p->data, key) && --n == 0) return p;
    }
    // Báo rằng không tìm thấy dữ liệu hoặc không thể trả về một đối tượng.
    return NULL;
}

/* Tim khach theo tu khoa va cho chon theo STT; co the cho phep tao khach moi. */
static GuestNode *pickGuest(int allowCreate) {
    // Khai báo biến [key] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char key[GUEST_NAME_LEN];
    GuestNode *created;
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count, choice, addOption;

    // Kiểm tra điều kiện [g_guests == NULL && !allowCreate]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (g_guests == NULL && !allowCreate) {
        // Hiển thị nội dung [[!] Chua co khach nao.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Chua co khach nao.\n");
        // Báo rằng không tìm thấy dữ liệu hoặc không thể trả về một đối tượng.
        return NULL;
    }
    // Lặp thao tác nhập/chọn cho đến khi dữ liệu hợp lệ hoặc người dùng hủy; break/return bên dưới sẽ thoát vòng lặp.
    for (;;) {
        // Kiểm tra điều kiện [g_guests == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (g_guests == NULL) {
            // Hiển thị nội dung [Chua co khach nao trong he thong.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("Chua co khach nao trong he thong.\n");
            // Lưu [giá trị biểu thức [0]] vào biến [count] để các lệnh sau sử dụng.
            count = 0;
            key[0] = '\0';
        } else {
            // Gọi hàm [inputString] để đọc chuỗi, kiểm tra độ dài và cho phép/ràng buộc chuỗi rỗng theo tham số cuối.
            inputString("Tim khach (ten, CCCD, SDT hoac ma; Enter = xem tat ca): ", key, GUEST_NAME_LEN, 1);
            // Lưu [giá trị biểu thức [printGuestMatches(key)]] vào biến [count] để các lệnh sau sử dụng.
            count = printGuestMatches(key);
            // Kiểm tra điều kiện [count == 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
            if (count == 0) printf("Khong tim thay khach phu hop voi \"%s\".\n", key);
        }

        // Lưu [giá trị biểu thức [allowCreate ? count + 1 : -2]] vào biến [addOption] để các lệnh sau sử dụng.
        addOption = allowCreate ? count + 1 : -2;
        // Kiểm tra điều kiện [count > 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (count > 0) printf("  1..%d: chon khach theo STT\n", count);
        // Kiểm tra điều kiện [allowCreate]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (allowCreate) printf("  %d: them khach moi\n", addOption);
        // Hiển thị nội dung [  0: tim lai -1: huy] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("  0: tim lai\n -1: huy\n");
        // Lưu [số nguyên người dùng nhập sau khi kiểm tra giới hạn] vào biến [choice] để các lệnh sau sử dụng.
        choice = inputInt("Chon: ", -1, allowCreate ? addOption : count);

        // Kiểm tra điều kiện [choice == -1]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (choice == -1) return NULL;
        // Kiểm tra điều kiện [choice == 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (choice == 0) continue;
        // Kiểm tra điều kiện [choice <= count]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (choice <= count) return nthGuestMatch(key, choice);

        // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
        printTitle("THEM KHACH MOI");
        // Lưu [giá trị biểu thức [createGuestInteractive()]] vào biến [created] để các lệnh sau sử dụng.
        created = createGuestInteractive();
        // Kiểm tra điều kiện [created != NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (created != NULL) return created;
    }
}

static void uiUpdateGuest(void) {
    // Khai báo biến [name] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char name[GUEST_NAME_LEN], idCard[ID_CARD_LEN], phone[PHONE_LEN];
    GuestNode *node, *other;

    // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
    printTitle("SUA THONG TIN KHACH");
    // Lưu [giá trị biểu thức [pickGuest(0)]] vào biến [node] để các lệnh sau sử dụng.
    node = pickGuest(0);
    // Kiểm tra điều kiện [node == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (node == NULL) return;
    // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
    strcpy(name, node->data.fullName);
    // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
    strcpy(idCard, node->data.idCard);
    // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
    strcpy(phone, node->data.phone);

    // Hiển thị nội dung [(Nhan Enter de giu nguyen gia tri trong ngoac)] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("(Nhan Enter de giu nguyen gia tri trong ngoac)\n");
    // Gọi hàm [inputStringOrKeep] để cho sửa chuỗi; nhấn Enter thì giữ nguyên giá trị hiện tại.
    inputStringOrKeep("Ho ten", name, GUEST_NAME_LEN);
    // Lặp thao tác nhập/chọn cho đến khi dữ liệu hợp lệ hoặc người dùng hủy; break/return bên dưới sẽ thoát vòng lặp.
    for (;;) {
        // Gọi hàm [inputDigitsOrKeep] để cho sửa chuỗi chữ số; nhấn Enter thì giữ nguyên giá trị hiện tại.
        inputDigitsOrKeep("CCCD (chi gom chu so, toi da 12)", idCard, ID_CARD_MIN_DIGITS, ID_CARD_MAX_DIGITS);
        // Lưu [giá trị biểu thức [findGuestByIdCard(g_guests, idCard)]] vào biến [other] để các lệnh sau sử dụng.
        other = findGuestByIdCard(g_guests, idCard);
        // Kiểm tra điều kiện [other == NULL || other == node]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (other == NULL || other == node) break;
        // Hiển thị nội dung [[!] CCCD nay da thuoc ve khach %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] CCCD nay da thuoc ve khach %s.\n", other->data.guestId);
        // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
        strcpy(idCard, node->data.idCard);
    }
    // Gọi hàm [inputDigitsOrKeep] để cho sửa chuỗi chữ số; nhấn Enter thì giữ nguyên giá trị hiện tại.
    inputDigitsOrKeep("So dien thoai (chi gom chu so, toi da 10)", phone, PHONE_MIN_DIGITS, PHONE_MAX_DIGITS);

    // Gọi hàm [updateGuest] để chép tên, CCCD và điện thoại mới vào bản ghi khách.
    updateGuest(&node->data, name, idCard, phone);
    // Hiển thị nội dung [Da cap nhat khach %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Da cap nhat khach %s.\n", node->data.guestId);
    // Gọi hàm [autoSave] để ghi các danh sách hiện tại xuống những tệp dữ liệu.
    autoSave();
}

static void uiDeleteGuest(void) {
    // Khai báo biến [id] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char id[GUEST_ID_LEN];
    GuestNode *node;
    // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
    printTitle("XOA KHACH");
    // Lưu [giá trị biểu thức [pickGuest(0)]] vào biến [node] để các lệnh sau sử dụng.
    node = pickGuest(0);
    // Kiểm tra điều kiện [node == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (node == NULL) return;
    // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
    strcpy(id, node->data.guestId);
    // Kiểm tra điều kiện [hasActiveBookingForGuest(g_bookings, id)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (hasActiveBookingForGuest(g_bookings, id)) {
        // Hiển thị nội dung [[!] Khong the xoa: khach %s dang co phieu dat hoac dang o.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Khong the xoa: khach %s dang co phieu dat hoac dang o.\n", id);
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Kiểm tra điều kiện [!inputYesNo("Ban chac chan muon xoa?")]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (!inputYesNo("Ban chac chan muon xoa?")) return;
    // Kiểm tra điều kiện [deleteGuest(&g_guests, id) == LIST_OK]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (deleteGuest(&g_guests, id) == LIST_OK) {
        // Hiển thị nội dung [Da xoa khach %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("Da xoa khach %s.\n", id);
        // Gọi hàm [autoSave] để ghi các danh sách hiện tại xuống những tệp dữ liệu.
        autoSave();
    }
}

static void uiFindGuestByIdCard(void) {
    // Khai báo biến [idCard] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char idCard[ID_CARD_LEN];
    GuestNode *node;
    // Gọi hàm [inputDigits] để đọc chuỗi chỉ gồm chữ số và giới hạn độ dài.
    inputDigits("CCCD can tim: ", idCard, ID_CARD_MIN_DIGITS, ID_CARD_MAX_DIGITS);
    // Lưu [giá trị biểu thức [findGuestByIdCard(g_guests, idCard)]] vào biến [node] để các lệnh sau sử dụng.
    node = findGuestByIdCard(g_guests, idCard);
    // Kiểm tra điều kiện [node == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (node == NULL) {
        // Hiển thị nội dung [[!] Khong co khach nao co CCCD %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Khong co khach nao co CCCD %s.\n", idCard);
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Gọi hàm [printGuestHeader] để in tiêu đề các cột của bảng khách.
    printGuestHeader();
    // Gọi hàm [printGuestRow] để in thông tin của một khách thành một dòng.
    printGuestRow(&node->data);
    // Gọi hàm [printGuestFooter] để in đường kẻ kết thúc bảng khách.
    printGuestFooter();
}

static void uiSearchGuestByName(void) {
    // Khai báo biến [key] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char key[GUEST_NAME_LEN];
    // Gọi hàm [inputString] để đọc chuỗi, kiểm tra độ dài và cho phép/ràng buộc chuỗi rỗng theo tham số cuối.
    inputString("Nhap ten (hoac mot phan ten): ", key, GUEST_NAME_LEN, 0);
    // Gọi hàm [searchGuestsByName] để tìm và in khách có tên chứa từ khóa.
    searchGuestsByName(g_guests, key);
}

static void uiGuestHistory(void) {
    GuestNode *node;
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count;
    // Lưu [giá trị biểu thức [pickGuest(0)]] vào biến [node] để các lệnh sau sử dụng.
    node = pickGuest(0);
    // Kiểm tra điều kiện [node == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (node == NULL) return;
    // Hiển thị nội dung [Lich su dat phong cua %s - %s (moi nhat truoc):] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Lich su dat phong cua %s - %s (moi nhat truoc):\n", node->data.guestId, node->data.fullName);
    // Gọi hàm [printBookingHeader] để in tiêu đề các cột của bảng phiếu.
    printBookingHeader();
    // Lưu [giá trị biểu thức [printHistoryReverse(g_bookings, node->data.guestId)]] vào biến [count] để các lệnh sau sử dụng.
    count = printHistoryReverse(g_bookings, node->data.guestId);
    // Gọi hàm [printBookingFooter] để in đường kẻ kết thúc bảng phiếu.
    printBookingFooter();
    // Hiển thị nội dung [Tong: %d phieu.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Tong: %d phieu.\n", count);
}

static void menuGuest(void) {
    // Khai báo biến [choice] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int choice;
    do {
        // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
        printTitle("QUAN LY KHACH");
        // Hiển thị nội dung [1. Danh sach khach] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("1. Danh sach khach\n");
        // Hiển thị nội dung [2. Them khach] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("2. Them khach\n");
        // Hiển thị nội dung [3. Sua khach] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("3. Sua khach\n");
        // Hiển thị nội dung [4. Xoa khach] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("4. Xoa khach\n");
        // Hiển thị nội dung [5. Tim khach theo CCCD] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("5. Tim khach theo CCCD\n");
        // Hiển thị nội dung [6. Tim khach theo ten] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("6. Tim khach theo ten\n");
        // Hiển thị nội dung [7. Lich su dat phong cua khach] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("7. Lich su dat phong cua khach\n");
        // Hiển thị nội dung [0. Quay lai] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("0. Quay lai\n");
        // Lưu [số nguyên người dùng nhập sau khi kiểm tra giới hạn] vào biến [choice] để các lệnh sau sử dụng.
        choice = inputInt("Chon: ", 0, 7);
        // Đọc giá trị [choice] để chọn một nhánh case tương ứng bên dưới.
        switch (choice) {
            // Đây là nhánh xử lý khi giá trị switch bằng [1].
            case 1: printGuests(g_guests); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [2].
            case 2: uiAddGuest(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [3].
            case 3: uiUpdateGuest(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [4].
            case 4: uiDeleteGuest(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [5].
            case 5: uiFindGuestByIdCard(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [6].
            case 6: uiSearchGuestByName(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [7].
            case 7: uiGuestHistory(); break;
        }
        // Kiểm tra điều kiện [choice != 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

/* Nhap lai cho den khi ngay tra sau ngay nhan va so dem khong qua gioi han. */
static void inputDateRange(Date *in, Date *out) {
    // Lặp thao tác nhập/chọn cho đến khi dữ liệu hợp lệ hoặc người dùng hủy; break/return bên dưới sẽ thoát vòng lặp.
    for (;;) {
        *in = inputDate("Ngay nhan phong (dd/mm/yyyy): ");
        *out = inputDate("Ngay tra phong  (dd/mm/yyyy): ");
        // Kiểm tra điều kiện [compareDate(*out, *in) <= 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (compareDate(*out, *in) <= 0) {
            // Hiển thị nội dung [  [!] Ngay tra phai sau ngay nhan. Nhap lai.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Ngay tra phai sau ngay nhan. Nhap lai.\n");
            continue;
        }
        // Kiểm tra điều kiện [nightsBetween(*in, *out) > MAX_NIGHTS]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (nightsBetween(*in, *out) > MAX_NIGHTS) {
            // Hiển thị nội dung [  [!] Toi da %d dem cho mot lan dat. Nhap lai.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Toi da %d dem cho mot lan dat. Nhap lai.\n", MAX_NIGHTS);
            continue;
        }
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
}

static void uiCheckAvailability(void) {
    // Khai báo biến [in] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Date in, out;
    // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
    printTitle("KIEM TRA PHONG TRONG");
    // Gọi hàm [inputDateRange] để nhập lại cặp ngày cho đến khi ngày trả sau ngày nhận và số đêm hợp lệ.
    inputDateRange(&in, &out);
    // Hiển thị nội dung [So dem: %d] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("So dem: %d\n", nightsBetween(in, out));
    // Gọi hàm [listAvailableRooms] để in những phòng không có lịch đặt giao với khoảng ngày.
    listAvailableRooms(g_rooms, g_bookings, in, out);
}

/* Huong dan dat phong: chon ngay, phong trong, khach; xem tam tinh va xac nhan. */
static void uiCreateBooking(void) {
    // Khai báo biến [roomId] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char roomId[ROOM_ID_LEN];
    GuestNode *guest;
    RoomNode *room;
    // Khai báo biến [in] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Date in, out;
    // Khai báo biến [rc] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int rc, newId, nights;
    // Khai báo biến [preview] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Booking preview;
    // Khai báo biến [inv] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Invoice inv;

    // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
    printTitle("DAT PHONG");
    // Kiểm tra điều kiện [g_rooms == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (g_rooms == NULL) {
        // Hiển thị nội dung [[!] Can co it nhat mot phong truoc khi dat phong.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Can co it nhat mot phong truoc khi dat phong.\n");
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }

    // Gọi hàm [inputDateRange] để nhập lại cặp ngày cho đến khi ngày trả sau ngày nhận và số đêm hợp lệ.
    inputDateRange(&in, &out);
    // Nếu ngày nhận đã ở quá khứ và người dùng không đồng ý tiếp tục thì dừng thao tác đặt phòng.
    if (compareDate(in, getToday()) < 0 &&
        !inputYesNo("Ngay nhan phong da qua. Van tiep tuc?")) {
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }

    // Kiểm tra điều kiện [listAvailableRooms(g_rooms, g_bookings, in, out) == 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (listAvailableRooms(g_rooms, g_bookings, in, out) == 0) {
        // Hiển thị nội dung [[!] Khong con phong trong trong khoang ngay nay.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Khong con phong trong trong khoang ngay nay.\n");
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Lặp thao tác nhập/chọn cho đến khi dữ liệu hợp lệ hoặc người dùng hủy; break/return bên dưới sẽ thoát vòng lặp.
    for (;;) {
        // Gọi hàm [inputCode] để đọc mã từ bàn phím, kiểm tra độ dài và chuẩn hóa chữ hoa.
        inputCode("Chon ma phong: ", roomId, ROOM_ID_LEN);
        // Lưu [nút phòng có mã cần tìm, hoặc NULL nếu không tồn tại] vào biến [room] để các lệnh sau sử dụng.
        room = findRoomById(g_rooms, roomId);
        // Kiểm tra điều kiện [room == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (room == NULL) {
            // Hiển thị nội dung [[!] Khong tim thay phong %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("[!] Khong tim thay phong %s.\n", roomId);
        } else if (!isRoomAvailable(g_bookings, room->data.roomId, in, out)) {
            // Hiển thị nội dung [[!] Phong %s da co nguoi dat trong khoang ngay nay.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("[!] Phong %s da co nguoi dat trong khoang ngay nay.\n", room->data.roomId);
        } else {
            break;
        }
        // Kiểm tra điều kiện [!inputYesNo("Chon phong khac?")]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!inputYesNo("Chon phong khac?")) return;
    }

    // Hiển thị nội dung [--- Chon khach dat phong ---] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("\n--- Chon khach dat phong ---\n");
    // Lưu [giá trị biểu thức [pickGuest(1)]] vào biến [guest] để các lệnh sau sử dụng.
    guest = pickGuest(1);
    // Kiểm tra điều kiện [guest == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (guest == NULL) {
        // Hiển thị nội dung [Da huy dat phong.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("Da huy dat phong.\n");
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Hiển thị nội dung [Khach: %s - %s - CCCD %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Khach: %s - %s - CCCD %s\n", guest->data.guestId, guest->data.fullName, guest->data.idCard);

    // Đặt toàn bộ byte của cấu trúc về 0 để bắt đầu với dữ liệu sạch.
    memset(&preview, 0, sizeof(preview));
    // Tạo phiếu giả chỉ để ước tính hóa đơn, đặt ngày nhận giống phiếu sắp tạo.
    preview.checkIn = in;
    // Đặt ngày trả dự kiến để tính số đêm và tiền tạm tính.
    preview.checkOut = out;
    // Gọi hàm [calcAmount] để tính tiền phòng, mức giảm giá, phụ thu và tổng tiền.
    calcAmount(&preview, room->data.pricePerNight, out, &inv);
    // Lưu [giá trị biểu thức [inv.nights]] vào biến [nights] để các lệnh sau sử dụng.
    nights = inv.nights;
    // Hiển thị nội dung [Phong %s (%s), %d dem x %.0f VND] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Phong %s (%s), %d dem x %.0f VND", room->data.roomId, roomTypeName(room->data.type),
           nights, room->data.pricePerNight);
    // Kiểm tra điều kiện [inv.discount > 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (inv.discount > 0) printf(", giam %.0f VND", inv.discount);
    // Hiển thị nội dung [ => Tam tinh: %.0f VND] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf(" => Tam tinh: %.0f VND\n", inv.total);
    // Kiểm tra điều kiện [!inputYesNo("Xac nhan dat phong?")]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (!inputYesNo("Xac nhan dat phong?")) return;

    rc = createBooking(&g_bookings, g_rooms, g_guests, room->data.roomId, guest->data.guestId,
                       in, out, &newId);
    // Kiểm tra điều kiện [rc == BK_OK]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (rc == BK_OK) {
        // Hiển thị nội dung [Dat phong thanh cong. Ma phieu: %d] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("Dat phong thanh cong. Ma phieu: %d\n", newId);
        // Gọi hàm [autoSave] để ghi các danh sách hiện tại xuống những tệp dữ liệu.
        autoSave();
    } else {
        // Hiển thị nội dung [[!] %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] %s\n", bookingErrorMessage(rc));
    }
}

/* Chon phieu Da dat, canh bao ngay bat thuong va chuyen phieu sang Dang o. */
static void uiCheckIn(void) {
    // Khai báo biến [id] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int id, rc;
    BookingNode *b;
    // Khai báo biến [today] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Date today = getToday();

    // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
    printTitle("NHAN PHONG (CHECK-IN)");
    // Kiểm tra điều kiện [printBookings(g_bookings, BK_BOOKED) == 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (printBookings(g_bookings, BK_BOOKED) == 0) return;
    // Lưu [số nguyên người dùng nhập sau khi kiểm tra giới hạn] vào biến [id] để các lệnh sau sử dụng.
    id = inputInt("Ma phieu: ", 1, 2147483647);
    // Lưu [nút phiếu có mã cần tìm, hoặc NULL nếu không tồn tại] vào biến [b] để các lệnh sau sử dụng.
    b = findBookingById(g_bookings, id);
    // Kiểm tra điều kiện [b == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (b == NULL) {
        // Hiển thị nội dung [[!] %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] %s\n", bookingErrorMessage(BK_ERR_NOT_FOUND));
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Kiểm tra điều kiện [b->data.status == BK_BOOKED]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (b->data.status == BK_BOOKED) {
        // Nếu hôm nay còn trước ngày nhận mà người dùng không đồng ý nhận sớm thì không check-in.
        if (compareDate(today, b->data.checkIn) < 0 &&
            !inputYesNo("Chua den ngay nhan phong. Van nhan phong som?")) {
            // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
            return;
        }
        // Nếu hôm nay đã đến hoặc qua ngày trả mà người dùng không đồng ý tiếp tục thì không check-in.
        if (compareDate(today, b->data.checkOut) >= 0 &&
            !inputYesNo("Da qua ngay tra phong cua phieu. Van nhan phong?")) {
            // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
            return;
        }
    }
    // Lưu [mã kết quả nhận phòng] vào biến [rc] để các lệnh sau sử dụng.
    rc = checkIn(g_bookings, id);
    // Kiểm tra điều kiện [rc == BK_OK]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (rc == BK_OK) {
        // Hiển thị nội dung [Phieu %d: khach da nhan phong %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("Phieu %d: khach da nhan phong %s.\n", id, b->data.roomId);
        // Gọi hàm [autoSave] để ghi các danh sách hiện tại xuống những tệp dữ liệu.
        autoSave();
    } else {
        // Hiển thị nội dung [[!] %s (trang thai hien tai: %s)] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] %s (trang thai hien tai: %s)\n", bookingErrorMessage(rc), bookingStatusName(b->data.status));
    }
}

/* Chon phieu Dang o, nhap ngay tra thuc te, tinh/in hoa don va luu ket qua. */
static void uiCheckOut(void) {
    // Khai báo biến [id] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int id, rc;
    BookingNode *b;
    RoomNode *room;
    GuestNode *guest;
    // Khai báo biến [actual] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Date actual;
    // Khai báo biến [inv] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Invoice inv;

    // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
    printTitle("TRA PHONG (CHECK-OUT)");
    // Kiểm tra điều kiện [printBookings(g_bookings, BK_STAYING) == 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (printBookings(g_bookings, BK_STAYING) == 0) return;
    // Lưu [số nguyên người dùng nhập sau khi kiểm tra giới hạn] vào biến [id] để các lệnh sau sử dụng.
    id = inputInt("Ma phieu: ", 1, 2147483647);
    // Lưu [nút phiếu có mã cần tìm, hoặc NULL nếu không tồn tại] vào biến [b] để các lệnh sau sử dụng.
    b = findBookingById(g_bookings, id);
    // Kiểm tra điều kiện [b == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (b == NULL) {
        // Hiển thị nội dung [[!] %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] %s\n", bookingErrorMessage(BK_ERR_NOT_FOUND));
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Kiểm tra điều kiện [b->data.status != BK_STAYING]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (b->data.status != BK_STAYING) {
        // Hiển thị nội dung [[!] %s (trang thai hien tai: %s)] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] %s (trang thai hien tai: %s)\n", bookingErrorMessage(BK_ERR_STATUS),
               // Gọi hàm [bookingStatusName] để lấy tên tiếng Việt tương ứng với mã trạng thái phiếu.
               bookingStatusName(b->data.status));
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Lưu [giá trị biểu thức [inputDateOrKeep("Ngay tra thuc te", getToday())]] vào biến [actual] để các lệnh sau sử dụng.
    actual = inputDateOrKeep("Ngay tra thuc te", getToday());
    // Lặp lại khối lệnh khi điều kiện [compareDate(actual, b->data.checkIn) < 0] vẫn đúng.
    while (compareDate(actual, b->data.checkIn) < 0) {
        // Hiển thị nội dung [  [!] Ngay tra khong duoc truoc ngay nhan phong.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("  [!] Ngay tra khong duoc truoc ngay nhan phong.\n");
        // Lưu [giá trị biểu thức [inputDateOrKeep("Ngay tra thuc te", b->data.checkOut)]] vào biến [actual] để các lệnh sau sử dụng.
        actual = inputDateOrKeep("Ngay tra thuc te", b->data.checkOut);
    }

    // Chỉ cảnh báo khi ngày trả thực tế muộn hơn ngày dự kiến và phần thời gian kéo dài làm trùng với lịch đặt khác của phòng.
    if (compareDate(actual, b->data.checkOut) > 0 &&
        !isRoomAvailable(g_bookings, b->data.roomId, b->data.checkOut, actual)) {
        // Hiển thị nội dung [[!] Canh bao: khoang tra muon trung voi phieu dat khac cua phong %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Canh bao: khoang tra muon trung voi phieu dat khac cua phong %s.\n", b->data.roomId);
        // Kiểm tra điều kiện [!inputYesNo("Van tra phong voi ngay nay?")]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!inputYesNo("Van tra phong voi ngay nay?")) return;
    }

    // Lưu [mã kết quả trả phòng] vào biến [rc] để các lệnh sau sử dụng.
    rc = checkOut(g_bookings, g_rooms, id, actual, &inv);
    // Kiểm tra điều kiện [rc != BK_OK]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (rc != BK_OK) {
        // Hiển thị nội dung [[!] %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] %s\n", bookingErrorMessage(rc));
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Lưu [nút phòng có mã cần tìm, hoặc NULL nếu không tồn tại] vào biến [room] để các lệnh sau sử dụng.
    room = findRoomById(g_rooms, b->data.roomId);
    // Lưu [nút khách có mã cần tìm, hoặc NULL nếu không tồn tại] vào biến [guest] để các lệnh sau sử dụng.
    guest = findGuestById(g_guests, b->data.guestId);
    // Gọi hàm [printInvoice] để in thông tin khách, phòng và các khoản tiền trên hóa đơn.
    printInvoice(&b->data, room != NULL ? &room->data : NULL, guest != NULL ? &guest->data : NULL, &inv);
    // Gọi hàm [autoSave] để ghi các danh sách hiện tại xuống những tệp dữ liệu.
    autoSave();
}

/* Hien cac phieu Da dat va huy phieu duoc chon sau khi xac nhan. */
static void uiCancelBooking(void) {
    // Khai báo biến [id] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int id, rc;
    BookingNode *b;
    // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
    printTitle("HUY DAT PHONG");
    // Kiểm tra điều kiện [printBookings(g_bookings, BK_BOOKED) == 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (printBookings(g_bookings, BK_BOOKED) == 0) return;
    // Lưu [số nguyên người dùng nhập sau khi kiểm tra giới hạn] vào biến [id] để các lệnh sau sử dụng.
    id = inputInt("Ma phieu can huy: ", 1, 2147483647);
    // Lưu [nút phiếu có mã cần tìm, hoặc NULL nếu không tồn tại] vào biến [b] để các lệnh sau sử dụng.
    b = findBookingById(g_bookings, id);
    // Kiểm tra điều kiện [b == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (b == NULL) {
        // Hiển thị nội dung [[!] %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] %s\n", bookingErrorMessage(BK_ERR_NOT_FOUND));
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Kiểm tra điều kiện [b->data.status != BK_BOOKED]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (b->data.status != BK_BOOKED) {
        // Hiển thị nội dung [[!] Chi huy duoc phieu o trang thai 'Da dat' (hien tai: %s).] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Chi huy duoc phieu o trang thai 'Da dat' (hien tai: %s).\n",
               // Gọi hàm [bookingStatusName] để lấy tên tiếng Việt tương ứng với mã trạng thái phiếu.
               bookingStatusName(b->data.status));
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Kiểm tra điều kiện [!inputYesNo("Ban chac chan muon huy phieu nay?")]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (!inputYesNo("Ban chac chan muon huy phieu nay?")) return;
    // Lưu [giá trị biểu thức [cancelBooking(g_bookings, id)]] vào biến [rc] để các lệnh sau sử dụng.
    rc = cancelBooking(g_bookings, id);
    // Kiểm tra điều kiện [rc == BK_OK]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (rc == BK_OK) {
        // Hiển thị nội dung [Da huy phieu %d.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("Da huy phieu %d.\n", id);
        // Gọi hàm [autoSave] để ghi các danh sách hiện tại xuống những tệp dữ liệu.
        autoSave();
    } else {
        // Hiển thị nội dung [[!] %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] %s\n", bookingErrorMessage(rc));
    }
}

static void uiListBookings(void) {
    // Khai báo biến [f] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int f;
    // Hiển thị nội dung [Loc theo trang thai: -1 = Tat ca, 0 = Da dat, 1 = Dang o, 2 = Da tra, 3 = Huy] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Loc theo trang thai: -1 = Tat ca, 0 = Da dat, 1 = Dang o, 2 = Da tra, 3 = Huy\n");
    // Lưu [số nguyên người dùng nhập sau khi kiểm tra giới hạn] vào biến [f] để các lệnh sau sử dụng.
    f = inputInt("Chon: ", -1, 3);
    // Gọi hàm [printBookings] để in các phiếu theo trạng thái được chọn.
    printBookings(g_bookings, f);
}

static void menuBooking(void) {
    // Khai báo biến [choice] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int choice;
    do {
        // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
        printTitle("DAT PHONG / TRA PHONG");
        // Hiển thị nội dung [1. Danh sach phieu dat] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("1. Danh sach phieu dat\n");
        // Hiển thị nội dung [2. Kiem tra phong trong theo khoang ngay] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("2. Kiem tra phong trong theo khoang ngay\n");
        // Hiển thị nội dung [3. Dat phong] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("3. Dat phong\n");
        // Hiển thị nội dung [4. Nhan phong (check-in)] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("4. Nhan phong (check-in)\n");
        // Hiển thị nội dung [5. Tra phong (check-out, in hoa don)] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("5. Tra phong (check-out, in hoa don)\n");
        // Hiển thị nội dung [6. Huy dat phong] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("6. Huy dat phong\n");
        // Hiển thị nội dung [0. Quay lai] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("0. Quay lai\n");
        // Lưu [số nguyên người dùng nhập sau khi kiểm tra giới hạn] vào biến [choice] để các lệnh sau sử dụng.
        choice = inputInt("Chon: ", 0, 6);
        // Đọc giá trị [choice] để chọn một nhánh case tương ứng bên dưới.
        switch (choice) {
            // Đây là nhánh xử lý khi giá trị switch bằng [1].
            case 1: uiListBookings(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [2].
            case 2: uiCheckAvailability(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [3].
            case 3: uiCreateBooking(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [4].
            case 4: uiCheckIn(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [5].
            case 5: uiCheckOut(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [6].
            case 6: uiCancelBooking(); break;
        }
        // Kiểm tra điều kiện [choice != 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

static void menuReport(void) {
    // Khai báo biến [choice] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int choice, year;
    do {
        // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
        printTitle("THONG KE");
        // Hiển thị nội dung [1. Doanh thu theo thang] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("1. Doanh thu theo thang\n");
        // Hiển thị nội dung [2. Doanh thu theo phong] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("2. Doanh thu theo phong\n");
        // Hiển thị nội dung [3. So phong trong hom nay] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("3. So phong trong hom nay\n");
        // Hiển thị nội dung [4. Danh sach khach dang o] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("4. Danh sach khach dang o\n");
        // Hiển thị nội dung [0. Quay lai] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("0. Quay lai\n");
        // Lưu [số nguyên người dùng nhập sau khi kiểm tra giới hạn] vào biến [choice] để các lệnh sau sử dụng.
        choice = inputInt("Chon: ", 0, 4);
        // Đọc giá trị [choice] để chọn một nhánh case tương ứng bên dưới.
        switch (choice) {
            // Đây là nhánh xử lý khi giá trị switch bằng [1].
            case 1:
                // Lưu [giá trị biểu thức [inputIntOrKeep("Nam", MIN_YEAR, MAX_YEAR, getToday().year)]] vào biến [year] để các lệnh sau sử dụng.
                year = inputIntOrKeep("Nam", MIN_YEAR, MAX_YEAR, getToday().year);
                // Gọi hàm [revenueByMonth] để tính và in doanh thu theo từng tháng trong năm.
                revenueByMonth(g_bookings, year);
                break;
            // Đây là nhánh xử lý khi giá trị switch bằng [2].
            case 2: revenueByRoom(g_rooms, g_bookings); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [3].
            case 3: reportAvailableToday(g_rooms, g_bookings); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [4].
            case 4: listCurrentGuests(g_bookings, g_guests); break;
        }
        // Kiểm tra điều kiện [choice != 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (choice != 0) pauseScreen();
    } while (choice != 0);
}

/* Vong lap menu chinh: dieu huong quan ly phong, khach, dat phong va thong ke. */
static void menuLoop(void) {
    // Khai báo biến [choice] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int choice;
    // Khai báo biến [today] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char today[DATE_STR_LEN];
    do {
        // Gọi hàm [formatDate] để định dạng ngày thành chuỗi dd/mm/yyyy để in hoặc ghép vào thông báo.
        formatDate(getToday(), today, sizeof(today));
        // Gọi hàm [printTitle] để in tiêu đề của màn hình chức năng.
        printTitle("QUAN LY DAT PHONG KHACH SAN");
        // Hiển thị nội dung [  Hom nay: %s | %d phong | %d khach] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("  Hom nay: %s | %d phong | %d khach\n", today, countRooms(g_rooms), countGuests(g_guests));
        // Hiển thị nội dung [------------------------------------------------------------] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("------------------------------------------------------------\n");
        // Hiển thị nội dung [1. Quan ly phong] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("1. Quan ly phong\n");
        // Hiển thị nội dung [2. Quan ly khach] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("2. Quan ly khach\n");
        // Hiển thị nội dung [3. Dat phong / Tra phong] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("3. Dat phong / Tra phong\n");
        // Hiển thị nội dung [4. Thong ke] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("4. Thong ke\n");
        // Hiển thị nội dung [5. Luu du lieu] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("5. Luu du lieu\n");
        // Hiển thị nội dung [0. Thoat] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("0. Thoat\n");
        // Lưu [số nguyên người dùng nhập sau khi kiểm tra giới hạn] vào biến [choice] để các lệnh sau sử dụng.
        choice = inputInt("Chon: ", 0, 5);
        // Đọc giá trị [choice] để chọn một nhánh case tương ứng bên dưới.
        switch (choice) {
            // Đây là nhánh xử lý khi giá trị switch bằng [1].
            case 1: menuRoom(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [2].
            case 2: menuGuest(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [3].
            case 3: menuBooking(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [4].
            case 4: menuReport(); break;
            // Đây là nhánh xử lý khi giá trị switch bằng [5].
            case 5:
                // Kiểm tra điều kiện [saveAll(g_rooms, g_guests, g_bookings)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
                if (saveAll(g_rooms, g_guests, g_bookings)) printf("Da luu du lieu vao %s\n", getDataDir());
                // Gọi hàm [pauseScreen] để chờ người dùng nhấn Enter trước khi tiếp tục.
                pauseScreen();
                break;
        }
    } while (choice != 0);
}

/* Diem bat dau: chon thu muc du lieu, nap tep, chay menu, luu va giai phong RAM. */
// Hàm [main] nạp dữ liệu, chạy menu, lưu kết quả và giải phóng bộ nhớ.
int main(int argc, char *argv[]) {
    // Kiểm tra điều kiện [argc >= 2]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (argc >= 2) setDataDir(argv[1]);

    // Gọi hàm [loadAll] để đọc dữ liệu phòng, khách và phiếu từ các tệp.
    loadAll(&g_rooms, &g_guests, &g_bookings);
    // Gọi hàm [menuLoop] để hiện menu chính và chuyển đến chức năng người dùng chọn.
    menuLoop();

    // Kiểm tra điều kiện [saveAll(g_rooms, g_guests, g_bookings)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (saveAll(g_rooms, g_guests, g_bookings)) {
        // Hiển thị nội dung [Da luu du lieu. Tam biet!] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("Da luu du lieu. Tam biet!\n");
    }
    // Gọi hàm [freeRoomList] để giải phóng lần lượt bộ nhớ của các nút phòng.
    freeRoomList(&g_rooms);
    // Gọi hàm [freeGuestList] để giải phóng lần lượt bộ nhớ của các nút khách.
    freeGuestList(&g_guests);
    // Gọi hàm [freeBookingList] để giải phóng lần lượt bộ nhớ của các nút phiếu.
    freeBookingList(&g_bookings);
    // Trả mã kết quả [0] cho hàm gọi để báo trạng thái xử lý.
    return 0;
}
