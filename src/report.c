#include <stdio.h>
#include "report.h"

/* De quy cong tien cac phieu da tra trong dung thang va nam duoc chon. */
// Hàm [sumRevenue] cộng doanh thu các phiếu đã trả của tháng/năm.
double sumRevenue(const BookingNode *p, int month, int year) {
    // Khai báo biến [here] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    double here = 0.0;
    // Kiểm tra điều kiện [p == NULL]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (p == NULL) return 0.0;
    // Kiểm tra phiếu đã trả và ngày trả thuộc đúng tháng, năm đang thống kê; chỉ các phiếu này mới được cộng doanh thu.
    if (p->data.status == BK_CHECKEDOUT &&
        p->data.checkOut.month == month && p->data.checkOut.year == year) {
        // Lưu [giá trị biểu thức [p->data.totalAmount]] vào biến [here] để các lệnh sau sử dụng.
        here = p->data.totalAmount;
    }
    // Trả giá trị [here + sumRevenue(p->next, month, year)] về cho hàm đã gọi dòng này.
    return here + sumRevenue(p->next, month, year);
}

void revenueByMonth(const BookingNode *bookings, int year) {
    // Khai báo biến [m] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int m;
    // Khai báo biến [total] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    double total = 0.0, v;
    // Hiển thị nội dung [DOANH THU THEO THANG - NAM %d (tinh theo ngay tra phong)] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("\nDOANH THU THEO THANG - NAM %d (tinh theo ngay tra phong)\n", year);
    // Hiển thị nội dung [+-------+---------------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-------+---------------------+\n");
    // Hiển thị nội dung [| Thang |     Doanh thu (VND) |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("| Thang |     Doanh thu (VND) |\n");
    // Hiển thị nội dung [+-------+---------------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-------+---------------------+\n");
    // Lặp theo ba phần [m = 1; m <= 12; m++]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (m = 1; m <= 12; m++) {
        // Lưu [giá trị biểu thức [sumRevenue(bookings, m, year)]] vào biến [v] để các lệnh sau sử dụng.
        v = sumRevenue(bookings, m, year);
        // Cập nhật biến [total] bằng phép [+=], thường để tăng bộ đếm hoặc cộng dồn kết quả.
        total += v;
        // Hiển thị nội dung [| %5d | %19.0f |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("| %5d | %19.0f |\n", m, v);
    }
    // Hiển thị nội dung [+-------+---------------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-------+---------------------+\n");
    // Hiển thị nội dung [| Tong  | %19.0f |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("| Tong  | %19.0f |\n", total);
    // Hiển thị nội dung [+-------+---------------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+-------+---------------------+\n");
}

/* Cong doanh thu va dem phieu da tra rieng cho tung phong. */
void revenueByRoom(const RoomNode *rooms, const BookingNode *bookings) {
    const RoomNode *r;
    const BookingNode *b;
    // Khai báo biến [total] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    double total = 0.0;

    // Hiển thị nội dung [DOANH THU THEO PHONG (cac phieu da tra)] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("\nDOANH THU THEO PHONG (cac phieu da tra)\n");
    // Hiển thị nội dung [+------------+------+----------+---------------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+------------+------+----------+---------------------+\n");
    // Hiển thị nội dung [| Ma phong   | Loai | So phieu |     Doanh thu (VND) |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("| Ma phong   | Loai | So phieu |     Doanh thu (VND) |\n");
    // Hiển thị nội dung [+------------+------+----------+---------------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+------------+------+----------+---------------------+\n");
    // Lặp theo ba phần [r = rooms; r != NULL; r = r->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (r = rooms; r != NULL; r = r->next) {
        // Khai báo biến [sum] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
        double sum = 0.0;
        // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
        int count = 0;
        // Lặp theo ba phần [b = bookings; b != NULL; b = b->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
        for (b = bookings; b != NULL; b = b->next) {
            // Kiểm tra điều kiện [b->data.status == BK_CHECKEDOUT && equalsIgnoreCase(b->data.roomId, r->data.roomId)]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
            if (b->data.status == BK_CHECKEDOUT && equalsIgnoreCase(b->data.roomId, r->data.roomId)) {
                // Cập nhật biến [sum] bằng phép [+=], thường để tăng bộ đếm hoặc cộng dồn kết quả.
                sum += b->data.totalAmount;
                // Cập nhật biến [count] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
                count++;
            }
        }
        // Cập nhật biến [total] bằng phép [+=], thường để tăng bộ đếm hoặc cộng dồn kết quả.
        total += sum;
        // Hiển thị nội dung [| %-10s | %-4s | %8d | %19.0f |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("| %-10s | %-4s | %8d | %19.0f |\n", r->data.roomId, roomTypeName(r->data.type), count, sum);
    }
    // Hiển thị nội dung [+------------+------+----------+---------------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+------------+------+----------+---------------------+\n");
    // Hiển thị nội dung [| %-10s |      |          | %19.0f |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("| %-10s |      |          | %19.0f |\n", "Tong", total);
    // Hiển thị nội dung [+------------+------+----------+---------------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+------------+------+----------+---------------------+\n");
}

