# Quản lý đặt phòng khách sạn (BTL Kỹ thuật lập trình – Đề tài 6)

Chương trình console viết bằng C99. Dữ liệu được lưu trong RAM bằng 3 danh sách liên kết đơn (phòng, khách, phiếu đặt) và ghi xuống file nhị phân `.dat`.

## Cấu trúc thư mục

```
src/
  main.c              menu chính và các menu con (menuRoom, menuGuest, menuBooking, menuReport)
  utils.c / utils.h   ngày tháng (năm nhuận, dateToDays...), nhập dữ liệu có kiểm tra
  room.c  / room.h    thêm/sửa/xóa/tìm phòng, lọc, sắp xếp bằng con trỏ hàm
  guest.c / guest.h   thêm/sửa/xóa khách, tìm theo CCCD hoặc tên
  booking.c/booking.h kiểm tra trùng lịch, đặt/nhận/trả/hủy phòng, tính tiền, hóa đơn
  report.c / report.h doanh thu theo tháng/phòng, phòng trống hôm nay, khách đang ở
  fileio.c / fileio.h đọc/ghi rooms.dat, guests.dat, bookings.dat
data/                 dữ liệu mẫu (6 phòng, 5 khách, 7 phiếu đặt)
tests/seed_input.txt  kịch bản nhập dùng để tạo dữ liệu mẫu
build.bat             script biên dịch trên Windows
```

## Biên dịch và chạy

Cần có `gcc` (MinGW, w64devkit, hoặc gcc đi kèm Dev-C++ / Code::Blocks). Chạy từ thư mục gốc của dự án:

```
gcc -std=c99 -Wall -Wextra src/*.c -o hotel
hotel                 # dùng thư mục mặc định data/
hotel D:\du_lieu      # truyền thư mục dữ liệu qua đối dòng lệnh
```

Trên Windows có thể chạy `build.bat` thay cho lệnh gcc.

Đường dẫn `data/` tính từ thư mục làm việc hiện tại. Nếu chạy trong IDE mà không thấy dữ liệu, hãy đặt thư mục làm việc là thư mục gốc dự án.

Muốn bắt đầu với dữ liệu trống thì xóa thư mục `data/`; chương trình sẽ tự tạo lại. Muốn tạo lại dữ liệu mẫu: xóa `data/` rồi chạy `hotel < tests\seed_input.txt`.

## Quy tắc nghiệp vụ

- Phòng không lưu trạng thái "trống". Phòng trống trong khoảng `[ngày nhận, ngày trả)` khi không có phiếu *Đã đặt* hoặc *Đang ở* nào trùng khoảng đó.
- Hai khoảng ngày trùng nhau khi `newIn < oldOut && newOut > oldIn`. Vì vậy đặt nối tiếp (10–12 rồi 12–14) được chấp nhận.
- Ngày phải hợp lệ (đúng số ngày trong tháng, tính năm nhuận) và ngày trả > ngày nhận. Mỗi lần đặt tối đa 365 đêm.
- Số đêm = `dateToDays(checkOut) - dateToDays(checkIn)`, nên tính đúng cả khi qua cuối tháng hoặc cuối năm (30/12 → 02/01 = 3 đêm).
- Trạng thái phiếu: 0 Đã đặt → 1 Đang ở (check-in) → 2 Đã trả (check-out). Chỉ phiếu Đã đặt mới hủy được (→ 3).
- Mã phòng, mã khách và CCCD không được trùng. Mã khách do chương trình tự sinh (KH001, KH002, ...).
- CCCD và số điện thoại chỉ gồm chữ số, tối đa lần lượt 12 và 10 ký tự. Độ dài tối thiểu chỉnh bằng `ID_CARD_MIN_DIGITS` / `PHONE_MIN_DIGITS` trong `guest.h`.
- Tìm phòng theo mã là tìm gần đúng: nhập `2` sẽ ra mọi phòng có chữ `2` trong mã.
- Khi đặt phòng, sửa, xóa hoặc xem lịch sử khách, người dùng tìm khách theo tên, CCCD, SĐT hoặc mã rồi chọn theo số thứ tự (STT). Khi đặt phòng có thể thêm khách mới ngay tại bước chọn khách.
- Không được xóa phòng hoặc khách đang có phiếu Đã đặt / Đang ở.

### Tính tiền (các mức khai báo bằng `#define` trong `booking.h`)

- Tiền phòng = số đêm đặt × giá một đêm. Trả sớm vẫn tính đủ số đêm đã đặt.
- Ở từ `LONG_STAY_NIGHTS` (7) đêm trở lên: giảm `LONG_STAY_DISCOUNT` (10%) tiền phòng.
- Trả muộn: mỗi ngày muộn tính thêm 1 đêm cộng phụ thu `LATE_FEE_RATE` (50%). Ngày trả của phiếu được cập nhật thành ngày trả thực tế.
- Doanh thu chỉ tính các phiếu *Đã trả*, gộp theo tháng của ngày trả phòng.

## Lưu file

- Mỗi file có dạng `[int số bản ghi][bản ghi 1][bản ghi 2]...`. Chương trình chỉ ghi phần dữ liệu (`Room`, `Guest`, `Booking`), không ghi con trỏ `next`.
- Dữ liệu được tự động lưu sau mỗi thao tác thêm/sửa/xóa, khi chọn "Lưu dữ liệu" và khi thoát. Mỗi lần ghi, chương trình ghi ra file `.tmp` trước rồi mới thay file cũ.
- Lần chạy đầu chưa có file thì chương trình bắt đầu với danh sách rỗng, không báo lỗi.

## Hàm đệ quy

- `sumRevenue` (report.c): tổng doanh thu một tháng.
- `printHistoryReverse` (booking.c): lịch sử đặt phòng của một khách, mới nhất trước.
- `countAvailable` (booking.c): đếm số phòng trống trong một khoảng ngày.


## Chạy bộ dữ liệu demo

Thư mục `demo_data/` có sẵn 6 phòng, 5 khách và 7 phiếu đặt ở các trạng thái Đã đặt, Đang ở, Đã trả và Hủy. Dữ liệu này tách riêng với `data/`, nên có thể dùng để xem các menu mà không cần nhập lại thông tin.

Chạy từ thư mục gốc của project, truyền `demo_data` làm tham số:

```text
Windows: hotel.exe demo_data
macOS/Linux: ./hotel demo_data
```

Nếu dùng CMake, thay `hotel.exe` hoặc `./hotel` bằng đường dẫn tới executable vừa build, ví dụ `cmake-build-debug/TripMaster.exe demo_data` trên Windows. Đường dẫn `demo_data` được tính từ thư mục làm việc hiện tại.
