/*
 * fileio.c - Cài đặt đọc/ghi file nhị phân.
 */
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

#define MAX_RECORDS 1000000   /* Chặn số bản ghi vô lý khi file bị hỏng */

static char g_dataDir[DIR_LEN] = DATA_DIR;

void setDataDir(const char *dir) {
    size_t len;
    if (dir == NULL || dir[0] == '\0') return;
    len = strlen(dir);
    if (len + 2 > sizeof(g_dataDir)) {
        printf("[!] Duong dan thu muc qua dai, dung mac dinh %s\n", DATA_DIR);
        return;
    }
    strcpy(g_dataDir, dir);
    if (dir[len - 1] != '/' && dir[len - 1] != '\\') strcat(g_dataDir, "/");
}

const char *getDataDir(void) {
    return g_dataDir;
}

static void buildPath(char *out, size_t size, const char *fileName) {
    snprintf(out, size, "%s%s", g_dataDir, fileName);
}

/* Tạo thư mục dữ liệu nếu chưa có (bỏ dấu '/' cuối trước khi gọi mkdir) */
static void ensureDataDir(void) {
    char dir[DIR_LEN];
    size_t len;
    strcpy(dir, g_dataDir);
    len = strlen(dir);
    while (len > 1 && (dir[len - 1] == '/' || dir[len - 1] == '\\')) dir[--len] = '\0';
    if (MAKE_DIR(dir) != 0 && errno != EEXIST) {
        /* Không tạo được: fopen ở bước sau sẽ báo lỗi cụ thể */
    }
}

/*
 * Ghi an toàn: ghi vào file tạm trước, ghi xong mới thay file cũ.
 * Nếu chương trình dừng giữa chừng thì file cũ vẫn còn nguyên.
 */
static FILE *openTemp(const char *fileName, char *tmpPath, size_t size) {
    FILE *f;
    ensureDataDir();
    snprintf(tmpPath, size, "%s%s.tmp", g_dataDir, fileName);
    f = fopen(tmpPath, "wb");
    if (f == NULL) printf("[!] Khong mo duoc file %s de ghi.\n", tmpPath);
    return f;
}

static int commitTemp(FILE *f, const char *fileName, const char *tmpPath, int ok) {
    char path[PATH_LEN];
    if (fclose(f) != 0) ok = 0;
    if (!ok) {
        remove(tmpPath);
        printf("[!] Loi khi ghi file %s.\n", fileName);
        return 0;
    }
    buildPath(path, sizeof(path), fileName);
    remove(path);  /* Trên Windows, rename không ghi đè file đã có */
    if (rename(tmpPath, path) != 0) {
        printf("[!] Khong doi ten duoc %s thanh %s.\n", tmpPath, path);
        return 0;
    }
    return 1;
}

/* Mở file để đọc và đọc số bản ghi ở đầu file. Trả về NULL nếu chưa có file. */
static FILE *openForRead(const char *fileName, int *count) {
    char path[PATH_LEN];
    FILE *f;
    buildPath(path, sizeof(path), fileName);
    f = fopen(path, "rb");
    if (f == NULL) return NULL;  /* Lần chạy đầu: chưa có file, bắt đầu rỗng */
    if (fread(count, sizeof(int), 1, f) != 1 || *count < 0 || *count > MAX_RECORDS) {
        printf("[!] File %s bi hong hoac rong, bo qua.\n", path);
        fclose(f);
        return NULL;
    }
    return f;
}

/* ===================== PHÒNG ===================== */

int saveRooms(const RoomNode *head) {
    char tmp[PATH_LEN];
    const RoomNode *p;
    int count = countRooms(head), ok;
    FILE *f = openTemp(ROOMS_FILE, tmp, sizeof(tmp));
    if (f == NULL) return 0;
    ok = fwrite(&count, sizeof(int), 1, f) == 1;
    for (p = head; ok && p != NULL; p = p->next) {
        ok = fwrite(&p->data, sizeof(Room), 1, f) == 1;
    }
    return commitTemp(f, ROOMS_FILE, tmp, ok);
}

int loadRooms(RoomNode **head) {
    int count, i, loaded = 0;
    Room r;
    FILE *f = openForRead(ROOMS_FILE, &count);
    if (f == NULL) return 0;
    for (i = 0; i < count; i++) {
        if (fread(&r, sizeof(Room), 1, f) != 1) {
            printf("[!] %s: chi doc duoc %d/%d ban ghi.\n", ROOMS_FILE, i, count);
            break;
        }
        r.roomId[ROOM_ID_LEN - 1] = '\0';
        r.note[ROOM_NOTE_LEN - 1] = '\0';
        if (r.type < ROOM_SINGLE || r.type > ROOM_VIP || r.roomId[0] == '\0') continue;
        if (addRoom(head, r) == LIST_OK) loaded++;
    }
    fclose(f);
    return loaded;
}

