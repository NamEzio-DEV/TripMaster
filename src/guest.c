#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "guest.h"
#include "utils.h"

GuestNode *findGuestById(GuestNode *head, const char *guestId) {
    GuestNode *p;
    // Lặp theo ba phần [p = head; p != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (p = head; p != NULL; p = p->next) {
        // Kiểm tra điều kiện [equalsIgnoreCase(p->data.guestId, guestId)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (equalsIgnoreCase(p->data.guestId, guestId)) return p;
    }
    // Báo rằng không tìm thấy dữ liệu hoặc không thể trả về một đối tượng.
    return NULL;
}

GuestNode *findGuestByIdCard(GuestNode *head, const char *idCard) {
    GuestNode *p;
    // Lặp theo ba phần [p = head; p != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (p = head; p != NULL; p = p->next) {
        // Kiểm tra điều kiện [strcmp(p->data.idCard, idCard) == 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (strcmp(p->data.idCard, idCard) == 0) return p;
    }
    // Báo rằng không tìm thấy dữ liệu hoặc không thể trả về một đối tượng.
    return NULL;
}

/* Them khach moi neu ma khach va CCCD chua bi dung boi nguoi khac. */
// Hàm [addGuest] kiểm tra mã/CCCD trùng rồi thêm khách vào danh sách.
int addGuest(GuestNode **head, Guest g) {
    GuestNode *node, *p;
    // Kiểm tra điều kiện [findGuestById(*head, g.guestId) != NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (findGuestById(*head, g.guestId) != NULL) return LIST_DUPLICATE;
    // Kiểm tra điều kiện [findGuestByIdCard(*head, g.idCard) != NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (findGuestByIdCard(*head, g.idCard) != NULL) return LIST_DUPLICATE_ID_CARD;

    // Lưu [giá trị biểu thức [(GuestNode *)malloc(sizeof(GuestNode))]] vào biến [node] để các lệnh sau sử dụng.
    node = (GuestNode *)malloc(sizeof(GuestNode));
    // Kiểm tra điều kiện [node == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (node == NULL) return LIST_NO_MEMORY;
    // Sao chép thông tin khách vào nút mới vừa cấp phát.
    node->data = g;
    // Đặt nút mới làm nút cuối danh sách.
    node->next = NULL;

    // Kiểm tra điều kiện [*head == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (*head == NULL) {
        *head = node;
    } else {
        // Lặp theo ba phần [p = *head; p->next != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
        for (p = *head; p->next != NULL; p = p->next) { }
        // Nối nút khách mới sau nút cuối hiện tại.
        p->next = node;
    }
    // Trả mã kết quả [LIST_OK] cho hàm gọi để báo trạng thái xử lý.
    return LIST_OK;
}

