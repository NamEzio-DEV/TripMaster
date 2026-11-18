#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include "utils.h"

/* Xac dinh nam nhuan theo lich Gregory: chia het cho 4, tru nam tron the ky
   neu nam do khong chia het cho 400. */
// Hàm [isLeapYear] kiểm tra năm nhuận.
int isLeapYear(int year) {
    // Trả giá trị [(year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)] về cho hàm đã gọi dòng này.
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/* Tra ve so ngay cua thang; thang 2 duoc tinh rieng neu la nam nhuan. */
// Hàm [daysInMonth] trả số ngày của tháng, có xét tháng 2 năm nhuận.
int daysInMonth(int month, int year) {
    // Số ngày của từng tháng theo thứ tự từ tháng 1 đến tháng 12.
    static const int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    // Kiểm tra điều kiện [month < 1 || month > 12]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (month < 1 || month > 12) return 0;
    // Kiểm tra điều kiện [month == 2 && isLeapYear(year)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (month == 2 && isLeapYear(year)) return 29;
    // Trả giá trị [days[month - 1]] về cho hàm đã gọi dòng này.
    return days[month - 1];
}

/* Kiem tra nam, thang va ngay co ton tai trong lich hay khong. */
// Hàm [isValidDate] kiểm tra ngày có tồn tại trong lịch.
int isValidDate(Date d) {
    // Kiểm tra điều kiện [d.year < MIN_YEAR || d.year > MAX_YEAR]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (d.year < MIN_YEAR || d.year > MAX_YEAR) return 0;
    // Kiểm tra điều kiện [d.month < 1 || d.month > 12]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (d.month < 1 || d.month > 12) return 0;
    // Kiểm tra điều kiện [d.day < 1 || d.day > daysInMonth(d.month, d.year)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (d.day < 1 || d.day > daysInMonth(d.month, d.year)) return 0;
    // Trả mã kết quả [1] cho hàm gọi để báo trạng thái xử lý.
    return 1;
}

/* Chuyen ngay thanh moc lien tuc de tinh chenh lech qua thang, nam. */
// Hàm [dateToDays] đổi ngày thành số ngày liên tục để tính khoảng cách.
long dateToDays(Date d) {
    // Khai báo biến [y] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    long y = d.year;
    // Khai báo biến [m] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    long m = d.month;
    // Khai báo biến [era] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    long era, yoe, doy, doe;
    // Kiểm tra điều kiện [m <= 2]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (m <= 2) y -= 1;
    // Chia năm thành các chu kỳ 400 năm để tính ngày mà vẫn tính đúng năm nhuận.
    era = y / 400;
    // Lấy vị trí năm hiện tại bên trong chu kỳ 400 năm.
    yoe = y - era * 400;
    // Đổi tháng và ngày thành số thứ tự ngày tính từ đầu năm.
    doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d.day - 1;
    // Tính số ngày từ đầu chu kỳ 400 năm đến ngày đang xét.
    doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    // Trả giá trị [era * 146097 + doe] về cho hàm đã gọi dòng này.
    return era * 146097 + doe;
}

/* So sanh hai ngay: -1 neu a truoc b, 0 neu bang nhau, 1 neu a sau b. */
// Hàm [compareDate] so sánh hai ngày và trả về -1, 0 hoặc 1.
int compareDate(Date a, Date b) {
    // Khai báo biến [da] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    long da = dateToDays(a), db = dateToDays(b);
    // Kiểm tra điều kiện [da < db]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (da < db) return -1;
    // Kiểm tra điều kiện [da > db]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (da > db) return 1;
    // Trả mã kết quả [0] cho hàm gọi để báo trạng thái xử lý.
    return 0;
}

/* Cong/tru tung ngay de tu dong xu ly viec chuyen thang va chuyen nam. */
// Hàm [addDays] cộng hoặc trừ ngày, tự xử lý ranh giới tháng/năm.
Date addDays(Date d, int n) {
    // Lặp lại khối lệnh khi điều kiện [n > 0] vẫn đúng.
    while (n > 0) {
        // Tăng ngày lên một ngày lịch.
        d.day++;
        // Kiểm tra điều kiện [d.day > daysInMonth(d.month, d.year)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (d.day > daysInMonth(d.month, d.year)) {
            // Khi vượt số ngày của tháng, chuyển về ngày đầu tháng kế tiếp.
            d.day = 1;
            // Chuyển sang tháng kế tiếp.
            d.month++;
            // Tháng 13 không tồn tại nên chuyển sang tháng 1 của năm kế tiếp.
            if (d.month > 12) { d.month = 1; d.year++; }
        }
        // Cập nhật biến [n] bằng phép [--], thường để tăng bộ đếm hoặc cộng dồn kết quả.
        n--;
    }
    // Lặp lại khối lệnh khi điều kiện [n < 0] vẫn đúng.
    while (n < 0) {
        // Giảm ngày đi một ngày lịch.
        d.day--;
        // Kiểm tra điều kiện [d.day < 1]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (d.day < 1) {
            d.month--;
            // Tháng 0 không tồn tại nên chuyển về tháng 12 của năm trước.
            if (d.month < 1) { d.month = 12; d.year--; }
            // Đặt ngày bằng ngày cuối cùng của tháng mới sau khi lùi tháng.
            d.day = daysInMonth(d.month, d.year);
        }
        // Cập nhật biến [n] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
        n++;
    }
    // Trả giá trị [d] về cho hàm đã gọi dòng này.
    return d;
}

