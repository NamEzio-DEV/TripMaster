#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "fileio.h"

#ifdef _WIN32
#include <direct.h>
#define MAKE_DIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#define MAKE_DIR(path) mkdir(path, 0755)
#endif

#define MAX_RECORDS 1000000

static char g_dataDir[DIR_LEN] = DATA_DIR;

void setDataDir(const char *dir) {
    // Khai báo biến [len] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    size_t len;
    // Kiểm tra điều kiện [dir == NULL || dir[0] == '\0']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (dir == NULL || dir[0] == '\0') return;
    // Lưu [giá trị biểu thức [strlen(dir)]] vào biến [len] để các lệnh sau sử dụng.
    len = strlen(dir);
    // Kiểm tra điều kiện [len + 2 > sizeof(g_dataDir)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (len + 2 > sizeof(g_dataDir)) {
        // Hiển thị nội dung [[!] Duong dan thu muc qua dai, dung mac dinh %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Duong dan thu muc qua dai, dung mac dinh %s\n", DATA_DIR);
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
    // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
    strcpy(g_dataDir, dir);
    // Kiểm tra điều kiện [dir[len - 1] != '/' && dir[len - 1] != '\\']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (dir[len - 1] != '/' && dir[len - 1] != '\\') strcat(g_dataDir, "/");
}

const char *getDataDir(void) {
    // Trả giá trị [g_dataDir] về cho hàm đã gọi dòng này.
    return g_dataDir;
}

static void buildPath(char *out, size_t size, const char *fileName) {
    // Gọi hàm [snprintf] để ghép chuỗi theo mẫu vào bộ đệm và giới hạn số ký tự để tránh ghi tràn.
    snprintf(out, size, "%s%s", g_dataDir, fileName);
}

/* Tao thu muc du lieu neu chua ton tai; thu muc co san thi tiep tuc binh thuong. */
static void ensureDataDir(void) {
    // Khai báo biến [dir] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char dir[DIR_LEN];
    // Khai báo biến [len] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    size_t len;
    // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
    strcpy(dir, g_dataDir);
    // Lưu [giá trị biểu thức [strlen(dir)]] vào biến [len] để các lệnh sau sử dụng.
    len = strlen(dir);
    // Lặp lại khối lệnh khi điều kiện [len > 1 && (dir[len - 1] == '/' || dir[len - 1] == '\\')] vẫn đúng.
    while (len > 1 && (dir[len - 1] == '/' || dir[len - 1] == '\\')) dir[--len] = '\0';
    // Kiểm tra điều kiện [MAKE_DIR(dir) != 0 && errno != EEXIST]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (MAKE_DIR(dir) != 0 && errno != EEXIST) {

    }
}

static FILE *openTemp(const char *fileName, char *tmpPath, size_t size) {
    FILE *f;
    // Gọi hàm [ensureDataDir] để đảm bảo thư mục lưu dữ liệu đã tồn tại.
    ensureDataDir();
    // Gọi hàm [snprintf] để ghép chuỗi theo mẫu vào bộ đệm và giới hạn số ký tự để tránh ghi tràn.
    snprintf(tmpPath, size, "%s%s.tmp", g_dataDir, fileName);
    // Lưu [giá trị biểu thức [fopen(tmpPath, "wb")]] vào biến [f] để các lệnh sau sử dụng.
    f = fopen(tmpPath, "wb");
    // Kiểm tra điều kiện [f == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (f == NULL) printf("[!] Khong mo duoc file %s de ghi.\n", tmpPath);
    // Trả giá trị [f] về cho hàm đã gọi dòng này.
    return f;
}

/* Ghi vao file .tmp truoc, ghi xong moi doi ten thanh file chinh. */
static int commitTemp(FILE *f, const char *fileName, const char *tmpPath, int ok) {
    // Khai báo biến [path] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char path[PATH_LEN];
    // Kiểm tra điều kiện [fclose(f) != 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (fclose(f) != 0) ok = 0;
    // Kiểm tra điều kiện [!ok]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (!ok) {
        // Gọi hàm [remove] để xóa tệp tạm hoặc tệp cũ theo đường dẫn được truyền vào.
        remove(tmpPath);
        // Hiển thị nội dung [[!] Loi khi ghi file %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Loi khi ghi file %s.\n", fileName);
        // Trả mã kết quả [0] cho hàm gọi để báo trạng thái xử lý.
        return 0;
    }
    // Gọi hàm [buildPath] để ghép thư mục dữ liệu với tên tệp để tạo đường dẫn đầy đủ.
    buildPath(path, sizeof(path), fileName);
    // Gọi hàm [remove] để xóa tệp tạm hoặc tệp cũ theo đường dẫn được truyền vào.
    remove(path);
    // Kiểm tra điều kiện [rename(tmpPath, path) != 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (rename(tmpPath, path) != 0) {
        // Hiển thị nội dung [[!] Khong doi ten duoc %s thanh %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] Khong doi ten duoc %s thanh %s.\n", tmpPath, path);
        // Trả mã kết quả [0] cho hàm gọi để báo trạng thái xử lý.
        return 0;
    }
    // Trả mã kết quả [1] cho hàm gọi để báo trạng thái xử lý.
    return 1;
}

