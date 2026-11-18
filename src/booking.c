#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "booking.h"

const char *bookingStatusName(int status) {
    // Đọc giá trị [status] để chọn một nhánh case tương ứng bên dưới.
    switch (status) {
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_BOOKED].
        case BK_BOOKED:     return "Da dat";
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_STAYING].
        case BK_STAYING:    return "Dang o";
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_CHECKEDOUT].
        case BK_CHECKEDOUT: return "Da tra";
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_CANCELLED].
        case BK_CANCELLED:  return "Huy";
        // Đây là nhánh dự phòng nếu không có case nào khớp.
        default:            return "?";
    }
}

const char *bookingErrorMessage(int code) {
    // Đọc giá trị [code] để chọn một nhánh case tương ứng bên dưới.
    switch (code) {
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_OK].
        case BK_OK:            return "Thanh cong.";
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_ERR_ROOM].
        case BK_ERR_ROOM:      return "Khong tim thay phong.";
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_ERR_GUEST].
        case BK_ERR_GUEST:     return "Khong tim thay khach.";
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_ERR_DATE].
        case BK_ERR_DATE:      return "Ngay khong hop le (ngay tra phai sau ngay nhan).";
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_ERR_CONFLICT].
        case BK_ERR_CONFLICT:  return "Phong da co nguoi dat trong khoang ngay nay (trung lich).";
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_ERR_NOT_FOUND].
        case BK_ERR_NOT_FOUND: return "Khong tim thay phieu dat.";
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_ERR_STATUS].
        case BK_ERR_STATUS:    return "Trang thai phieu khong cho phep thao tac nay.";
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_ERR_MEMORY].
        case BK_ERR_MEMORY:    return "Khong du bo nho.";
        // Đây là nhánh xử lý khi giá trị switch bằng [BK_ERR_TOO_LONG].
        case BK_ERR_TOO_LONG:  return "So dem vuot qua gioi han cho phep.";
        // Đây là nhánh dự phòng nếu không có case nào khớp.
        default:               return "Loi khong xac dinh.";
    }
}

/* Chi phieu da dat va dang o moi chiem lich phong; phieu huy/da tra khong tinh. */
static int isActive(int status) {
    // Trả giá trị [status == BK_BOOKED || status == BK_STAYING] về cho hàm đã gọi dòng này.
    return status == BK_BOOKED || status == BK_STAYING;
}

// Hàm [nightsBetween] tính số đêm giữa hai ngày.
int nightsBetween(Date in, Date out) {
    // Trả giá trị [(int)(dateToDays(out) - dateToDays(in))] về cho hàm đã gọi dòng này.
    return (int)(dateToDays(out) - dateToDays(in));
}

/* Hai khoang [in, out) giao nhau neu in1 < out2 va out1 > in2. */
// Hàm [isDateRangeOverlap] kiểm tra hai khoảng ngày có giao nhau.
int isDateRangeOverlap(Date newIn, Date newOut, Date oldIn, Date oldOut) {
    // Trả giá trị [compareDate(newIn, oldOut) < 0 && compareDate(newOut, oldIn) > 0] về cho hàm đã gọi dòng này.
    return compareDate(newIn, oldOut) < 0 && compareDate(newOut, oldIn) > 0;
}

/* Phong trong neu khong co phieu con hieu luc cung phong bi trung khoang ngay. */
// Hàm [isRoomAvailable] kiểm tra phòng có lịch đặt trùng hay không.
int isRoomAvailable(const BookingNode *head, const char *roomId, Date in, Date out) {
    // Lặp theo ba phần [; head != NULL; head = head->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; head != NULL; head = head->next) {
        const Booking *b = &head->data;
        // Kiểm tra điều kiện [!isActive(b->status)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!isActive(b->status)) continue;
        // Kiểm tra điều kiện [!equalsIgnoreCase(b->roomId, roomId)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!equalsIgnoreCase(b->roomId, roomId)) continue;
        // Kiểm tra điều kiện [isDateRangeOverlap(in, out, b->checkIn, b->checkOut)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (isDateRangeOverlap(in, out, b->checkIn, b->checkOut)) return 0;
    }
    // Trả mã kết quả [1] cho hàm gọi để báo trạng thái xử lý.
    return 1;
}

