#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "room.h"
#include "utils.h"

const char *roomTypeName(int type) {
    // Đọc giá trị [type] để chọn một nhánh case tương ứng bên dưới.
    switch (type) {
        // Đây là nhánh xử lý khi giá trị switch bằng [ROOM_SINGLE].
        case ROOM_SINGLE: return "Don";
        // Đây là nhánh xử lý khi giá trị switch bằng [ROOM_DOUBLE].
        case ROOM_DOUBLE: return "Doi";
        // Đây là nhánh xử lý khi giá trị switch bằng [ROOM_VIP].
        case ROOM_VIP:    return "VIP";
        // Đây là nhánh dự phòng nếu không có case nào khớp.
        default:          return "?";
    }
}

/* Tim phong theo ma, khong phan biet chu hoa/chu thuong. */
RoomNode *findRoomById(RoomNode *head, const char *roomId) {
    RoomNode *p;
    // Lặp theo ba phần [p = head; p != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (p = head; p != NULL; p = p->next) {
        // Kiểm tra điều kiện [equalsIgnoreCase(p->data.roomId, roomId)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (equalsIgnoreCase(p->data.roomId, roomId)) return p;
    }
    // Báo rằng không tìm thấy dữ liệu hoặc không thể trả về một đối tượng.
    return NULL;
}

/* Them phong vao cuoi danh sach; tu choi ma trung va bao loi neu het bo nho. */
// Hàm [addRoom] kiểm tra trùng mã rồi thêm phòng vào danh sách.
int addRoom(RoomNode **head, Room r) {
    RoomNode *node, *p;
    // Kiểm tra điều kiện [findRoomById(*head, r.roomId) != NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (findRoomById(*head, r.roomId) != NULL) return LIST_DUPLICATE;

    // Lưu [giá trị biểu thức [(RoomNode *)malloc(sizeof(RoomNode))]] vào biến [node] để các lệnh sau sử dụng.
    node = (RoomNode *)malloc(sizeof(RoomNode));
    // Kiểm tra điều kiện [node == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (node == NULL) return LIST_NO_MEMORY;
    // Sao chép toàn bộ dữ liệu phòng đã nhập vào nút vừa cấp phát.
    node->data = r;
    // Đánh dấu nút mới là nút cuối danh sách vì hiện chưa có nút theo sau.
    node->next = NULL;

    // Kiểm tra điều kiện [*head == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (*head == NULL) {
        *head = node;
    } else {
        // Lặp theo ba phần [p = *head; p->next != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
        for (p = *head; p->next != NULL; p = p->next) { }
        // Nối nút phòng mới sau nút cuối hiện tại.
        p->next = node;
    }
    // Trả mã kết quả [LIST_OK] cho hàm gọi để báo trạng thái xử lý.
    return LIST_OK;
}

/* Tim va go nut phong khoi danh sach, sau do giai phong bo nho cua nut do. */
// Hàm [deleteRoom] gỡ phòng khỏi danh sách và giải phóng bộ nhớ.
int deleteRoom(RoomNode **head, const char *roomId) {
    RoomNode **pp = head;
    // Lặp lại khối lệnh khi điều kiện [*pp != NULL] vẫn đúng.
    while (*pp != NULL) {
        // Kiểm tra điều kiện [equalsIgnoreCase((*pp)->data.roomId, roomId)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (equalsIgnoreCase((*pp)->data.roomId, roomId)) {
            RoomNode *victim = *pp;
            *pp = victim->next;
            // Giải phóng nút nhớ động này sau khi đã gỡ nó khỏi danh sách.
            free(victim);
            // Trả mã kết quả [LIST_OK] cho hàm gọi để báo trạng thái xử lý.
            return LIST_OK;
        }
        // Lưu [giá trị biểu thức [&(*pp)->next]] vào biến [pp] để các lệnh sau sử dụng.
        pp = &(*pp)->next;
    }
    // Trả mã kết quả [LIST_NOT_FOUND] cho hàm gọi để báo trạng thái xử lý.
    return LIST_NOT_FOUND;
}

