#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

#define MIN_YEAR 1900
#define MAX_YEAR 9999
#define LINE_BUF 256
#define DATE_STR_LEN 11

typedef struct {
    int day;
    int month;
    int year;
} Date;

int  isLeapYear(int year);
int  daysInMonth(int month, int year);
int  isValidDate(Date d);
long dateToDays(Date d);
int  compareDate(Date a, Date b);
Date addDays(Date d, int n);
Date getToday(void);
void formatDate(Date d, char *buf, size_t size);

void trim(char *s);
void toUpperStr(char *s);
int  equalsIgnoreCase(const char *a, const char *b);
int  containsIgnoreCase(const char *text, const char *key);
int  isDigitsOnly(const char *s);

int    readLine(char *buf, int size);
int    inputInt(const char *prompt, int min, int max);
double inputDouble(const char *prompt, double min, double max);
void   inputString(const char *prompt, char *buf, int size, int allowEmpty);
void   inputCode(const char *prompt, char *buf, int size);
void   inputDigits(const char *prompt, char *buf, int minLen, int maxLen);
Date   inputDate(const char *prompt);
int    inputYesNo(const char *prompt);

int    inputIntOrKeep(const char *prompt, int min, int max, int current);
double inputDoubleOrKeep(const char *prompt, double min, double max, double current);
void   inputStringOrKeep(const char *prompt, char *buf, int size);
void   inputDigitsOrKeep(const char *prompt, char *buf, int minLen, int maxLen);
Date   inputDateOrKeep(const char *prompt, Date current);

void pauseScreen(void);

#endif
