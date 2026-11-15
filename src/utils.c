/*
 * utils.c - Cài đặt các hàm tiện ích: ngày tháng, chuỗi, nhập dữ liệu.
 *
 * Mọi thao tác nhập đều đọc nguyên một dòng bằng fgets rồi mới phân tích,
 * nhờ vậy nhập chữ vào ô số không làm kẹt bộ đệm như khi dùng scanf.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include "utils.h"

/* ====================== NGÀY THÁNG ====================== */

int isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int month, int year) {
    static const int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12) return 0;
    if (month == 2 && isLeapYear(year)) return 29;
    return days[month - 1];
}

int isValidDate(Date d) {
    if (d.year < MIN_YEAR || d.year > MAX_YEAR) return 0;
    if (d.month < 1 || d.month > 12) return 0;
    if (d.day < 1 || d.day > daysInMonth(d.month, d.year)) return 0;
    return 1;
}

/*
 * Đổi ngày sang số ngày liên tục (thuật toán "days from civil").
 * Hiệu của hai giá trị chính là số ngày giữa hai mốc, kể cả khi qua
 * cuối tháng, cuối năm hay năm nhuận. Ví dụ 30/12 -> 02/01 = 3.
 */
long dateToDays(Date d) {
    long y = d.year;
    long m = d.month;
    long era, yoe, doy, doe;
    if (m <= 2) y -= 1;
    era = y / 400;
    yoe = y - era * 400;
    doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d.day - 1;
    doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe;
}

int compareDate(Date a, Date b) {
    long da = dateToDays(a), db = dateToDays(b);
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

Date addDays(Date d, int n) {
    while (n > 0) {
        d.day++;
        if (d.day > daysInMonth(d.month, d.year)) {
            d.day = 1;
            d.month++;
            if (d.month > 12) { d.month = 1; d.year++; }
        }
        n--;
    }
    while (n < 0) {
        d.day--;
        if (d.day < 1) {
            d.month--;
            if (d.month < 1) { d.month = 12; d.year--; }
            d.day = daysInMonth(d.month, d.year);
        }
        n++;
    }
    return d;
}

Date getToday(void) {
    Date d;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    d.day = t->tm_mday;
    d.month = t->tm_mon + 1;
    d.year = t->tm_year + 1900;
    return d;
}

void formatDate(Date d, char *buf, size_t size) {
    snprintf(buf, size, "%02d/%02d/%04d", d.day, d.month, d.year);
}

/* ====================== CHUỖI ====================== */

void trim(char *s) {
    size_t len, start = 0;
    if (s == NULL) return;
    len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) s[--len] = '\0';
    while (s[start] != '\0' && isspace((unsigned char)s[start])) start++;
    if (start > 0) memmove(s, s + start, len - start + 1);
}

void toUpperStr(char *s) {
    for (; *s; s++) *s = (char)toupper((unsigned char)*s);
}

int equalsIgnoreCase(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0;
        a++;
        b++;
    }
    return *a == '\0' && *b == '\0';
}

int containsIgnoreCase(const char *text, const char *key) {
    size_t n = strlen(key), i, j;
    size_t len = strlen(text);
    if (n == 0) return 1;
    for (i = 0; i + n <= len; i++) {
        for (j = 0; j < n; j++) {
            if (tolower((unsigned char)text[i + j]) != tolower((unsigned char)key[j])) break;
        }
        if (j == n) return 1;
    }
    return 0;
}

int isDigitsOnly(const char *s) {
    if (*s == '\0') return 0;
    for (; *s; s++) {
        if (!isdigit((unsigned char)*s)) return 0;
    }
    return 1;
}

/* ====================== NHẬP DỮ LIỆU ====================== */

/*
 * Đọc một dòng từ bàn phím, bỏ ký tự xuống dòng và khoảng trắng hai đầu.
 * Trả về 1 nếu đọc được, 0 nếu dòng dài quá bộ đệm (phần thừa bị bỏ).
 * Gặp EOF (Ctrl+Z / hết dữ liệu) thì thoát chương trình; dữ liệu đã được
 * lưu sau mỗi thao tác nên không bị mất.
 */