/* Lay ngay hien tai theo dong ho va mui gio cua may dang chay chuong trinh. */
// Hàm [getToday] lấy ngày hiện tại từ đồng hồ máy.
Date getToday(void) {
    // Khai báo biến [d] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Date d;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    // Gán ngày hiện tại lấy từ đồng hồ máy vào trường ngày.
    d.day = t->tm_mday;
    // Đổi tháng của tm sang cách đếm từ 1 đến 12 (tm_mon bắt đầu từ 0).
    d.month = t->tm_mon + 1;
    // Đổi năm tm từ số năm tính từ 1900 sang năm đầy đủ.
    d.year = t->tm_year + 1900;
    // Trả giá trị [d] về cho hàm đã gọi dòng này.
    return d;
}

void formatDate(Date d, char *buf, size_t size) {
    // Gọi hàm [snprintf] để ghép chuỗi theo mẫu vào bộ đệm và giới hạn số ký tự để tránh ghi tràn.
    snprintf(buf, size, "%02d/%02d/%04d", d.day, d.month, d.year);
}

/* Xoa khoang trang o dau va cuoi chuoi nhap, giu nguyen khoang trang ben trong. */
void trim(char *s) {
    // Khai báo biến [len] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    size_t len, start = 0;
    // Kiểm tra điều kiện [s == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (s == NULL) return;
    // Lưu [giá trị biểu thức [strlen(s)]] vào biến [len] để các lệnh sau sử dụng.
    len = strlen(s);
    // Lặp lại khối lệnh khi điều kiện [len > 0 && isspace((unsigned char)s[len - 1])] vẫn đúng.
    while (len > 0 && isspace((unsigned char)s[len - 1])) s[--len] = '\0';
    // Lặp lại khối lệnh khi điều kiện [s[start] != '\0' && isspace((unsigned char)s[start])] vẫn đúng.
    while (s[start] != '\0' && isspace((unsigned char)s[start])) start++;
    // Kiểm tra điều kiện [start > 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (start > 0) memmove(s, s + start, len - start + 1);
}

void toUpperStr(char *s) {
    // Lặp theo ba phần [; *s; s++]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; *s; s++) *s = (char)toupper((unsigned char)*s);
}

// Hàm [equalsIgnoreCase] so sánh hai chuỗi không phân biệt hoa/thường.
int equalsIgnoreCase(const char *a, const char *b) {
    // Lặp lại khối lệnh khi điều kiện [*a && *b] vẫn đúng.
    while (*a && *b) {
        // Kiểm tra điều kiện [tolower((unsigned char)*a) != tolower((unsigned char)*b)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        // Cập nhật biến [a] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
        a++;
        // Cập nhật biến [b] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
        b++;
    }
    // Trả giá trị [*a == '\0' && *b == '\0'] về cho hàm đã gọi dòng này.
    return *a == '\0' && *b == '\0';
}