static FILE *openForRead(const char *fileName, int *count) {
    // Khai báo biến [path] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char path[PATH_LEN];
    FILE *f;
    // Gọi hàm [buildPath] để ghép thư mục dữ liệu với tên tệp để tạo đường dẫn đầy đủ.
    buildPath(path, sizeof(path), fileName);
    // Lưu [giá trị biểu thức [fopen(path, "rb")]] vào biến [f] để các lệnh sau sử dụng.
    f = fopen(path, "rb");
    // Kiểm tra điều kiện [f == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (f == NULL) return NULL;
    // Kiểm tra điều kiện [fread(count, sizeof(int), 1, f) != 1 || *count < 0 || *count > MAX_RECORDS]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (fread(count, sizeof(int), 1, f) != 1 || *count < 0 || *count > MAX_RECORDS) {
        // Hiển thị nội dung [[!] File %s bi hong hoac rong, bo qua.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("[!] File %s bi hong hoac rong, bo qua.\n", path);
        // Gọi hàm [fclose] để đóng tệp và kiểm tra việc ghi đã hoàn tất.
        fclose(f);
        // Báo rằng không tìm thấy dữ liệu hoặc không thể trả về một đối tượng.
        return NULL;
    }
    // Trả giá trị [f] về cho hàm đã gọi dòng này.
    return f;
}

/* Luu so luong va tung Room; chi ghi du lieu phong, khong ghi con tro next. */
// Hàm [saveRooms] ghi danh sách phòng xuống tệp.
int saveRooms(const RoomNode *head) {
    // Khai báo biến [tmp] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char tmp[PATH_LEN];
    const RoomNode *p;
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count = countRooms(head), ok;
    FILE *f = openTemp(ROOMS_FILE, tmp, sizeof(tmp));
    // Kiểm tra điều kiện [f == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (f == NULL) return 0;
    // Lưu [giá trị biểu thức [fwrite(&count, sizeof(int), 1, f) == 1]] vào biến [ok] để các lệnh sau sử dụng.
    ok = fwrite(&count, sizeof(int), 1, f) == 1;
    // Lặp theo ba phần [p = head; ok && p != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (p = head; ok && p != NULL; p = p->next) {
        // Lưu [giá trị biểu thức [fwrite(&p->data, sizeof(Room), 1, f) == 1]] vào biến [ok] để các lệnh sau sử dụng.
        ok = fwrite(&p->data, sizeof(Room), 1, f) == 1;
    }
    // Trả giá trị [commitTemp(f, ROOMS_FILE, tmp, ok)] về cho hàm đã gọi dòng này.
    return commitTemp(f, ROOMS_FILE, tmp, ok);
}

/* Doc phong tu tep, kiem tra du lieu co ban roi chen vao danh sach trong RAM. */
// Hàm [loadRooms] đọc phòng và bỏ qua bản ghi sai/trùng.
int loadRooms(RoomNode **head) {
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count, i, loaded = 0;
    // Khai báo biến [r] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Room r;
    FILE *f = openForRead(ROOMS_FILE, &count);
    // Kiểm tra điều kiện [f == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (f == NULL) return 0;
    // Lặp theo ba phần [i = 0; i < count; i++]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (i = 0; i < count; i++) {
        // Kiểm tra điều kiện [fread(&r, sizeof(Room), 1, f) != 1]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (fread(&r, sizeof(Room), 1, f) != 1) {
            // Hiển thị nội dung [[!] %s: chi doc duoc %d/%d ban ghi.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("[!] %s: chi doc duoc %d/%d ban ghi.\n", ROOMS_FILE, i, count);
            break;
        }
        /* Dam bao cac truong chuoi co ky tu ket thuc. */
        r.roomId[ROOM_ID_LEN - 1] = '\0';
        r.note[ROOM_NOTE_LEN - 1] = '\0';
        // Kiểm tra điều kiện [r.type < ROOM_SINGLE || r.type > ROOM_VIP || r.roomId[0] == '\0']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (r.type < ROOM_SINGLE || r.type > ROOM_VIP || r.roomId[0] == '\0') continue;
        // Kiểm tra điều kiện [addRoom(head, r) == LIST_OK]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (addRoom(head, r) == LIST_OK) loaded++;
    }
    // Gọi hàm [fclose] để đóng tệp và kiểm tra việc ghi đã hoàn tất.
    fclose(f);
    // Trả giá trị [loaded] về cho hàm đã gọi dòng này.
    return loaded;
}

