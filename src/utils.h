/*
 * utils.h - Các hàm tiện ích: xử lý ngày tháng và nhập dữ liệu có kiểm tra.
 */
#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

#define MIN_YEAR 1900
#define MAX_YEAR 9999
#define LINE_BUF 256     /* Độ dài tối đa một dòng nhập từ bàn phím */
#define DATE_STR_LEN 11  /* "dd/mm/yyyy" + '\0' */

typedef struct {
    int day;
    int month;
    int year;
} Date;

/* ---------- Ngày tháng ---------- */
int  isLeapYear(int year);
int  daysInMonth(int month, int year);
int  isValidDate(Date d);
long dateToDays(Date d);              /* Số ngày tính từ mốc cố định */
int  compareDate(Date a, Date b);     /* <0 nếu a<b, 0 nếu bằng, >0 nếu a>b */
Date addDays(Date d, int n);          /* n có thể âm */
Date getToday(void);
void formatDate(Date d, char *buf, size_t size);

/* ---------- Chuỗi ---------- */
void trim(char *s);
void toUpperStr(char *s);
int  equalsIgnoreCase(const char *a, const char *b);
int  containsIgnoreCase(const char *text, const char *key);
int  isDigitsOnly(const char *s);

/* ---------- Nhập dữ liệu có kiểm tra ---------- */
int    readLine(char *buf, int size);  /* 1 = OK, 0 = dòng quá dài */
int    inputInt(const char *prompt, int min, int max);
double inputDouble(const char *prompt, double min, double max);
void   inputString(const char *prompt, char *buf, int size, int allowEmpty);
void   inputCode(const char *prompt, char *buf, int size);      /* Mã: không rỗng, không dấu cách, viết hoa */
void   inputDigits(const char *prompt, char *buf, int exactLen); /* Chuỗi chữ số đúng độ dài */
Date   inputDate(const char *prompt);
int    inputYesNo(const char *prompt);

/* Các biến thể "Enter để giữ nguyên" dùng khi sửa bản ghi */
int    inputIntOrKeep(const char *prompt, int min, int max, int current);
double inputDoubleOrKeep(const char *prompt, double min, double max, double current);
void   inputStringOrKeep(const char *prompt, char *buf, int size);
void   inputDigitsOrKeep(const char *prompt, char *buf, int exactLen);
Date   inputDateOrKeep(const char *prompt, Date current);

void pauseScreen(void);

#endif /* UTILS_H */
