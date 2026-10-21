# screen.c

Cài đặt các **màn hình giao diện** của đồng hồ báo thức trên OLED SH1106 128x64. Mỗi hàm `Screen_...` vẽ trọn một màn hình: xóa bộ đệm, vẽ chữ, rồi đẩy ra màn hình.

## Phụ thuộc

- `screen.h` (kéo theo `oled.h`) và `<stdio.h>` (`sprintf`).
- Dùng các hàm của `oled.c`: `OLED_Clear`, `OLED_SetCursor`, `OLED_WriteString`, `OLED_UpdateScreen`.
- Dùng `HAL_Delay`.
- Biến nội bộ: `static char textBuffer[32]` để định dạng chuỗi số bằng `sprintf`.

## Quy ước chung

- Mọi hàm đều theo mẫu: `OLED_Clear()` → vẽ → `OLED_UpdateScreen()`, tức là **vẽ lại toàn bộ màn hình** mỗi lần gọi.
- Tọa độ tính bằng pixel, `OLED_SetCursor(x, y)` đặt góc trên bên trái của chuỗi.
- Font 5x7: `scale = 1` mỗi ký tự rộng 6 pixel (21 ký tự trên một dòng), `scale = 2` mỗi ký tự rộng 12 pixel.
- Chuỗi tiêu đề và đường kẻ: đường kẻ ngang là chuỗi 21 dấu `-` đặt ở `(0, 11)` (rộng đúng 126 pixel).
- Font chỉ có chữ in hoa nên mọi chuỗi dùng tiếng Việt không dấu, viết hoa.

## Hàm

### `Screen_ShowWelcome()`

Màn hình chào: "ALARM CLOCK" tại (20, 16) và "RTC STM32F103" tại (20, 36), sau đó dừng 1000 ms. Được gọi một lần trong `main()`.

### `Screen_DrawClock(h, m, s, alarmHour, alarmMinute, alarmEnable)`

| Thành phần | Vị trí | Scale |
|---|---|---|
| Tiêu đề "ALARM CLOCK" | (28, 1) | 1 |
| Đường kẻ | (0, 11) | 1 |
| Giờ `HH:MM:SS` | (16, 22) | 2 |
| Dòng báo thức `AL: HH:MM ON/OFF` | (19, 51) | 1 |

### `Screen_DrawMenu(menuItem)`

Tiêu đề "CAI DAT" tại (40, 1), đường kẻ, rồi ba mục tại x = 10, y = 20 / 35 / 50: `GIO THUC`, `BAO THUC`, `THOAT`. Mục đang chọn có tiền tố `> `, mục còn lại có hai dấu cách.

### `Screen_DrawSetTime(editHour, editMinute, editSecond, editTimeField)`

- Tiêu đề "GIO THUC" tại (39, 1), đường kẻ.
- Giờ `HH:MM:SS` tại (16, 22), scale 2.
- **Con trỏ gạch chân** `--` ở y = 42, x phụ thuộc trường đang chỉnh: 16 (giờ), 52 (phút), 88 (giây). Các vị trí này khớp với ký tự của chuỗi giờ ở scale 2 (mỗi ký tự 12 pixel, hai chữ số và một dấu `:` là 36 pixel).
- Dòng hướng dẫn "NHAN:TIEP GIU:LUU" tại (7, 54): nhấn để sang trường tiếp, giữ để lưu.

### `Screen_DrawSetAlarm(editAlarmHour, editAlarmMinute, editAlarmEnable, editAlarmField)`

Giống màn hình chỉnh giờ nhưng:

- Tiêu đề "BAO THUC".
- Chuỗi chính là `HH:MM ON/OFF` tại (16, 22), scale 2.
- Gạch chân: x = 16 (giờ), 52 (phút), 88 (bật/tắt, vẽ `----` dài hơn vì chữ `ON/OFF` rộng hơn).

### `Screen_DrawAlarmRinging(alarmHour, alarmMinute)`

"BAO THUC!" tại (39, 5), giờ báo thức `HH:MM` tại (34, 23) scale 2, và dòng "NHAN DE TAT COI" tại (18, 52).

### `Screen_ShowSaved()`

Hiện "DA LUU" tại (42, 25) rồi dừng 500 ms. Được `longPress()` trong `button.c` gọi sau khi lưu giờ hoặc báo thức.

## Lưu ý

- `Screen_ShowWelcome` (1 s) và `Screen_ShowSaved` (0,5 s) dùng `HAL_Delay` nên **chặn vòng lặp chính** trong lúc hiển thị, kể cả việc quét nút và còi.
- Mỗi lần vẽ đẩy cả khung hình qua I2C (khoảng 100 ms ở 100 kHz), vì vậy không nên gọi các hàm vẽ quá dày.
- Tên tham số (`alarmHour`, `editHour`...) trùng tên biến toàn cục của `main.c` nhưng chỉ là tham số cục bộ của hàm; các hàm trong file này không truy cập biến toàn cục.