// Hàm [deleteGuest] gỡ khách khỏi danh sách và giải phóng bộ nhớ.
int deleteGuest(GuestNode **head, const char *guestId) {
    GuestNode **pp = head;
    // Lặp lại khối lệnh khi điều kiện [*pp != NULL] vẫn đúng.
    while (*pp != NULL) {
        // Kiểm tra điều kiện [equalsIgnoreCase((*pp)->data.guestId, guestId)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (equalsIgnoreCase((*pp)->data.guestId, guestId)) {
            GuestNode *victim = *pp;
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

void updateGuest(Guest *g, const char *fullName, const char *idCard, const char *phone) {
    // Chép chuỗi nhưng giới hạn số ký tự để không ghi tràn bộ đệm.
    strncpy(g->fullName, fullName, GUEST_NAME_LEN - 1);
    g->fullName[GUEST_NAME_LEN - 1] = '\0';
    // Chép chuỗi nhưng giới hạn số ký tự để không ghi tràn bộ đệm.
    strncpy(g->idCard, idCard, ID_CARD_LEN - 1);
    g->idCard[ID_CARD_LEN - 1] = '\0';
    // Chép chuỗi nhưng giới hạn số ký tự để không ghi tràn bộ đệm.
    strncpy(g->phone, phone, PHONE_LEN - 1);
    g->phone[PHONE_LEN - 1] = '\0';
}

// Hàm [countGuests] đếm số khách trong danh sách.
int countGuests(const GuestNode *head) {
    // Khai báo biến [n] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int n = 0;
    // Lặp theo ba phần [; head != NULL; head = head->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; head != NULL; head = head->next) n++;
    // Trả giá trị [n] về cho hàm đã gọi dòng này.
    return n;
}

/* Tao ma KH tiep theo dua tren ma so lon nhat dang co trong danh sach. */
void generateGuestId(GuestNode *head, char *out, int size) {
    const GuestNode *p;
    // Khai báo biến [prefixLen] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    size_t prefixLen = strlen(GUEST_ID_PREFIX);
    // Khai báo biến [maxNo] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    long maxNo = 0;

    // Lặp theo ba phần [p = head; p != NULL; p = p->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (p = head; p != NULL; p = p->next) {
        const char *id = p->data.guestId;
        // Xác nhận mã có đúng tiền tố KH và phần số phía sau chỉ gồm chữ số; nhờ vậy không lấy nhầm mã khách sai định dạng.
        if (strlen(id) > prefixLen && strncmp(id, GUEST_ID_PREFIX, prefixLen) == 0 &&
            // Gọi hàm [isDigitsOnly] để kiểm tra chuỗi có chỉ chứa chữ số hay không.
            isDigitsOnly(id + prefixLen)) {
            // Khai báo biến [n] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
            long n = strtol(id + prefixLen, NULL, 10);
            // Kiểm tra điều kiện [n > maxNo]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
            if (n > maxNo) maxNo = n;
        }
    }

    do {
        // Cập nhật biến [maxNo] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
        maxNo++;
        // Gọi hàm [snprintf] để ghép chuỗi theo mẫu vào bộ đệm và giới hạn số ký tự để tránh ghi tràn.
        snprintf(out, (size_t)size, "%s%03ld", GUEST_ID_PREFIX, maxNo);
    } while (findGuestById(head, out) != NULL);
}

/* Kiem tra tu khoa co nam trong ma, ten, CCCD hoac so dien thoai khong. */
// Hàm [guestMatches] kiểm tra từ khóa có khớp dữ liệu khách không.
int guestMatches(const Guest *g, const char *keyword) {
    return containsIgnoreCase(g->guestId, keyword) ||
           // Gọi hàm [containsIgnoreCase] để tìm từ khóa trong chuỗi mà không phân biệt chữ hoa/thường.
           containsIgnoreCase(g->fullName, keyword) ||
           // Gọi hàm [containsIgnoreCase] để tìm từ khóa trong chuỗi mà không phân biệt chữ hoa/thường.
           containsIgnoreCase(g->idCard, keyword) ||
           // Gọi hàm [containsIgnoreCase] để tìm từ khóa trong chuỗi mà không phân biệt chữ hoa/thường.
           containsIgnoreCase(g->phone, keyword);
}

void printGuestHeader(void) {
    // Hiển thị nội dung [+-------------+----------------------------------------------------+--------------+------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-------------+----------------------------------------------------+--------------+------------+\n");
    // Hiển thị nội dung [| %-11s | %-50s | %-12s | %-10s |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("| %-11s | %-50s | %-12s | %-10s |\n", "Ma khach", "Ho ten", "CCCD", "SDT");
    // Hiển thị nội dung [+-------------+----------------------------------------------------+--------------+------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-------------+----------------------------------------------------+--------------+------------+\n");
}

void printGuestRow(const Guest *g) {
    // Hiển thị nội dung [| %-11s | %-50s | %-12s | %-10s |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("| %-11s | %-50s | %-12s | %-10s |\n", g->guestId, g->fullName, g->idCard, g->phone);
}

void printGuestFooter(void) {
    // Hiển thị nội dung [+-------------+----------------------------------------------------+--------------+------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-------------+----------------------------------------------------+--------------+------------+\n");
}

// Hàm [searchGuestsByName] in khách có tên chứa từ khóa.
int searchGuestsByName(const GuestNode *head, const char *keyword) {
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count = 0;
    // Gọi hàm [printGuestHeader] để in tiêu đề các cột của bảng khách.
    printGuestHeader();
    // Lặp theo ba phần [; head != NULL; head = head->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; head != NULL; head = head->next) {
        // Kiểm tra điều kiện [containsIgnoreCase(head->data.fullName, keyword)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (containsIgnoreCase(head->data.fullName, keyword)) {
            // Gọi hàm [printGuestRow] để in thông tin của một khách thành một dòng.
            printGuestRow(&head->data);
            // Cập nhật biến [count] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
            count++;
        }
    }
    // Gọi hàm [printGuestFooter] để in đường kẻ kết thúc bảng khách.
    printGuestFooter();
    // Hiển thị nội dung [Tim thay: %d khach.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Tim thay: %d khach.\n", count);
    // Trả giá trị [count] về cho hàm đã gọi dòng này.
    return count;
}

void printGuests(const GuestNode *head) {
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count = 0;
    // Gọi hàm [printGuestHeader] để in tiêu đề các cột của bảng khách.
    printGuestHeader();
    // Lặp theo ba phần [; head != NULL; head = head->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; head != NULL; head = head->next) {
        // Gọi hàm [printGuestRow] để in thông tin của một khách thành một dòng.
        printGuestRow(&head->data);
        // Cập nhật biến [count] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
        count++;
    }
    // Gọi hàm [printGuestFooter] để in đường kẻ kết thúc bảng khách.
    printGuestFooter();
    // Hiển thị nội dung [Tong: %d khach.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Tong: %d khach.\n", count);
}

/* Giai phong toan bo danh sach khach khi chuong trinh ket thuc. */
void freeGuestList(GuestNode **head) {
    GuestNode *p = *head, *next;
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