int readLine(char *buf, int size) {
    size_t len;
    if (fgets(buf, size, stdin) == NULL) {
        printf("\nKet thuc du lieu vao. Thoat chuong trinh.\n");
        exit(0);
    }
    len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
        trim(buf);
        return 1;
    }
    if (!feof(stdin)) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
        buf[0] = '\0';
        return 0;
    }
    trim(buf);
    return 1;
}

/* Phân tích số nguyên; trả về 1 nếu cả chuỗi là một số hợp lệ */
static int parseInt(const char *s, int *out) {
    char *end;
    long v;
    if (*s == '\0') return 0;
    errno = 0;
    v = strtol(s, &end, 10);
    if (*end != '\0' || errno == ERANGE || v < -2147483647L || v > 2147483647L) return 0;
    *out = (int)v;
    return 1;
}

static int parseDouble(const char *s, double *out) {
    char *end;
    double v;
    if (*s == '\0') return 0;
    errno = 0;
    v = strtod(s, &end);
    if (*end != '\0' || errno == ERANGE || v != v) return 0; /* v != v: NaN */
    *out = v;
    return 1;
}

/* Phân tích "dd/mm/yyyy" (chấp nhận cả dấu '-' hoặc '.') */
static int parseDate(const char *s, Date *out) {
    Date d;
    char extra;
    if (sscanf(s, "%d/%d/%d%c", &d.day, &d.month, &d.year, &extra) != 3 &&
        sscanf(s, "%d-%d-%d%c", &d.day, &d.month, &d.year, &extra) != 3 &&
        sscanf(s, "%d.%d.%d%c", &d.day, &d.month, &d.year, &extra) != 3) {
        return 0;
    }
    *out = d;
    return 1;
}

int inputInt(const char *prompt, int min, int max) {
    char buf[LINE_BUF];
    int v;
    for (;;) {
        printf("%s", prompt);
        if (readLine(buf, sizeof(buf)) && parseInt(buf, &v)) {
            if (v >= min && v <= max) return v;
            printf("  [!] Gia tri phai trong khoang %d..%d.\n", min, max);
        } else {
            printf("  [!] Vui long nhap mot so nguyen.\n");
        }
    }
}

double inputDouble(const char *prompt, double min, double max) {
    char buf[LINE_BUF];
    double v;
    for (;;) {
        printf("%s", prompt);
        if (readLine(buf, sizeof(buf)) && parseDouble(buf, &v)) {
            if (v >= min && v <= max) return v;
            printf("  [!] Gia tri phai trong khoang %.0f..%.0f.\n", min, max);
        } else {
            printf("  [!] Vui long nhap mot so.\n");
        }
    }
}

void inputString(const char *prompt, char *buf, int size, int allowEmpty) {
    char line[LINE_BUF];
    for (;;) {
        printf("%s", prompt);
        if (!readLine(line, sizeof(line))) {
            printf("  [!] Dong nhap qua dai.\n");
            continue;
        }
        if (!allowEmpty && line[0] == '\0') {
            printf("  [!] Khong duoc de trong.\n");
            continue;
        }
        if ((int)strlen(line) >= size) {
            printf("  [!] Toi da %d ky tu.\n", size - 1);
            continue;
        }
        strcpy(buf, line);
        return;
    }
}

void inputCode(const char *prompt, char *buf, int size) {
    for (;;) {
        inputString(prompt, buf, size, 0);
        if (strchr(buf, ' ') != NULL || strchr(buf, '\t') != NULL) {
            printf("  [!] Ma khong duoc chua dau cach.\n");
            continue;
        }
        toUpperStr(buf);
        return;
    }
}

static int isDigitsInRange(const char *s, int minLen, int maxLen) {
    int len = (int)strlen(s);
    return isDigitsOnly(s) && len >= minLen && len <= maxLen;
}

static void printDigitsError(int minLen, int maxLen) {
    if (minLen == maxLen)
        printf("  [!] Phai gom dung %d chu so.\n", minLen);
    else
        printf("  [!] Chi duoc nhap chu so, tu %d den %d ky tu.\n", minLen, maxLen);
}