// Hàm [countAvailable] đếm đệ quy số phòng còn trống.
int countAvailable(const RoomNode *p, Date in, Date out, const BookingNode *bk) {
    // Kiểm tra điều kiện [p == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (p == NULL) return 0;
    return (isRoomAvailable(bk, p->data.roomId, in, out) ? 1 : 0)
           + countAvailable(p->next, in, out, bk);
}

// Hàm [listAvailableRooms] in phòng không bị đặt trong khoảng ngày.
int listAvailableRooms(const RoomNode *rooms, const BookingNode *bookings, Date in, Date out) {
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count = 0;
    // Gọi hàm [printRoomHeader] để in tiêu đề các cột của bảng phòng.
    printRoomHeader();
    // Lặp theo ba phần [; rooms != NULL; rooms = rooms->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; rooms != NULL; rooms = rooms->next) {
        // Kiểm tra điều kiện [isRoomAvailable(bookings, rooms->data.roomId, in, out)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (isRoomAvailable(bookings, rooms->data.roomId, in, out)) {
            // Gọi hàm [printRoomRow] để in thông tin của một phòng thành một dòng.
            printRoomRow(&rooms->data);
            // Cập nhật biến [count] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
            count++;
        }
    }
    // Hiển thị nội dung [+------------+------+-----------------+----------------------------------------------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+------------+------+-----------------+----------------------------------------------------+\n");
    // Hiển thị nội dung [Co %d phong trong.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Co %d phong trong.\n", count);
    // Trả giá trị [count] về cho hàm đã gọi dòng này.
    return count;
}

// Hàm [hasActiveBookingForRoom] kiểm tra phòng có đặt chỗ còn hiệu lực.
int hasActiveBookingForRoom(const BookingNode *head, const char *roomId) {
    // Lặp theo ba phần [; head != NULL; head = head->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; head != NULL; head = head->next) {
        // Kiểm tra điều kiện [isActive(head->data.status) && equalsIgnoreCase(head->data.roomId, roomId)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (isActive(head->data.status) && equalsIgnoreCase(head->data.roomId, roomId)) return 1;
    }
    // Trả mã kết quả [0] cho hàm gọi để báo trạng thái xử lý.
    return 0;
}

// Hàm [hasActiveBookingForGuest] kiểm tra khách có đặt chỗ còn hiệu lực.
int hasActiveBookingForGuest(const BookingNode *head, const char *guestId) {
    // Lặp theo ba phần [; head != NULL; head = head->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; head != NULL; head = head->next) {
        // Kiểm tra điều kiện [isActive(head->data.status) && equalsIgnoreCase(head->data.guestId, guestId)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (isActive(head->data.status) && equalsIgnoreCase(head->data.guestId, guestId)) return 1;
    }
    // Trả mã kết quả [0] cho hàm gọi để báo trạng thái xử lý.
    return 0;
}

BookingNode *findBookingById(BookingNode *head, int bookingId) {
    // Lặp theo ba phần [; head != NULL; head = head->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; head != NULL; head = head->next) {
        // Kiểm tra điều kiện [head->data.bookingId == bookingId]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (head->data.bookingId == bookingId) return head;
    }
    // Báo rằng không tìm thấy dữ liệu hoặc không thể trả về một đối tượng.
    return NULL;
}

// Hàm [nextBookingId] tạo mã phiếu kế tiếp dựa trên mã lớn nhất hiện có.
int nextBookingId(const BookingNode *head) {
    // Khai báo biến [maxId] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int maxId = 0;
    // Lặp theo ba phần [; head != NULL; head = head->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; head != NULL; head = head->next) {
        // Kiểm tra điều kiện [head->data.bookingId > maxId]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (head->data.bookingId > maxId) maxId = head->data.bookingId;
    }
    // Trả giá trị [maxId + 1] về cho hàm đã gọi dòng này.
    return maxId + 1;
}