/* Tim chuoi con khong phan biet chu hoa/chu thuong; tu khoa rong khop tat ca. */
// Hàm [containsIgnoreCase] tìm chuỗi con không phân biệt hoa/thường.
int containsIgnoreCase(const char *text, const char *key) {
    // Khai báo biến [n] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    size_t n = strlen(key), i, j;
    // Khai báo biến [len] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    size_t len = strlen(text);
    // Kiểm tra điều kiện [n == 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (n == 0) return 1;
    // Lặp theo ba phần [i = 0; i + n <= len; i++]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (i = 0; i + n <= len; i++) {
        // Lặp theo ba phần [j = 0; j < n; j++]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
        for (j = 0; j < n; j++) {
            // Kiểm tra điều kiện [tolower((unsigned char)text[i + j]) != tolower((unsigned char)key[j])]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
            if (tolower((unsigned char)text[i + j]) != tolower((unsigned char)key[j])) break;
        }
        // Kiểm tra điều kiện [j == n]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (j == n) return 1;
    }
    // Trả mã kết quả [0] cho hàm gọi để báo trạng thái xử lý.
    return 0;
}

// Hàm [isDigitsOnly] kiểm tra chuỗi có chỉ gồm chữ số.
int isDigitsOnly(const char *s) {
    // Kiểm tra điều kiện [*s == '\0']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (*s == '\0') return 0;
    // Lặp theo ba phần [; *s; s++]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (; *s; s++) {
        // Kiểm tra điều kiện [!isdigit((unsigned char)*s)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!isdigit((unsigned char)*s)) return 0;
    }
    // Trả mã kết quả [1] cho hàm gọi để báo trạng thái xử lý.
    return 1;
}

/* Doc mot dong an toan; neu dong qua dai thi bo phan con thua va bao loi. */
// Hàm [readLine] đọc một dòng nhập và xử lý trường hợp nhập quá dài.
int readLine(char *buf, int size) {
    // Khai báo biến [len] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    size_t len;
    // Kiểm tra điều kiện [fgets(buf, size, stdin) == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (fgets(buf, size, stdin) == NULL) {
        // Hiển thị nội dung [Ket thuc du lieu vao. Thoat chuong trinh.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("\nKet thuc du lieu vao. Thoat chuong trinh.\n");
        // Gọi hàm [exit] để kết thúc chương trình ngay khi không còn dữ liệu đầu vào.
        exit(0);
    }
    // Lưu [giá trị biểu thức [strlen(buf)]] vào biến [len] để các lệnh sau sử dụng.
    len = strlen(buf);
    // Kiểm tra điều kiện [len > 0 && buf[len - 1] == '\n']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
        // Gọi hàm [trim] để xóa khoảng trắng thừa ở đầu và cuối chuỗi.
        trim(buf);
        // Trả mã kết quả [1] cho hàm gọi để báo trạng thái xử lý.
        return 1;
    }
    // Kiểm tra điều kiện [!feof(stdin)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (!feof(stdin)) {
        // Khai báo biến [c] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
        int c;
        // Lặp lại khối lệnh khi điều kiện [(c = getchar()) != '\n' && c != EOF] vẫn đúng.
        while ((c = getchar()) != '\n' && c != EOF) { }
        buf[0] = '\0';
        // Trả mã kết quả [0] cho hàm gọi để báo trạng thái xử lý.
        return 0;
    }
    // Gọi hàm [trim] để xóa khoảng trắng thừa ở đầu và cuối chuỗi.
    trim(buf);
    // Trả mã kết quả [1] cho hàm gọi để báo trạng thái xử lý.
    return 1;
}

static int parseInt(const char *s, int *out) {
    char *end;
    // Khai báo biến [v] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    long v;
    // Kiểm tra điều kiện [*s == '\0']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (*s == '\0') return 0;
    // Lưu [giá trị biểu thức [0]] vào biến [errno] để các lệnh sau sử dụng.
    errno = 0;
    // Lưu [giá trị biểu thức [strtol(s, &end, 10)]] vào biến [v] để các lệnh sau sử dụng.
    v = strtol(s, &end, 10);
    // Kiểm tra điều kiện [*end != '\0' || errno == ERANGE || v < -2147483647L || v > 2147483647L]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (*end != '\0' || errno == ERANGE || v < -2147483647L || v > 2147483647L) return 0;
    *out = (int)v;
    // Trả mã kết quả [1] cho hàm gọi để báo trạng thái xử lý.
    return 1;
}

