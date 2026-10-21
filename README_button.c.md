# button.c

Module xử lý **encoder xoay** và **nút bấm** của đồng hồ báo thức. Module chỉ đọc đầu vào rồi thay đổi các biến trạng thái dùng chung (khai báo trong `button.h`, định nghĩa trong `main.c`), sau đó gọi `updateDisplay()` để vẽ lại màn hình.

## Phụ thuộc

- `button.h` (chân phần cứng, biến và hàm dùng chung).
- Gọi sang `main.c`: `updateDisplay()`, `Clock_GetTime()`, `Clock_SetTime()`.
- Gọi sang `screen.c`: `Screen_ShowSaved()`.
- Dùng `HAL_GPIO_ReadPin`, `HAL_GPIO_WritePin`, `HAL_GetTick`.

## Biến nội bộ (static)

| Biến | Vai trò |
|---|---|
| `lastEncoderA` | Mức cuối cùng đọc được ở kênh A |
| `lastEncoderTime` | Thời điểm (ms) của lần xoay hợp lệ gần nhất, dùng để chống dội |
| `lastButtonState` | Trạng thái nút ở vòng quét trước (mặc định `GPIO_PIN_SET`) |
| `buttonPressTime` | Thời điểm bắt đầu nhấn nút |
| `longPressDone` | Cờ đã xử lý nhấn giữ, tránh xử lý lặp |

## Encoder

### `static int8_t readEncoder(void)`

Trả về `+1`, `-1` hoặc `0`.

1. Đọc kênh A, so với `lastEncoderA`. Không đổi thì trả về 0.
2. Chỉ xét **cạnh lên** của A (`GPIO_PIN_SET`).
3. Chống dội: bỏ qua nếu cách lần xoay hợp lệ trước chưa quá 3 ms.
4. Đọc kênh B để xác định chiều: B ở mức thấp → `+1`, ngược lại → `-1`.
5. Luôn cập nhật `lastEncoderA`.

### `void Encoder_Handle(void)`

Gọi `readEncoder()`, nếu có xoay thì xử lý theo màn hình hiện tại:

| Màn hình | Tác dụng khi xoay |
|---|---|
| `SCREEN_MENU` | Đổi `menuItem` trong 0 đến 2, quay vòng |
| `SCREEN_SET_TIME` | Tăng/giảm trường đang chọn: giờ 0 đến 23, phút và giây 0 đến 59, quay vòng |
| `SCREEN_SET_ALARM` | Trường 0/1: tăng/giảm giờ/phút báo thức (quay vòng). Trường 2: đảo bật/tắt (không phụ thuộc chiều xoay) |
| `SCREEN_CLOCK` | Không làm gì |

Mỗi lần thay đổi đều gọi `updateDisplay()`.

## Nút bấm

### `static void shortPress(void)` (nhấn ngắn)

Xử lý theo thứ tự ưu tiên:

1. **Chuông đang reo** (`alarmRinging`): tắt chuông (xóa cờ, kéo chân còi xuống thấp), về `SCREEN_CLOCK`.
2. `SCREEN_CLOCK` → vào `SCREEN_MENU` (mục 0).
3. `SCREEN_MENU`:
   - Mục 0 (Giờ thực): đọc giờ hiện tại từ RTC vào biến `edit...`, vào `SCREEN_SET_TIME`.
   - Mục 1 (Báo thức): nạp báo thức đã lưu vào biến `editAlarm...`, vào `SCREEN_SET_ALARM`.
   - Mục 2 (Thoát): về `SCREEN_CLOCK`.
4. `SCREEN_SET_TIME`: chuyển sang trường kế tiếp (giờ → phút → giây → giờ).
5. `SCREEN_SET_ALARM`: chuyển trường kế tiếp (giờ → phút → bật/tắt → giờ).

### `static void longPress(void)` (nhấn giữ)

| Màn hình | Tác dụng |
|---|---|
| `SCREEN_SET_TIME` | Ghi giờ đã chỉnh vào RTC (`Clock_SetTime`), hiện thông báo "ĐÃ LƯU", về menu (mục 0) |
| `SCREEN_SET_ALARM` | Lưu giờ/phút/trạng thái báo thức, xóa `alarmTriggered`, hiện "ĐÃ LƯU", về menu (mục 1) |
| `SCREEN_MENU` | Thoát về `SCREEN_CLOCK` |
| `SCREEN_CLOCK` | Không làm gì |

### `void Button_Handle(void)`

Phát hiện cạnh trên trạng thái nút (nhấn = `GPIO_PIN_RESET`):

- **Cạnh xuống**: ghi `buttonPressTime`, xóa `longPressDone`.
- **Đang giữ**: khi đủ `LONG_PRESS_TIME` (1500 ms), đặt `longPressDone = 1` và gọi `longPress()` ngay, không cần chờ thả nút.
- **Cạnh lên**: nếu thời gian nhấn lớn hơn 30 ms (lọc nhiễu) và nhỏ hơn `LONG_PRESS_TIME`, và chưa xử lý nhấn giữ, thì gọi `shortPress()`.
- Cuối hàm cập nhật `lastButtonState`.

### `void Button_Init(void)`

Đọc trạng thái ban đầu của kênh A và chân nút để các lần phát hiện cạnh đầu tiên không bị sai.

## Lưu ý

- Cả hai hàm quét (`Encoder_Handle`, `Button_Handle`) là polling, phải được gọi thường xuyên trong vòng lặp chính. Các hàm có `HAL_Delay` (như `Screen_ShowSaved`, chờ 500 ms) làm vòng lặp tạm dừng.
- Nhấn giữ khi chuông đang reo **không** tắt được chuông, vì `longPress()` không kiểm tra `alarmRinging`; chỉ nhấn ngắn mới tắt.
- Chiều xoay phụ thuộc cách nối dây A/B. Nếu thấy ngược chiều, đổi điều kiện `GPIO_PIN_RESET` ở bước đọc kênh B hoặc đổi dây A/B.