/* Tinh tien phong, giam gia o dai ngay va phu thu neu tra muon; tra ve tong tien. */
// Hàm [calcAmount] tính tiền phòng, giảm giá, phụ thu và tổng tiền.
double calcAmount(const Booking *b, double pricePerNight, Date actualOut, Invoice *inv) {
    // Khai báo biến [tmp] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Invoice tmp;
    // Dùng bộ nhớ do caller cấp nếu có; nếu không, dùng biến tạm cục bộ.
    Invoice *v = (inv != NULL) ? inv : &tmp;
    // Khai báo biến [late] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int late;

    // Ghi [số đêm giữa hai ngày] vào trường [v->nights] để lưu thông tin của bản ghi.
    v->nights = nightsBetween(b->checkIn, b->checkOut);
    // Tiền phòng trước giảm giá bằng số đêm nhân giá một đêm.
    v->roomCharge = v->nights * pricePerNight;
    // Giảm giá theo tỷ lệ cấu hình nếu thời gian ở đạt ngưỡng dài ngày.
    v->discount = (v->nights >= LONG_STAY_NIGHTS) ? v->roomCharge * LONG_STAY_DISCOUNT : 0.0;

    // Lưu [số đêm của khoảng lưu trú] vào biến [late] để các lệnh sau sử dụng.
    late = nightsBetween(b->checkOut, actualOut);
    // Chặn số ngày trễ âm: trả sớm hoặc đúng hạn được tính là 0 ngày trễ.
    v->lateDays = late > 0 ? late : 0;
    // Mỗi ngày trễ bị tính thêm một đêm tiền phòng cộng tỷ lệ phụ thu.
    v->lateFee = v->lateDays * pricePerNight * (1.0 + LATE_FEE_RATE);

    // Tổng phải trả = tiền phòng trừ giảm giá rồi cộng phí trả muộn.
    v->total = v->roomCharge - v->discount + v->lateFee;
    // Trả giá trị [v->total] về cho hàm đã gọi dòng này.
    return v->total;
}

/* Tao phieu sau khi kiem tra phong/khach ton tai, ngay hop le va khong trung lich. */
// Hàm [createBooking] kiểm tra điều kiện rồi tạo phiếu đặt mới.
int createBooking(BookingNode **head, RoomNode *rooms, GuestNode *guests,
                  const char *roomId, const char *guestId,
                  Date in, Date out, int *newId) {
    RoomNode *room;
    GuestNode *guest;
    BookingNode *node, *p;

    // Lưu [nút phòng có mã cần tìm, hoặc NULL nếu không tồn tại] vào biến [room] để các lệnh sau sử dụng.
    room = findRoomById(rooms, roomId);
    // Kiểm tra điều kiện [room == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (room == NULL) return BK_ERR_ROOM;
    // Lưu [nút khách có mã cần tìm, hoặc NULL nếu không tồn tại] vào biến [guest] để các lệnh sau sử dụng.
    guest = findGuestById(guests, guestId);
    // Kiểm tra điều kiện [guest == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (guest == NULL) return BK_ERR_GUEST;
    // Kiểm tra điều kiện [!isValidDate(in) || !isValidDate(out) || compareDate(out, in) <= 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (!isValidDate(in) || !isValidDate(out) || compareDate(out, in) <= 0) return BK_ERR_DATE;
    // Kiểm tra điều kiện [nightsBetween(in, out) > MAX_NIGHTS]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (nightsBetween(in, out) > MAX_NIGHTS) return BK_ERR_TOO_LONG;
    // Kiểm tra điều kiện [!isRoomAvailable(*head, room->data.roomId, in, out)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (!isRoomAvailable(*head, room->data.roomId, in, out)) return BK_ERR_CONFLICT;

    // Lưu [giá trị biểu thức [(BookingNode *)malloc(sizeof(BookingNode))]] vào biến [node] để các lệnh sau sử dụng.
    node = (BookingNode *)malloc(sizeof(BookingNode));
    // Kiểm tra điều kiện [node == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (node == NULL) return BK_ERR_MEMORY;

    // Đặt toàn bộ byte của cấu trúc về 0 để bắt đầu với dữ liệu sạch.
    memset(&node->data, 0, sizeof(Booking));
    node->data.bookingId = nextBookingId(*head);
    // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
    strcpy(node->data.roomId, room->data.roomId);
    // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
    strcpy(node->data.guestId, guest->data.guestId);
    node->data.checkIn = in;
    node->data.checkOut = out;
    node->data.status = BK_BOOKED;
    node->data.totalAmount = calcAmount(&node->data, room->data.pricePerNight, out, NULL);
    // Đặt nút phiếu mới làm nút cuối danh sách.
    node->next = NULL;

    // Kiểm tra điều kiện [*head == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (*head == NULL) {
        *head = node;
    } else {
        // Lặp theo ba phần [p = *head; p->next != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
        for (p = *head; p->next != NULL; p = p->next) { }
        // Nối phiếu mới sau nút cuối hiện tại.
        p->next = node;
    }
    // Kiểm tra điều kiện [newId != NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (newId != NULL) *newId = node->data.bookingId;
    // Trả mã kết quả [BK_OK] cho hàm gọi để báo trạng thái xử lý.
    return BK_OK;
}