void updateRoom(Room *r, int type, double pricePerNight, const char *note) {
    // Thay loại phòng bằng loại mới người dùng chọn.
    r->type = type;
    // Thay giá thuê một đêm bằng giá mới.
    r->pricePerNight = pricePerNight;
    // Chép chuỗi nhưng giới hạn số ký tự để không ghi tràn bộ đệm.
    strncpy(r->note, note, ROOM_NOTE_LEN - 1);
    r->note[ROOM_NOTE_LEN - 1] = '\0';
}

// Hàm [countRooms] đếm số phòng trong danh sách.
int countRooms(const RoomNode *head) {
    // Khai báo biến [n] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int n = 0;
    // Lặp theo ba phần [; head != NULL; head = head->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; head != NULL; head = head->next) n++;
    // Trả giá trị [n] về cho hàm đã gọi dòng này.
    return n;
}

// Hàm [cmpRoomById] thực hiện xử lý trong phần thân hàm.
int cmpRoomById(const Room *a, const Room *b) {
    // Trả giá trị [strcmp(a->roomId, b->roomId)] về cho hàm đã gọi dòng này.
    return strcmp(a->roomId, b->roomId);
}

// Hàm [cmpRoomByPriceAsc] thực hiện xử lý trong phần thân hàm.
int cmpRoomByPriceAsc(const Room *a, const Room *b) {
    // Kiểm tra điều kiện [a->pricePerNight < b->pricePerNight]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (a->pricePerNight < b->pricePerNight) return -1;
    // Kiểm tra điều kiện [a->pricePerNight > b->pricePerNight]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (a->pricePerNight > b->pricePerNight) return 1;
    // Trả giá trị [cmpRoomById(a, b)] về cho hàm đã gọi dòng này.
    return cmpRoomById(a, b);
}

// Hàm [cmpRoomByPriceDesc] thực hiện xử lý trong phần thân hàm.
int cmpRoomByPriceDesc(const Room *a, const Room *b) {
    // Trả giá trị [cmpRoomByPriceAsc(b, a)] về cho hàm đã gọi dòng này.
    return cmpRoomByPriceAsc(b, a);
}

// Hàm [cmpRoomByTypeThenPrice] thực hiện xử lý trong phần thân hàm.
int cmpRoomByTypeThenPrice(const Room *a, const Room *b) {
    // Kiểm tra điều kiện [a->type != b->type]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (a->type != b->type) return a->type - b->type;
    // Trả giá trị [cmpRoomByPriceAsc(a, b)] về cho hàm đã gọi dòng này.
    return cmpRoomByPriceAsc(a, b);
}

/* Sap xep danh sach bang selection sort; ham so sanh duoc truyen tu ben ngoai. */
void sortRooms(RoomNode *head, RoomCompareFn cmp) {
    RoomNode *i, *j, *best;
    // Khai báo biến [tmp] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Room tmp;
    // Lặp theo ba phần [i = head; i != NULL; i = i->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (i = head; i != NULL; i = i->next) {
        // Lưu [giá trị biểu thức [i]] vào biến [best] để các lệnh sau sử dụng.
        best = i;
        // Lặp theo ba phần [j = i->next; j != NULL; j = j->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
        for (j = i->next; j != NULL; j = j->next) {
            // Kiểm tra điều kiện [cmp(&j->data, &best->data) < 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
            if (cmp(&j->data, &best->data) < 0) best = j;
        }
        // Kiểm tra điều kiện [best != i]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (best != i) {
            // Lưu [giá trị biểu thức [i->data]] vào biến [tmp] để các lệnh sau sử dụng.
            tmp = i->data;
            // Đổi chỗ dữ liệu của vị trí hiện tại với phòng nhỏ nhất tìm được.
            i->data = best->data;
            // Hoàn tất phép đổi chỗ bằng cách đưa dữ liệu cũ vào vị trí best.
            best->data = tmp;
        }
    }
}