static int parseDouble(const char *s, double *out) {
    char *end;
    // Khai báo biến [v] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    double v;
    // Kiểm tra điều kiện [*s == '\0']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (*s == '\0') return 0;
    // Lưu [giá trị biểu thức [0]] vào biến [errno] để các lệnh sau sử dụng.
    errno = 0;
    // Lưu [giá trị biểu thức [strtod(s, &end)]] vào biến [v] để các lệnh sau sử dụng.
    v = strtod(s, &end);
    // Kiểm tra điều kiện [*end != '\0' || errno == ERANGE || v != v]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (*end != '\0' || errno == ERANGE || v != v) return 0;
    *out = v;
    // Trả mã kết quả [1] cho hàm gọi để báo trạng thái xử lý.
    return 1;
}

/* Phan tich ngay theo dd/mm/yyyy, dd-mm-yyyy hoac dd.mm.yyyy. */
static int parseDate(const char *s, Date *out) {
    // Khai báo biến [d] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Date d;
    // Khai báo biến [extra] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char extra;
    // Thử lần lượt ba cách viết ngày; nếu cả ba lần phân tích đều không đọc đúng ba số ngày/tháng/năm thì báo sai định dạng.
    if (sscanf(s, "%d/%d/%d%c", &d.day, &d.month, &d.year, &extra) != 3 &&
        // Gọi hàm [sscanf] để thử phân tích chuỗi ngày theo định dạng đang xét.
        sscanf(s, "%d-%d-%d%c", &d.day, &d.month, &d.year, &extra) != 3 &&
        // Gọi hàm [sscanf] để thử phân tích chuỗi ngày theo định dạng đang xét.
        sscanf(s, "%d.%d.%d%c", &d.day, &d.month, &d.year, &extra) != 3) {
        // Trả mã kết quả [0] cho hàm gọi để báo trạng thái xử lý.
        return 0;
    }
    *out = d;
    // Trả mã kết quả [1] cho hàm gọi để báo trạng thái xử lý.
    return 1;
}

// Hàm [inputInt] nhập số nguyên đến khi nằm trong giới hạn.
int inputInt(const char *prompt, int min, int max) {
    // Khai báo biến [buf] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char buf[LINE_BUF];
    // Khai báo biến [v] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int v;
    // Lặp nhập lại cho đến khi dữ liệu hợp lệ; return bên dưới sẽ kết thúc vòng lặp.
    for (;;) {
        // Hiển thị nội dung [%s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("%s", prompt);
        // Kiểm tra điều kiện [readLine(buf, sizeof(buf)) && parseInt(buf, &v)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (readLine(buf, sizeof(buf)) && parseInt(buf, &v)) {
            // Kiểm tra điều kiện [v >= min && v <= max]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
            if (v >= min && v <= max) return v;
            // Hiển thị nội dung [  [!] Gia tri phai trong khoang %d..%d.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Gia tri phai trong khoang %d..%d.\n", min, max);
        } else {
            // Hiển thị nội dung [  [!] Vui long nhap mot so nguyen.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Vui long nhap mot so nguyen.\n");
        }
    }
}

// Hàm [inputDouble] nhập số thực đến khi nằm trong giới hạn.
double inputDouble(const char *prompt, double min, double max) {
    // Khai báo biến [buf] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char buf[LINE_BUF];
    // Khai báo biến [v] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    double v;
    // Lặp để hỏi lại khi dữ liệu sai; return bên dưới chỉ chạy sau khi đã nhận được giá trị hợp lệ.
    for (;;) {
        // Hiển thị nội dung [%s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("%s", prompt);
        // Kiểm tra điều kiện [readLine(buf, sizeof(buf)) && parseDouble(buf, &v)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (readLine(buf, sizeof(buf)) && parseDouble(buf, &v)) {
            // Kiểm tra điều kiện [v >= min && v <= max]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
            if (v >= min && v <= max) return v;
            // Hiển thị nội dung [  [!] Gia tri phai trong khoang %.0f..%.0f.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Gia tri phai trong khoang %.0f..%.0f.\n", min, max);
        } else {
            // Hiển thị nội dung [  [!] Vui long nhap mot so.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Vui long nhap mot so.\n");
        }
    }
}