/* Chuyen phieu tu Da dat sang Dang o; cac trang thai khac khong the nhan phong. */
// Hàm [checkIn] chuyển phiếu từ đã đặt sang đang ở.
int checkIn(BookingNode *head, int bookingId) {
    BookingNode *b = findBookingById(head, bookingId);
    // Kiểm tra điều kiện [b == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (b == NULL) return BK_ERR_NOT_FOUND;
    // Kiểm tra điều kiện [b->data.status != BK_BOOKED]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (b->data.status != BK_BOOKED) return BK_ERR_STATUS;
    b->data.status = BK_STAYING;
    // Trả mã kết quả [BK_OK] cho hàm gọi để báo trạng thái xử lý.
    return BK_OK;
}

/* Tinh hoa don, cap nhat ngay tra neu tra muon va chuyen phieu sang Da tra. */
// Hàm [checkOut] tính hóa đơn và chốt phiếu đã trả.
int checkOut(BookingNode *head, RoomNode *rooms, int bookingId, Date actualOut, Invoice *inv) {
    BookingNode *b = findBookingById(head, bookingId);
    RoomNode *room;
    // Kiểm tra điều kiện [b == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (b == NULL) return BK_ERR_NOT_FOUND;
    // Kiểm tra điều kiện [b->data.status != BK_STAYING]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (b->data.status != BK_STAYING) return BK_ERR_STATUS;
    // Kiểm tra điều kiện [!isValidDate(actualOut) || compareDate(actualOut, b->data.checkIn) < 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (!isValidDate(actualOut) || compareDate(actualOut, b->data.checkIn) < 0) return BK_ERR_DATE;
    // Lưu [nút phòng có mã cần tìm, hoặc NULL nếu không tồn tại] vào biến [room] để các lệnh sau sử dụng.
    room = findRoomById(rooms, b->data.roomId);
    // Kiểm tra điều kiện [room == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (room == NULL) return BK_ERR_ROOM;

    b->data.totalAmount = calcAmount(&b->data, room->data.pricePerNight, actualOut, inv);
    // Kiểm tra điều kiện [compareDate(actualOut, b->data.checkOut) > 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (compareDate(actualOut, b->data.checkOut) > 0) b->data.checkOut = actualOut;
    b->data.status = BK_CHECKEDOUT;
    // Trả mã kết quả [BK_OK] cho hàm gọi để báo trạng thái xử lý.
    return BK_OK;
}

/* Huy phieu chi khi phieu con o trang thai Da dat. */
// Hàm [cancelBooking] hủy phiếu khi trạng thái cho phép.
int cancelBooking(BookingNode *head, int bookingId) {
    BookingNode *b = findBookingById(head, bookingId);
    // Kiểm tra điều kiện [b == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (b == NULL) return BK_ERR_NOT_FOUND;
    // Kiểm tra điều kiện [b->data.status != BK_BOOKED]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (b->data.status != BK_BOOKED) return BK_ERR_STATUS;
    b->data.status = BK_CANCELLED;
    b->data.totalAmount = 0.0;
    // Trả mã kết quả [BK_OK] cho hàm gọi để báo trạng thái xử lý.
    return BK_OK;
}