/* Luu danh sach khach vao tep nhi phan thong qua tep tam an toan. */
// Hàm [saveGuests] ghi danh sách khách xuống tệp.
int saveGuests(const GuestNode *head) {
    // Khai báo biến [tmp] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char tmp[PATH_LEN];
    const GuestNode *p;
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count = countGuests(head), ok;
    FILE *f = openTemp(GUESTS_FILE, tmp, sizeof(tmp));
    // Kiểm tra điều kiện [f == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (f == NULL) return 0;
    // Lưu [giá trị biểu thức [fwrite(&count, sizeof(int), 1, f) == 1]] vào biến [ok] để các lệnh sau sử dụng.
    ok = fwrite(&count, sizeof(int), 1, f) == 1;
    // Lặp theo ba phần [p = head; ok && p != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (p = head; ok && p != NULL; p = p->next) {
        // Lưu [giá trị biểu thức [fwrite(&p->data, sizeof(Guest), 1, f) == 1]] vào biến [ok] để các lệnh sau sử dụng.
        ok = fwrite(&p->data, sizeof(Guest), 1, f) == 1;
    }
    // Trả giá trị [commitTemp(f, GUESTS_FILE, tmp, ok)] về cho hàm đã gọi dòng này.
    return commitTemp(f, GUESTS_FILE, tmp, ok);
}

/* Doc khach tu tep, dam bao cac chuoi ket thuc dung va bo qua ban ghi loi. */
// Hàm [loadGuests] đọc khách và kiểm tra dữ liệu trước khi thêm.
int loadGuests(GuestNode **head) {
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count, i, loaded = 0;
    // Khai báo biến [g] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Guest g;
    FILE *f = openForRead(GUESTS_FILE, &count);
    // Kiểm tra điều kiện [f == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (f == NULL) return 0;
    // Lặp theo ba phần [i = 0; i < count; i++]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (i = 0; i < count; i++) {
        // Kiểm tra điều kiện [fread(&g, sizeof(Guest), 1, f) != 1]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (fread(&g, sizeof(Guest), 1, f) != 1) {
            // Hiển thị nội dung [[!] %s: chi doc duoc %d/%d ban ghi.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("[!] %s: chi doc duoc %d/%d ban ghi.\n", GUESTS_FILE, i, count);
            break;
        }
        /* Dam bao cac truong chuoi co ky tu ket thuc. */
        g.guestId[GUEST_ID_LEN - 1] = '\0';
        g.fullName[GUEST_NAME_LEN - 1] = '\0';
        g.idCard[ID_CARD_LEN - 1] = '\0';
        g.phone[PHONE_LEN - 1] = '\0';
        // Kiểm tra điều kiện [g.guestId[0] == '\0']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (g.guestId[0] == '\0') continue;
        // Kiểm tra điều kiện [addGuest(head, g) == LIST_OK]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (addGuest(head, g) == LIST_OK) loaded++;
    }
    // Gọi hàm [fclose] để đóng tệp và kiểm tra việc ghi đã hoàn tất.
    fclose(f);
    // Trả giá trị [loaded] về cho hàm đã gọi dòng này.
    return loaded;
}

/* Luu so phieu va noi dung tung phieu, khong ghi con tro lien ket danh sach. */
// Hàm [saveBookings] ghi danh sách phiếu xuống tệp.
int saveBookings(const BookingNode *head) {
    // Khai báo biến [tmp] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char tmp[PATH_LEN];
    const BookingNode *p;
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count = 0, ok;
    FILE *f;
    // Lặp theo ba phần [p = head; p != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (p = head; p != NULL; p = p->next) count++;
    // Lưu [giá trị biểu thức [openTemp(BOOKINGS_FILE, tmp, sizeof(tmp))]] vào biến [f] để các lệnh sau sử dụng.
    f = openTemp(BOOKINGS_FILE, tmp, sizeof(tmp));
    // Kiểm tra điều kiện [f == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (f == NULL) return 0;
    // Lưu [giá trị biểu thức [fwrite(&count, sizeof(int), 1, f) == 1]] vào biến [ok] để các lệnh sau sử dụng.
    ok = fwrite(&count, sizeof(int), 1, f) == 1;
    // Lặp theo ba phần [p = head; ok && p != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (p = head; ok && p != NULL; p = p->next) {
        // Lưu [giá trị biểu thức [fwrite(&p->data, sizeof(Booking), 1, f) == 1]] vào biến [ok] để các lệnh sau sử dụng.
        ok = fwrite(&p->data, sizeof(Booking), 1, f) == 1;
    }
    // Trả giá trị [commitTemp(f, BOOKINGS_FILE, tmp, ok)] về cho hàm đã gọi dòng này.
    return commitTemp(f, BOOKINGS_FILE, tmp, ok);
}