// Hàm [filterRoomByType] thực hiện xử lý trong phần thân hàm.
int filterRoomByType(const Room *r, const void *ctx) {
    // Trả giá trị [r->type == *(const int *)ctx] về cho hàm đã gọi dòng này.
    return r->type == *(const int *)ctx;
}

// Hàm [filterRoomByPrice] thực hiện xử lý trong phần thân hàm.
int filterRoomByPrice(const Room *r, const void *ctx) {
    const PriceRange *range = (const PriceRange *)ctx;
    // Trả giá trị [r->pricePerNight >= range->minPrice && r->pricePerNight <= range->maxPrice] về cho hàm đã gọi dòng này.
    return r->pricePerNight >= range->minPrice && r->pricePerNight <= range->maxPrice;
}

// Hàm [filterRoomByIdContains] thực hiện xử lý trong phần thân hàm.
int filterRoomByIdContains(const Room *r, const void *ctx) {
    // Trả giá trị [containsIgnoreCase(r->roomId, (const char *)ctx)] về cho hàm đã gọi dòng này.
    return containsIgnoreCase(r->roomId, (const char *)ctx);
}

void printRoomHeader(void) {
    // Hiển thị nội dung [+------------+------+-----------------+----------------------------------------------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+------------+------+-----------------+----------------------------------------------------+\n");
    // Hiển thị nội dung [| %-10s | %-4s | %15s | %-50s |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("| %-10s | %-4s | %15s | %-50s |\n", "Ma phong", "Loai", "Gia/dem (VND)", "Ghi chu");
    // Hiển thị nội dung [+------------+------+-----------------+----------------------------------------------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+------------+------+-----------------+----------------------------------------------------+\n");
}

void printRoomRow(const Room *r) {
    // Hiển thị nội dung [| %-10s | %-4s | %15.0f | %-50s |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("| %-10s | %-4s | %15.0f | %-50s |\n",
           r->roomId, roomTypeName(r->type), r->pricePerNight, r->note);
}

static void printRoomFooter(void) {
    // Hiển thị nội dung [+------------+------+-----------------+----------------------------------------------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+------------+------+-----------------+----------------------------------------------------+\n");
}

/* In cac phong thoa bo loc pred; pred NULL co nghia la in tat ca phong. */
// Hàm [printRoomsIf] lọc và in các phòng phù hợp.
int printRoomsIf(const RoomNode *head, RoomFilterFn pred, const void *ctx) {
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count = 0;
    // Gọi hàm [printRoomHeader] để in tiêu đề các cột của bảng phòng.
    printRoomHeader();
    // Lặp theo ba phần [; head != NULL; head = head->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; head != NULL; head = head->next) {
        // Kiểm tra điều kiện [pred == NULL || pred(&head->data, ctx)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (pred == NULL || pred(&head->data, ctx)) {
            // Gọi hàm [printRoomRow] để in thông tin của một phòng thành một dòng.
            printRoomRow(&head->data);
            // Cập nhật biến [count] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
            count++;
        }
    }
    // Gọi hàm [printRoomFooter] để in đường kẻ kết thúc bảng phòng.
    printRoomFooter();
    // Hiển thị nội dung [Tong: %d phong.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Tong: %d phong.\n", count);
    // Trả giá trị [count] về cho hàm đã gọi dòng này.
    return count;
}

void printRooms(const RoomNode *head) {
    // Gọi hàm [printRoomsIf] để lọc danh sách phòng theo điều kiện rồi in các phòng phù hợp.
    printRoomsIf(head, NULL, NULL);
}

/* Giai phong lan luot moi nut va dat con tro dau danh sach ve NULL. */
void freeRoomList(RoomNode **head) {
    RoomNode *p = *head, *next;
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