void inputString(const char *prompt, char *buf, int size, int allowEmpty) {
    // Khai báo biến [line] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char line[LINE_BUF];
    // Lặp để hỏi lại khi dữ liệu sai; return bên dưới chỉ chạy sau khi đã nhận được giá trị hợp lệ.
    for (;;) {
        // Hiển thị nội dung [%s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("%s", prompt);
        // Kiểm tra điều kiện [!readLine(line, sizeof(line))]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!readLine(line, sizeof(line))) {
            // Hiển thị nội dung [  [!] Dong nhap qua dai.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Dong nhap qua dai.\n");
            continue;
        }
        // Kiểm tra điều kiện [!allowEmpty && line[0] == '\0']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!allowEmpty && line[0] == '\0') {
            // Hiển thị nội dung [  [!] Khong duoc de trong.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Khong duoc de trong.\n");
            continue;
        }
        // Kiểm tra điều kiện [(int)strlen(line) >= size]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if ((int)strlen(line) >= size) {
            // Hiển thị nội dung [  [!] Toi da %d ky tu.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Toi da %d ky tu.\n", size - 1);
            continue;
        }
        // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
        strcpy(buf, line);
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
}

void inputCode(const char *prompt, char *buf, int size) {
    // Lặp để hỏi lại khi dữ liệu sai; return bên dưới chỉ chạy sau khi đã nhận được giá trị hợp lệ.
    for (;;) {
        // Gọi hàm [inputString] để đọc chuỗi, kiểm tra độ dài và cho phép/ràng buộc chuỗi rỗng theo tham số cuối.
        inputString(prompt, buf, size, 0);
        // Kiểm tra điều kiện [strchr(buf, ' ') != NULL || strchr(buf, '\t') != NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (strchr(buf, ' ') != NULL || strchr(buf, '\t') != NULL) {
            // Hiển thị nội dung [  [!] Ma khong duoc chua dau cach.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Ma khong duoc chua dau cach.\n");
            continue;
        }
        // Gọi hàm [toUpperStr] để chuyển các chữ cái trong mã thành chữ hoa.
        toUpperStr(buf);
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
}

static int isDigitsInRange(const char *s, int minLen, int maxLen) {
    // Khai báo biến [len] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int len = (int)strlen(s);
    // Trả giá trị [isDigitsOnly(s) && len >= minLen && len <= maxLen] về cho hàm đã gọi dòng này.
    return isDigitsOnly(s) && len >= minLen && len <= maxLen;
}

static void printDigitsError(int minLen, int maxLen) {
    // Kiểm tra điều kiện [minLen == maxLen]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (minLen == maxLen)
        // Hiển thị nội dung [  [!] Phai gom dung %d chu so.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("  [!] Phai gom dung %d chu so.\n", minLen);
    // Chạy nhánh dự phòng khi các điều kiện phía trên đều sai.
    else
        // Hiển thị nội dung [  [!] Chi duoc nhap chu so, tu %d den %d ky tu.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("  [!] Chi duoc nhap chu so, tu %d den %d ky tu.\n", minLen, maxLen);
}

void inputDigits(const char *prompt, char *buf, int minLen, int maxLen) {
    // Khai báo biến [line] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char line[LINE_BUF];
    // Lặp để hỏi lại khi dữ liệu sai; return bên dưới chỉ chạy sau khi đã nhận được giá trị hợp lệ.
    for (;;) {
        // Hiển thị nội dung [%s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("%s", prompt);
        // Kiểm tra điều kiện [readLine(line, sizeof(line)) && isDigitsInRange(line, minLen, maxLen)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (readLine(line, sizeof(line)) && isDigitsInRange(line, minLen, maxLen)) {
            // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
            strcpy(buf, line);
            // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
            return;
        }
        // Gọi hàm [printDigitsError] để in lý do chuỗi không đạt yêu cầu về chữ số hoặc độ dài.
        printDigitsError(minLen, maxLen);
    }
}

// Hàm [inputDate] nhập ngày đến khi ngày hợp lệ.
Date inputDate(const char *prompt) {
    // Khai báo biến [buf] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char buf[LINE_BUF];
    // Khai báo biến [d] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Date d;
    // Lặp để hỏi lại khi dữ liệu sai; return bên dưới chỉ chạy sau khi đã nhận được giá trị hợp lệ.
    for (;;) {
        // Hiển thị nội dung [%s] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("%s", prompt);
        // Kiểm tra điều kiện [readLine(buf, sizeof(buf)) && parseDate(buf, &d)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (readLine(buf, sizeof(buf)) && parseDate(buf, &d)) {
            // Kiểm tra điều kiện [isValidDate(d)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
            if (isValidDate(d)) return d;
            // Hiển thị nội dung [  [!] Ngay khong ton tai (kiem tra so ngay trong thang / nam nhuan).] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Ngay khong ton tai (kiem tra so ngay trong thang / nam nhuan).\n");
        } else {
            // Hiển thị nội dung [  [!] Sai dinh dang. Nhap theo dang dd/mm/yyyy.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Sai dinh dang. Nhap theo dang dd/mm/yyyy.\n");
        }
    }
}