/* ===================== KHÁCH ===================== */

int saveGuests(const GuestNode *head) {
    char tmp[PATH_LEN];
    const GuestNode *p;
    int count = countGuests(head), ok;
    FILE *f = openTemp(GUESTS_FILE, tmp, sizeof(tmp));
    if (f == NULL) return 0;
    ok = fwrite(&count, sizeof(int), 1, f) == 1;
    for (p = head; ok && p != NULL; p = p->next) {
        ok = fwrite(&p->data, sizeof(Guest), 1, f) == 1;
    }
    return commitTemp(f, GUESTS_FILE, tmp, ok);
}

int loadGuests(GuestNode **head) {
    int count, i, loaded = 0;
    Guest g;
    FILE *f = openForRead(GUESTS_FILE, &count);
    if (f == NULL) return 0;
    for (i = 0; i < count; i++) {
        if (fread(&g, sizeof(Guest), 1, f) != 1) {
            printf("[!] %s: chi doc duoc %d/%d ban ghi.\n", GUESTS_FILE, i, count);
            break;
        }
        g.guestId[GUEST_ID_LEN - 1] = '\0';
        g.fullName[GUEST_NAME_LEN - 1] = '\0';
        g.idCard[ID_CARD_LEN - 1] = '\0';
        g.phone[PHONE_LEN - 1] = '\0';
        if (g.guestId[0] == '\0') continue;
        if (addGuest(head, g) == LIST_OK) loaded++;
    }
    fclose(f);
    return loaded;
}

/* ===================== ĐẶT PHÒNG ===================== */

int saveBookings(const BookingNode *head) {
    char tmp[PATH_LEN];
    const BookingNode *p;
    int count = 0, ok;
    FILE *f;
    for (p = head; p != NULL; p = p->next) count++;
    f = openTemp(BOOKINGS_FILE, tmp, sizeof(tmp));
    if (f == NULL) return 0;
    ok = fwrite(&count, sizeof(int), 1, f) == 1;
    for (p = head; ok && p != NULL; p = p->next) {
        ok = fwrite(&p->data, sizeof(Booking), 1, f) == 1;
    }
    return commitTemp(f, BOOKINGS_FILE, tmp, ok);
}

int loadBookings(BookingNode **head) {
    int count, i, loaded = 0;
    Booking b;
    BookingNode *node, *tail = *head;
    FILE *f = openForRead(BOOKINGS_FILE, &count);
    if (f == NULL) return 0;
    while (tail != NULL && tail->next != NULL) tail = tail->next;

    for (i = 0; i < count; i++) {
        if (fread(&b, sizeof(Booking), 1, f) != 1) {
            printf("[!] %s: chi doc duoc %d/%d ban ghi.\n", BOOKINGS_FILE, i, count);
            break;
        }
        b.roomId[ROOM_ID_LEN - 1] = '\0';
        b.guestId[GUEST_ID_LEN - 1] = '\0';
        if (b.status < BK_BOOKED || b.status > BK_CANCELLED) continue;
        if (!isValidDate(b.checkIn) || !isValidDate(b.checkOut)) continue;

        node = (BookingNode *)malloc(sizeof(BookingNode));
        if (node == NULL) {
            printf("[!] Khong du bo nho khi doc %s.\n", BOOKINGS_FILE);
            break;
        }
        node->data = b;
        node->next = NULL;
        if (tail == NULL) *head = node; else tail->next = node;
        tail = node;
        loaded++;
    }
    fclose(f);
    return loaded;
}

/* ===================== TỔNG HỢP ===================== */

int saveAll(const RoomNode *rooms, const GuestNode *guests, const BookingNode *bookings) {
    int ok = 1;
    ok &= saveRooms(rooms);
    ok &= saveGuests(guests);
    ok &= saveBookings(bookings);
    return ok;
}

int loadAll(RoomNode **rooms, GuestNode **guests, BookingNode **bookings) {
    int nr = loadRooms(rooms);
    int ng = loadGuests(guests);
    int nb = loadBookings(bookings);
    printf("Thu muc du lieu: %s\n", g_dataDir);
    printf("Da doc: %d phong, %d khach, %d phieu dat.\n", nr, ng, nb);
    return nr + ng + nb;
}