void printInvoice(const Booking *b, const Room *r, const Guest *g, const Invoice *inv) {
    // Khai báo biến [in] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char in[DATE_STR_LEN], out[DATE_STR_LEN];
    // Gọi hàm [formatDate] để định dạng ngày thành chuỗi dd/mm/yyyy để in hoặc ghép vào thông báo.
    formatDate(b->checkIn, in, sizeof(in));
    // Gọi hàm [formatDate] để định dạng ngày thành chuỗi dd/mm/yyyy để in hoặc ghép vào thông báo.
    formatDate(b->checkOut, out, sizeof(out));

    // Hiển thị nội dung [==================== HOA DON THANH TOAN ====================] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("\n==================== HOA DON THANH TOAN ====================\n");
    // Hiển thị nội dung [  Ma phieu      : %d] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("  Ma phieu      : %d\n", b->bookingId);
    // Hiển thị nội dung [  Khach hang    : %s (%s)] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("  Khach hang    : %s (%s)\n", g != NULL ? g->fullName : "(da xoa)", b->guestId);
    // Kiểm tra điều kiện [g != NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (g != NULL) printf("  CCCD / SDT    : %s / %s\n", g->idCard, g->phone);
    // Hiển thị nội dung [  Phong         : %s - %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("  Phong         : %s - %s\n", b->roomId, r != NULL ? roomTypeName(r->type) : "?");
    // Hiển thị nội dung [  Ngay nhan     : %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("  Ngay nhan     : %s\n", in);
    // Hiển thị nội dung [  Ngay tra      : %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("  Ngay tra      : %s\n", out);
    // Hiển thị nội dung [------------------------------------------------------------] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("------------------------------------------------------------\n");
    // Hiển thị nội dung [  So dem dat    : %d x %.0f = %15.0f VND] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("  So dem dat    : %d x %.0f = %15.0f VND\n",
           inv->nights, r != NULL ? r->pricePerNight : 0.0, inv->roomCharge);
    // Kiểm tra điều kiện [inv->discount > 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (inv->discount > 0)
        // Hiển thị nội dung [  Giam gia %2.0f%% (>= %d dem)       : -%14.0f VND] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("  Giam gia %2.0f%% (>= %d dem)       : -%14.0f VND\n",
               LONG_STAY_DISCOUNT * 100, LONG_STAY_NIGHTS, inv->discount);
    // Kiểm tra điều kiện [inv->lateDays > 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (inv->lateDays > 0)
        // Hiển thị nội dung [  Tra muon %d ngay (phu thu %.0f%%)  : +%14.0f VND] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("  Tra muon %d ngay (phu thu %.0f%%)  : +%14.0f VND\n",
               inv->lateDays, LATE_FEE_RATE * 100, inv->lateFee);
    // Hiển thị nội dung [------------------------------------------------------------] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("------------------------------------------------------------\n");
    // Hiển thị nội dung [  TONG CONG                       : %15.0f VND] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("  TONG CONG                       : %15.0f VND\n", inv->total);
    // Hiển thị nội dung [============================================================] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("============================================================\n");
}

void printBookingHeader(void) {
    // Hiển thị nội dung [+-------+------------+-------------+------------+------------+-------+----------+-----------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-------+------------+-------------+------------+------------+-------+----------+-----------------+\n");
    // Hiển thị nội dung [| %-5s | %-10s | %-11s | %-10s | %-10s | %5s | %-8s | %15s |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("| %-5s | %-10s | %-11s | %-10s | %-10s | %5s | %-8s | %15s |\n",
           "Ma", "Phong", "Ma khach", "Ngay nhan", "Ngay tra", "Dem", "T.thai", "So tien (VND)");
    // Hiển thị nội dung [+-------+------------+-------------+------------+------------+-------+----------+-----------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-------+------------+-------------+------------+------------+-------+----------+-----------------+\n");
}