// Hàm [inputYesNo] hỏi người dùng câu trả lời có hoặc không.
int inputYesNo(const char *prompt) {
    // Khai báo biến [buf] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char buf[LINE_BUF];
    // Lặp để hỏi lại khi dữ liệu sai; return bên dưới chỉ chạy sau khi đã nhận được giá trị hợp lệ.
    for (;;) {
        // Hiển thị nội dung [%s (y/n): ] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("%s (y/n): ", prompt);
        // Gọi hàm [readLine] để đọc một dòng bàn phím vào bộ đệm.
        readLine(buf, sizeof(buf));
        // Kiểm tra điều kiện [equalsIgnoreCase(buf, "y") || equalsIgnoreCase(buf, "c")]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (equalsIgnoreCase(buf, "y") || equalsIgnoreCase(buf, "c")) return 1;
        // Kiểm tra điều kiện [equalsIgnoreCase(buf, "n") || equalsIgnoreCase(buf, "k")]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (equalsIgnoreCase(buf, "n") || equalsIgnoreCase(buf, "k")) return 0;
        // Hiển thị nội dung [  [!] Nhap y hoac n.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("  [!] Nhap y hoac n.\n");
    }
}

// Hàm [inputIntOrKeep] nhập số mới hoặc giữ số cũ khi nhấn Enter.
int inputIntOrKeep(const char *prompt, int min, int max, int current) {
    // Khai báo biến [buf] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char buf[LINE_BUF];
    // Khai báo biến [v] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int v;
    // Lặp để hỏi lại khi dữ liệu sai; return bên dưới chỉ chạy sau khi đã nhận được giá trị hợp lệ.
    for (;;) {
        // Hiển thị nội dung [%s [%d]: ] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("%s [%d]: ", prompt, current);
        // Kiểm tra điều kiện [!readLine(buf, sizeof(buf))]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!readLine(buf, sizeof(buf))) { printf("  [!] Dong nhap qua dai.\n"); continue; }
        // Kiểm tra điều kiện [buf[0] == '\0']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (buf[0] == '\0') return current;
        // Kiểm tra điều kiện [parseInt(buf, &v) && v >= min && v <= max]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (parseInt(buf, &v) && v >= min && v <= max) return v;
        // Hiển thị nội dung [  [!] Nhap so nguyen trong khoang %d..%d (Enter de giu nguyen).] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("  [!] Nhap so nguyen trong khoang %d..%d (Enter de giu nguyen).\n", min, max);
    }
}

// Hàm [inputDoubleOrKeep] nhập giá mới hoặc giữ giá cũ khi nhấn Enter.
double inputDoubleOrKeep(const char *prompt, double min, double max, double current) {
    // Khai báo biến [buf] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char buf[LINE_BUF];
    // Khai báo biến [v] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    double v;
    // Lặp để hỏi lại khi dữ liệu sai; return bên dưới chỉ chạy sau khi đã nhận được giá trị hợp lệ.
    for (;;) {
        // Hiển thị nội dung [%s [%.0f]: ] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("%s [%.0f]: ", prompt, current);
        // Kiểm tra điều kiện [!readLine(buf, sizeof(buf))]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!readLine(buf, sizeof(buf))) { printf("  [!] Dong nhap qua dai.\n"); continue; }
        // Kiểm tra điều kiện [buf[0] == '\0']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (buf[0] == '\0') return current;
        // Kiểm tra điều kiện [parseDouble(buf, &v) && v >= min && v <= max]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (parseDouble(buf, &v) && v >= min && v <= max) return v;
        // Hiển thị nội dung [  [!] Nhap so trong khoang %.0f..%.0f (Enter de giu nguyen).] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("  [!] Nhap so trong khoang %.0f..%.0f (Enter de giu nguyen).\n", min, max);
    }
}