/* Thong ke phong trong cho dem nay, tu ngay hom nay den ngay mai. */
void reportAvailableToday(const RoomNode *rooms, const BookingNode *bookings) {
    // Khai báo biến [today] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Date today = getToday();
    // Khai báo biến [tomorrow] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    Date tomorrow = addDays(today, 1);
    // Khai báo biến [buf] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char buf[DATE_STR_LEN];
    // Khai báo biến [total] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int total = countRooms(rooms);
    // Khai báo biến [freeRooms] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int freeRooms = countAvailable(rooms, today, tomorrow, bookings);

    // Gọi hàm [formatDate] để định dạng ngày thành chuỗi dd/mm/yyyy để in hoặc ghép vào thông báo.
    formatDate(today, buf, sizeof(buf));
    // Hiển thị nội dung [Hom nay %s: %d / %d phong con trong (dem nay).] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("\nHom nay %s: %d / %d phong con trong (dem nay).\n", buf, freeRooms, total);
    // Kiểm tra điều kiện [freeRooms > 0]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
    if (freeRooms > 0) listAvailableRooms(rooms, bookings, today, tomorrow);
}

/* In danh sach cac phieu Dang o va thong tin khach tu danh sach khach. */
void listCurrentGuests(const BookingNode *bookings, GuestNode *guests) {
    const BookingNode *b;
    // Khai báo biến [in] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    char in[DATE_STR_LEN], out[DATE_STR_LEN];
    // Khai báo biến [count] để giữ dữ liệu dùng trong bước xử lý tiếp theo.
    int count = 0;

    // Hiển thị nội dung [DANH SACH KHACH DANG O] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("\nDANH SACH KHACH DANG O\n");
    // Hiển thị nội dung [+------------+-------------+--------------------------------+--------------+------------+------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+------------+-------------+--------------------------------+--------------+------------+------------+\n");
    // Hiển thị nội dung [| %-10s | %-11s | %-30s | %-12s | %-10s | %-10s |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("| %-10s | %-11s | %-30s | %-12s | %-10s | %-10s |\n",
           "Phong", "Ma khach", "Ho ten", "CCCD", "Ngay nhan", "Ngay tra");
    // Hiển thị nội dung [+------------+-------------+--------------------------------+--------------+------------+------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+------------+-------------+--------------------------------+--------------+------------+------------+\n");
    // Lặp theo ba phần [b = bookings; b != NULL; b = b->next]: khởi tạo, kiểm tra điều kiện rồi cập nhật biến lặp.
    for (b = bookings; b != NULL; b = b->next) {
        GuestNode *g;
        // Kiểm tra điều kiện [b->data.status != BK_STAYING]; chỉ khi đúng thì chạy lệnh hoặc khối ngay sau đây.
        if (b->data.status != BK_STAYING) continue;
        // Lưu [nút khách có mã cần tìm, hoặc NULL nếu không tồn tại] vào biến [g] để các lệnh sau sử dụng.
        g = findGuestById(guests, b->data.guestId);
        // Gọi hàm [formatDate] để định dạng ngày thành chuỗi dd/mm/yyyy để in hoặc ghép vào thông báo.
        formatDate(b->data.checkIn, in, sizeof(in));
        // Gọi hàm [formatDate] để định dạng ngày thành chuỗi dd/mm/yyyy để in hoặc ghép vào thông báo.
        formatDate(b->data.checkOut, out, sizeof(out));
        // Hiển thị nội dung [| %-10s | %-11s | %-30.30s | %-12s | %-10s | %-10s |] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
        printf("| %-10s | %-11s | %-30.30s | %-12s | %-10s | %-10s |\n",
               b->data.roomId, b->data.guestId,
               g != NULL ? g->data.fullName : "(khong ro)",
               g != NULL ? g->data.idCard : "",
               in, out);
        // Cập nhật biến [count] bằng phép [++], thường để tăng bộ đếm hoặc cộng dồn kết quả.
        count++;
    }
    // Hiển thị nội dung [+------------+-------------+--------------------------------+--------------+------------+------------+] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("+------------+-------------+--------------------------------+--------------+------------+------------+\n");
    // Hiển thị nội dung [Tong: %d phong dang co khach.] ra màn hình; các tham số sau chuỗi thay cho ký hiệu % nếu có.
    printf("Tong: %d phong dang co khach.\n", count);
}