void printBookingRow(const Booking *b) {
    // Khai báo biến [in] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char in[DATE_STR_LEN], out[DATE_STR_LEN];
    // Gọi hàm [formatDate] để định dạng ngày thành chuỗi dd/mm/yyyy để in hoặc ghép vào thông báo.
    formatDate(b->checkIn, in, sizeof(in));
    // Gọi hàm [formatDate] để định dạng ngày thành chuỗi dd/mm/yyyy để in hoặc ghép vào thông báo.
    formatDate(b->checkOut, out, sizeof(out));
    // Hiển thị nội dung [| %5d | %-10s | %-11s | %-10s | %-10s | %5d | %-8s | %15.0f |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("| %5d | %-10s | %-11s | %-10s | %-10s | %5d | %-8s | %15.0f |\n",
           b->bookingId, b->roomId, b->guestId, in, out,
           // Gọi hàm [nightsBetween] để tính số đêm giữa ngày nhận và ngày trả.
           nightsBetween(b->checkIn, b->checkOut), bookingStatusName(b->status), b->totalAmount);
}

void printBookingFooter(void) {
    // Hiển thị nội dung [+-------+------------+-------------+------------+------------+-------+----------+-----------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-------+------------+-------------+------------+------------+-------+----------+-----------------+\n");
}

// Hàm [printBookings] in danh sách phiếu, có thể lọc theo trạng thái.
int printBookings(const BookingNode *head, int statusFilter) {
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count = 0;
    // Gọi hàm [printBookingHeader] để in tiêu đề các cột của bảng phiếu.
    printBookingHeader();
    // Lặp theo ba phần [; head != NULL; head = head->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; head != NULL; head = head->next) {
        // Kiểm tra điều kiện [statusFilter < 0 || head->data.status == statusFilter]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (statusFilter < 0 || head->data.status == statusFilter) {
            // Gọi hàm [printBookingRow] để in một dòng thông tin phiếu.
            printBookingRow(&head->data);
            // Cập nhật biến [count] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
            count++;
        }
    }
    // Gọi hàm [printBookingFooter] để in đường kẻ kết thúc bảng phiếu.
    printBookingFooter();
    // Hiển thị nội dung [Tong: %d phieu.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Tong: %d phieu.\n", count);
    // Trả giá trị [count] về cho hàm đã gọi dòng này.
    return count;
}

/* In khi quay lui de phieu moi nhat hien truoc. */
// Hàm [printHistoryReverse] in lịch sử đặt phòng mới nhất trước bằng đệ quy.
int printHistoryReverse(const BookingNode *p, const char *guestId) {
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count;
    // Kiểm tra điều kiện [p == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (p == NULL) return 0;
    // Lưu [giá trị biểu thức [printHistoryReverse(p->next, guestId)]] vào biến [count] để các lệnh sau sử dụng.
    count = printHistoryReverse(p->next, guestId);
    // Kiểm tra điều kiện [equalsIgnoreCase(p->data.guestId, guestId)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (equalsIgnoreCase(p->data.guestId, guestId)) {
        // Gọi hàm [printBookingRow] để in một dòng thông tin phiếu.
        printBookingRow(&p->data);
        // Cập nhật biến [count] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
        count++;
    }
    // Trả giá trị [count] về cho hàm đã gọi dòng này.
    return count;
}

void freeBookingList(BookingNode **head) {
    BookingNode *p = *head, *next;
    // Lặp lại khối lệnh khi điều kiện [p != NULL] vẫn đúng.
    while (p != NULL) {
        // Lưu [giá trị biểu thức [p->next]] vào biến [next] để các lệnh sau sử dụng.
        next = p->next;
        // Giải phóng nút nhớ động này sau khi đã gỡ nó khỏi danh sách.
        free(p);
        // Lưu [giá trị biểu thức [next]] vào biến [p] để các lệnh sau sử dụng.
        p = next;
    }
    *head = NULL;
}
