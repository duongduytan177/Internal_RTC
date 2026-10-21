# oled.h

Header của **driver màn hình OLED SH1106 128x64 giao tiếp I2C**. File khai báo hằng số và các hàm công khai mà `oled.c` cung cấp.

## Phụ thuộc

- Include: `main.h` (HAL).
- Được include bởi: `screen.h` (nên `screen.c` và `main.c` dùng được các hàm OLED), `main.c`.
- Include guard: `__OLED_H`, có bọc `extern "C"` cho C++.

## Ghi chú phần cứng (theo comment trong file)

- Giao tiếp qua **I2C1**: PB6 (SCL), PB7 (SDA) trên STM32F103C8T6.
- Chip điều khiển **SH1106**: RAM rộng 132 cột trong khi màn hình chỉ 128 cột, nên khác SSD1306 ở độ lệch 2 cột (xử lý trong `OLED_UpdateScreen`).

## Hằng số

| Macro | Giá trị | Ý nghĩa |
|---|---|---|
| `OLED_ADDR` | `(0x3C << 1)` | Địa chỉ I2C 7 bit 0x3C, dịch trái 1 bit theo yêu cầu của HAL |
| `OLED_WIDTH` | `128` | Chiều rộng (pixel) |
| `OLED_HEIGHT` | `64` | Chiều cao (pixel) |

## Hàm công khai

| Hàm | Mô tả |
|---|---|
| `void OLED_Init(void)` | Gửi chuỗi lệnh khởi tạo SH1106, xóa và đẩy màn hình trống |
| `void OLED_Clear(void)` | Xóa bộ đệm khung hình trong RAM (chưa hiện lên màn hình) |
| `void OLED_UpdateScreen(void)` | Đẩy toàn bộ bộ đệm ra màn hình qua I2C |
| `void OLED_SetCursor(uint8_t x, uint8_t y)` | Đặt vị trí con trỏ (pixel) cho lần ghi chữ kế tiếp |
| `void OLED_WriteChar(char c, uint8_t scale)` | Vẽ một ký tự font 5x7, phóng đại `scale` lần |
| `void OLED_WriteString(const char *str, uint8_t scale)` | Vẽ chuỗi, con trỏ tự dịch sang phải sau mỗi ký tự |
| `void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color)` | Bật (`color != 0`) hoặc tắt một pixel |
| `void OLED_DrawHLine(uint8_t x, uint8_t y, uint8_t len)` | Vẽ đường ngang dài `len` pixel |

## Macro tên thay thế

Các macro `SH1106_Init`, `SH1106_Clear`, `SH1106_UpdateScreen`, `SH1106_SetCursor`, `SH1106_WriteChar`, `SH1106_WriteString` là bí danh trỏ tới các hàm `OLED_...` tương ứng, để code dùng tên kiểu thư viện SH1106 vẫn biên dịch được.

## Lưu ý

- Thứ tự dùng điển hình: `OLED_Clear()` → `OLED_SetCursor()` → `OLED_WriteString()` → `OLED_UpdateScreen()`. Các hàm vẽ chỉ ghi vào bộ đệm, phải gọi `OLED_UpdateScreen()` thì mới thấy kết quả.
- Nếu đổi sang module SSD1306 thì độ lệch 2 cột trong `oled.c` phải đổi về 0.
