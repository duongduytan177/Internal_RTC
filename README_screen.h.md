# screen.h

Header của **module giao diện người dùng (UI)** trên OLED. File định nghĩa các trạng thái màn hình (máy trạng thái hữu hạn) và khai báo các hàm vẽ do `screen.c` cung cấp.

## Phụ thuộc

- Include: `main.h` (HAL), `oled.h` (hàm vẽ cấp thấp).
- Được include bởi: `screen.c`, `button.h` (từ đó tới `button.c`), `main.c`.
- Include guard: `__SCREEN_H`, có bọc `extern "C"` cho C++.

## Kiểu `Screen_t`

Các trang giao diện (Finite State Machine):

| Giá trị | Màn hình |
|---|---|
| `SCREEN_CLOCK` (= 0) | Đồng hồ chính |
| `SCREEN_MENU` | Menu cài đặt |
| `SCREEN_SET_TIME` | Chỉnh giờ thực |
| `SCREEN_SET_ALARM` | Chỉnh báo thức |

Màn hình "chuông đang reo" **không** phải một giá trị của `Screen_t`, mà được điều khiển bằng cờ `alarmRinging` trong `main.c` (cờ này được ưu tiên khi vẽ).

## Hàm công khai

| Hàm | Mô tả |
|---|---|
| `void Screen_ShowWelcome(void)` | Màn hình chào lúc khởi động (dừng 1 giây) |
| `void Screen_DrawClock(uint8_t h, uint8_t m, uint8_t s, int alarmHour, int alarmMinute, uint8_t alarmEnable)` | Màn hình đồng hồ chính, kèm trạng thái báo thức |
| `void Screen_DrawMenu(int menuItem)` | Menu cài đặt, đánh dấu mục đang chọn |
| `void Screen_DrawSetTime(int editHour, int editMinute, int editSecond, int editTimeField)` | Màn hình chỉnh giờ, gạch chân trường đang chỉnh |
| `void Screen_DrawSetAlarm(int editAlarmHour, int editAlarmMinute, uint8_t editAlarmEnable, int editAlarmField)` | Màn hình chỉnh báo thức, gạch chân trường đang chỉnh |
| `void Screen_DrawAlarmRinging(int alarmHour, int alarmMinute)` | Màn hình khi chuông đang reo |
| `void Screen_ShowSaved(void)` | Thông báo "ĐÃ LƯU" (dừng 500 ms) |

## Lưu ý

- Các hàm vẽ chỉ nhận **giá trị truyền vào**, không đọc biến toàn cục. Dữ liệu do `updateDisplay()` trong `main.c` gom lại rồi truyền xuống.
- Thêm màn hình mới: thêm giá trị vào `Screen_t`, thêm hàm `Screen_Draw...` ở đây và trong `screen.c`, rồi thêm một `case` trong `updateDisplay()` của `main.c`.