/* Nap phieu dat, bo qua ban ghi co trang thai/ngay khong hop le. */
// Hàm [loadBookings] đọc phiếu và bỏ qua ngày/trạng thái không hợp lệ.
int loadBookings(BookingNode **head) {
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count, i, loaded = 0;
    // Khai báo biến [b] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Booking b;
    BookingNode *node, *tail = *head;
    FILE *f = openForRead(BOOKINGS_FILE, &count);
    // Kiểm tra điều kiện [f == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (f == NULL) return 0;
    // Lặp lại khối lệnh khi điều kiện [tail != NULL && tail->next != NULL] vẫn đúng.
    while (tail != NULL && tail->next != NULL) tail = tail->next;

    // Lặp theo ba phần [i = 0; i < count; i++]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (i = 0; i < count; i++) {
        // Kiểm tra điều kiện [fread(&b, sizeof(Booking), 1, f) != 1]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (fread(&b, sizeof(Booking), 1, f) != 1) {
            // Hiển thị nội dung [[!] %s: chi doc duoc %d/%d ban ghi.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("[!] %s: chi doc duoc %d/%d ban ghi.\n", BOOKINGS_FILE, i, count);
            break;
        }
        b.roomId[ROOM_ID_LEN - 1] = '\0';
        b.guestId[GUEST_ID_LEN - 1] = '\0';
        // Kiểm tra điều kiện [b.status < BK_BOOKED || b.status > BK_CANCELLED]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (b.status < BK_BOOKED || b.status > BK_CANCELLED) continue;
        // Kiểm tra điều kiện [!isValidDate(b.checkIn) || !isValidDate(b.checkOut)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!isValidDate(b.checkIn) || !isValidDate(b.checkOut)) continue;

        // Lưu [giá trị biểu thức [(BookingNode *)malloc(sizeof(BookingNode))]] vào biến [node] để các lệnh sau sử dụng.
        node = (BookingNode *)malloc(sizeof(BookingNode));
        // Kiểm tra điều kiện [node == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (node == NULL) {
            // Hiển thị nội dung [[!] Khong du bo nho khi doc %s.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("[!] Khong du bo nho khi doc %s.\n", BOOKINGS_FILE);
            break;
        }
        // Sao chép bản ghi vừa đọc từ tệp vào nút danh sách mới.
        node->data = b;
        // Nút mới hiện là nút cuối nên chưa trỏ tới nút tiếp theo.
        node->next = NULL;
        // Kiểm tra điều kiện [tail == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (tail == NULL) *head = node; else tail->next = node;
        // Lưu [giá trị biểu thức [node]] vào biến [tail] để các lệnh sau sử dụng.
        tail = node;
        // Cập nhật biến [loaded] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
        loaded++;
    }
    // Gọi hàm [fclose] để đóng tệp và kiểm tra việc ghi đã hoàn tất.
    fclose(f);
    // Trả giá trị [loaded] về cho hàm đã gọi dòng này.
    return loaded;
}

// Hàm [saveAll] lưu cả danh sách phòng, khách và phiếu.
int saveAll(const RoomNode *rooms, const GuestNode *guests, const BookingNode *bookings) {
    // Khai báo biến [ok] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int ok = 1;
    ok &= saveRooms(rooms);
    ok &= saveGuests(guests);
    ok &= saveBookings(bookings);
    // Trả giá trị [ok] về cho hàm đã gọi dòng này.
    return ok;
}

// Hàm [loadAll] nạp ba danh sách và báo số dòng đã đọc.
int loadAll(RoomNode **rooms, GuestNode **guests, BookingNode **bookings) {
    // Khai báo biến [nr] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int nr = loadRooms(rooms);
    // Khai báo biến [ng] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int ng = loadGuests(guests);
    // Khai báo biến [nb] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int nb = loadBookings(bookings);
    // Hiển thị nội dung [Thu muc du lieu: %s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Thu muc du lieu: %s\n", g_dataDir);
    // Hiển thị nội dung [Da doc: %d phong, %d khach, %d phieu dat.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Da doc: %d phong, %d khach, %d phieu dat.\n", nr, ng, nb);
    // Trả giá trị [nr + ng + nb] về cho hàm đã gọi dòng này.
    return nr + ng + nb;
}