void inputStringOrKeep(const char *prompt, char *buf, int size) {
    // Khai báo biến [line] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char line[LINE_BUF];
    // Lặp để hỏi lại khi dữ liệu sai; return bên dưới chỉ chạy sau khi đã nhận được giá trị hợp lệ.
    for (;;) {
        // Hiển thị nội dung [%s [%s]: ] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("%s [%s]: ", prompt, buf);
        // Kiểm tra điều kiện [!readLine(line, sizeof(line))]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!readLine(line, sizeof(line))) { printf("  [!] Dong nhap qua dai.\n"); continue; }
        // Kiểm tra điều kiện [line[0] == '\0']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (line[0] == '\0') return;
        // Kiểm tra điều kiện [(int)strlen(line) >= size]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if ((int)strlen(line) >= size) {
            // Hiển thị nội dung [  [!] Toi da %d ky tu.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
            printf("  [!] Toi da %d ky tu.\n", size - 1);
            continue;
        }
        // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
        strcpy(buf, line);
        // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
        return;
    }
}

void inputDigitsOrKeep(const char *prompt, char *buf, int minLen, int maxLen) {
    // Khai báo biến [line] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char line[LINE_BUF];
    // Lặp để hỏi lại khi dữ liệu sai; return bên dưới chỉ chạy sau khi đã nhận được giá trị hợp lệ.
    for (;;) {
        // Hiển thị nội dung [%s [%s]: ] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("%s [%s]: ", prompt, buf);
        // Kiểm tra điều kiện [!readLine(line, sizeof(line))]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!readLine(line, sizeof(line))) { printf("  [!] Dong nhap qua dai.\n"); continue; }
        // Kiểm tra điều kiện [line[0] == '\0']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (line[0] == '\0') return;
        // Kiểm tra điều kiện [isDigitsInRange(line, minLen, maxLen)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (isDigitsInRange(line, minLen, maxLen)) {
            // Chép chuỗi nguồn vào bộ đệm đích; độ dài đã được kiểm tra ở bước nhập trước đó.
            strcpy(buf, line);
            // Dừng hàm hiện tại và quay về nơi đã gọi hàm.
            return;
        }
        // Gọi hàm [printDigitsError] để in lý do chuỗi không đạt yêu cầu về chữ số hoặc độ dài.
        printDigitsError(minLen, maxLen);
        // Hiển thị nội dung [      (Enter de giu nguyen)] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("      (Enter de giu nguyen)\n");
    }
}

// Hàm [inputDateOrKeep] nhập ngày mới hoặc giữ ngày hiện tại khi nhấn Enter.
Date inputDateOrKeep(const char *prompt, Date current) {
    // Khai báo biến [buf] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char buf[LINE_BUF], cur[DATE_STR_LEN];
    // Khai báo biến [d] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Date d;
    // Gọi hàm [formatDate] để định dạng ngày thành chuỗi dd/mm/yyyy để in hoặc ghép vào thông báo.
    formatDate(current, cur, sizeof(cur));
    // Lặp để hỏi lại khi dữ liệu sai; return bên dưới chỉ chạy sau khi đã nhận được giá trị hợp lệ.
    for (;;) {
        // Hiển thị nội dung [%s [%s]: ] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("%s [%s]: ", prompt, cur);
        // Kiểm tra điều kiện [!readLine(buf, sizeof(buf))]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (!readLine(buf, sizeof(buf))) { printf("  [!] Dong nhap qua dai.\n"); continue; }
        // Kiểm tra điều kiện [buf[0] == '\0']; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (buf[0] == '\0') return current;
        // Kiểm tra điều kiện [parseDate(buf, &d) && isValidDate(d)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (parseDate(buf, &d) && isValidDate(d)) return d;
        // Hiển thị nội dung [  [!] Ngay khong hop le. Nhap dd/mm/yyyy (Enter de giu nguyen).] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("  [!] Ngay khong hop le. Nhap dd/mm/yyyy (Enter de giu nguyen).\n");
    }
}

void pauseScreen(void) {
    // Khai báo biến [buf] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char buf[LINE_BUF];
    // Hiển thị nội dung [Nhan Enter de tiep tuc...] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("\nNhan Enter de tiep tuc...");
    // Gọi hàm [readLine] để đọc một dòng bàn phím vào bộ đệm.
    readLine(buf, sizeof(buf));
}