void inputDigits(const char *prompt, char *buf, int minLen, int maxLen) {
    char line[LINE_BUF];
    for (;;) {
        printf("%s", prompt);
        if (readLine(line, sizeof(line)) && isDigitsInRange(line, minLen, maxLen)) {
            strcpy(buf, line);
            return;
        }
        printDigitsError(minLen, maxLen);
    }
}

Date inputDate(const char *prompt) {
    char buf[LINE_BUF];
    Date d;
    for (;;) {
        printf("%s", prompt);
        if (readLine(buf, sizeof(buf)) && parseDate(buf, &d)) {
            if (isValidDate(d)) return d;
            printf("  [!] Ngay khong ton tai (kiem tra so ngay trong thang / nam nhuan).\n");
        } else {
            printf("  [!] Sai dinh dang. Nhap theo dang dd/mm/yyyy.\n");
        }
    }
}

int inputYesNo(const char *prompt) {
    char buf[LINE_BUF];
    for (;;) {
        printf("%s (y/n): ", prompt);
        readLine(buf, sizeof(buf));
        if (equalsIgnoreCase(buf, "y") || equalsIgnoreCase(buf, "c")) return 1;
        if (equalsIgnoreCase(buf, "n") || equalsIgnoreCase(buf, "k")) return 0;
        printf("  [!] Nhap y hoac n.\n");
    }
}

int inputIntOrKeep(const char *prompt, int min, int max, int current) {
    char buf[LINE_BUF];
    int v;
    for (;;) {
        printf("%s [%d]: ", prompt, current);
        if (!readLine(buf, sizeof(buf))) { printf("  [!] Dong nhap qua dai.\n"); continue; }
        if (buf[0] == '\0') return current;
        if (parseInt(buf, &v) && v >= min && v <= max) return v;
        printf("  [!] Nhap so nguyen trong khoang %d..%d (Enter de giu nguyen).\n", min, max);
    }
}

double inputDoubleOrKeep(const char *prompt, double min, double max, double current) {
    char buf[LINE_BUF];
    double v;
    for (;;) {
        printf("%s [%.0f]: ", prompt, current);
        if (!readLine(buf, sizeof(buf))) { printf("  [!] Dong nhap qua dai.\n"); continue; }
        if (buf[0] == '\0') return current;
        if (parseDouble(buf, &v) && v >= min && v <= max) return v;
        printf("  [!] Nhap so trong khoang %.0f..%.0f (Enter de giu nguyen).\n", min, max);
    }
}

void inputStringOrKeep(const char *prompt, char *buf, int size) {
    char line[LINE_BUF];
    for (;;) {
        printf("%s [%s]: ", prompt, buf);
        if (!readLine(line, sizeof(line))) { printf("  [!] Dong nhap qua dai.\n"); continue; }
        if (line[0] == '\0') return;
        if ((int)strlen(line) >= size) {
            printf("  [!] Toi da %d ky tu.\n", size - 1);
            continue;
        }
        strcpy(buf, line);
        return;
    }
}

void inputDigitsOrKeep(const char *prompt, char *buf, int minLen, int maxLen) {
    char line[LINE_BUF];
    for (;;) {
        printf("%s [%s]: ", prompt, buf);
        if (!readLine(line, sizeof(line))) { printf("  [!] Dong nhap qua dai.\n"); continue; }
        if (line[0] == '\0') return;
        if (isDigitsInRange(line, minLen, maxLen)) {
            strcpy(buf, line);
            return;
        }
        printDigitsError(minLen, maxLen);
        printf("      (Enter de giu nguyen)\n");
    }
}

Date inputDateOrKeep(const char *prompt, Date current) {
    char buf[LINE_BUF], cur[DATE_STR_LEN];
    Date d;
    formatDate(current, cur, sizeof(cur));
    for (;;) {
        printf("%s [%s]: ", prompt, cur);
        if (!readLine(buf, sizeof(buf))) { printf("  [!] Dong nhap qua dai.\n"); continue; }
        if (buf[0] == '\0') return current;
        if (parseDate(buf, &d) && isValidDate(d)) return d;
        printf("  [!] Ngay khong hop le. Nhap dd/mm/yyyy (Enter de giu nguyen).\n");
    }
}

void pauseScreen(void) {
    char buf[LINE_BUF];
    printf("\nNhan Enter de tiep tuc...");
    readLine(buf, sizeof(buf));
}
