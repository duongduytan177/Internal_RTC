# oled.c

**Driver điều khiển trực tiếp màn hình OLED SH1106 128x64 qua I2C1.** Driver giữ một bộ đệm khung hình trong RAM, các hàm vẽ chỉ ghi vào bộ đệm, còn `OLED_UpdateScreen()` mới truyền bộ đệm ra màn hình.

## Phụ thuộc

- `oled.h` và `<string.h>` (`memset`, `memcpy`).
- `extern I2C_HandleTypeDef hi2c1;` (định nghĩa trong `main.c`, cấu hình ở `MX_I2C1_Init`).
- Dùng `HAL_I2C_Master_Transmit` (timeout 100 ms) và `HAL_Delay`.

## Biến nội bộ (static)

| Biến | Ý nghĩa |
|---|---|
| `buffer[1024]` | Bộ đệm khung hình: 128 cột x 64 dòng / 8 bit |
| `cursorX`, `cursorY` | Vị trí con trỏ ghi chữ (pixel) |
| `font5x7[][5]` | Bảng font 5x7 cho ASCII từ 32 (khoảng trắng) đến 90 (`Z`) |

### Bố cục bộ đệm

Màn hình chia thành 8 "page", mỗi page cao 8 pixel. Pixel (x, y) nằm ở:

```c
buffer[x + (y / 8) * 128]   // byte
bit (y % 8)                 // bit trong byte, bit 0 là hàng trên cùng của page
```

### Font

- Mỗi ký tự gồm 5 cột, mỗi cột là một byte, bit 0 là hàng trên cùng, dùng 7 hàng.
- Bảng chỉ có **ký tự in hoa, chữ số và một số dấu câu**. **Không có chữ thường**: ký tự ngoài khoảng 32 đến 90 (kể cả chữ thường) bị thay bằng khoảng trắng. Vì vậy giao diện dùng tiếng Việt không dấu viết hoa.
- Mỗi ký tự chiếm 6 pixel (5 pixel + 1 pixel cách), tức 21 ký tự trên một dòng ở `scale = 1`.

## Hàm

### `static void OLED_Command(uint8_t cmd)`

Gửi một lệnh tới SH1106: byte điều khiển `0x00` (lệnh) rồi byte lệnh, qua I2C. Không kiểm tra giá trị trả về.

### `void OLED_Init(void)`

Chờ 100 ms rồi gửi chuỗi lệnh cấu hình, sau đó xóa và đẩy màn hình trống:

| Lệnh | Ý nghĩa |
|---|---|
| `0xAE` | Tắt hiển thị khi đang cấu hình |
| `0xD5, 0x80` | Tỉ lệ chia xung nhịp hiển thị |
| `0xA8, 0x3F` | Multiplex ratio 1/64 |
| `0xD3, 0x00` | Offset hiển thị bằng 0 |
| `0x40` | Dòng bắt đầu hiển thị là dòng 0 |
| `0xAD, 0x8B` | Bật mạch tăng áp nội (DC-DC) của SH1106 |
| `0xA1` | Đảo chiều ngang (segment re-map) |
| `0xC8` | Đảo chiều dọc (COM scan) |
| `0xDA, 0x12` | Cấu hình chân COM |
| `0x81, 0x80` | Độ tương phản trung bình |
| `0xD9, 0x22` | Chu kỳ pre-charge |
| `0xDB, 0x35` | Mức VCOMH |
| `0xA4` | Hiển thị theo nội dung RAM |
| `0xA6` | Hiển thị bình thường (không đảo màu) |
| `0xAF` | Bật hiển thị |

### `void OLED_Clear(void)`

`memset` toàn bộ `buffer` về 0.

### `void OLED_UpdateScreen(void)`

Với mỗi page 0 đến 7:

1. Gửi lệnh `0xB0 + page` để chọn page.
2. Gửi `0x02` (cột thấp) và `0x10` (cột cao): bắt đầu từ cột 2, **bù độ lệch của SH1106** (RAM 132 cột, màn hình 128 cột).
3. Chép 128 byte của page vào gói `data[129]` (byte đầu `0x40` báo "dữ liệu") và gửi trong một lần truyền I2C.

### `void OLED_SetCursor(uint8_t x, uint8_t y)`

Lưu vị trí con trỏ.

### `void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color)`

Bật hoặc tắt một pixel trong bộ đệm. Tọa độ ngoài màn hình bị bỏ qua.

### `void OLED_DrawHLine(uint8_t x, uint8_t y, uint8_t len)`

Vẽ đường ngang bằng cách gọi `OLED_DrawPixel` `len` lần. Hiện chưa được `screen.c` dùng (đường kẻ ngang trong giao diện được vẽ bằng chuỗi dấu `-`).

### `void OLED_WriteChar(char c, uint8_t scale)`

1. Ký tự ngoài khoảng 32 đến 90 được thay bằng khoảng trắng.
2. Duyệt 5 cột x 7 hàng của font, mỗi pixel bật được vẽ thành khối `scale x scale` pixel, ghi thẳng vào `buffer` (có kiểm tra biên).
3. Dịch con trỏ sang phải `6 * scale` pixel.

### `void OLED_WriteString(const char *str, uint8_t scale)`

Gọi `OLED_WriteChar` cho từng ký tự của chuỗi.

## Lưu ý

- Mỗi lần `OLED_UpdateScreen` truyền khoảng 1 KB qua I2C 100 kHz, ước tính khoảng 100 ms và làm vòng lặp chính bị chặn trong thời gian đó. Tăng I2C lên 400 kHz sẽ rút ngắn đáng kể.
- Không có tự xuống dòng hay tự cuộn: chữ vượt quá chiều rộng bị cắt.
- `OLED_WriteChar` ghi vào bộ đệm trực tiếp (chỉ bật pixel, không xóa nền), nên muốn vẽ lại phải `OLED_Clear()` trước.
