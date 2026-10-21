# button.h

Header của module **nút bấm + encoder xoay**. File này làm hai việc: khai báo API của module (`button.c`) và là nơi chia sẻ chân phần cứng, biến, hàm dùng chung giữa `main.c` và `button.c`.

## Phụ thuộc

- Include: `main.h` (HAL), `screen.h` (kiểu `Screen_t`), `<stdint.h>`.
- Được include bởi: `button.c`, `main.c`.
- Include guard: `BUTTON_H`.

## Định nghĩa chân phần cứng

| Macro | Giá trị | Chức năng |
|---|---|---|
| `PORT_A`, `PIN_A` | `GPIOB`, `GPIO_PIN_3` | Encoder kênh A (CLK) |
| `PORT_B`, `PIN_B` | `GPIOB`, `GPIO_PIN_4` | Encoder kênh B (DT) |
| `PORT_SW`, `PIN_SW` | `GPIOB`, `GPIO_PIN_5` | Nút nhấn của encoder (SW), tích cực mức thấp |
| `PORT_COI`, `PIN_COI` | `GPIOA`, `GPIO_PIN_12` | Còi báo thức (mức cao = còi kêu) |

## Hằng số

| Macro | Giá trị | Ý nghĩa |
|---|---|---|
| `LONG_PRESS_TIME` | `1500` | Thời gian giữ nút (ms) để tính là nhấn giữ |

## Biến dùng chung (`extern`, định nghĩa trong `main.c`)

| Biến | Kiểu | Ý nghĩa |
|---|---|---|
| `screen` | `Screen_t` | Màn hình hiện tại của máy trạng thái |
| `editHour`, `editMinute`, `editSecond` | `int` | Giờ/phút/giây tạm khi chỉnh giờ thực |
| `editTimeField` | `int` | Trường đang chỉnh: 0 = giờ, 1 = phút, 2 = giây |
| `alarmHour`, `alarmMinute` | `int` | Giờ/phút báo thức đã lưu |
| `alarmEnable` | `uint8_t` | Bật/tắt báo thức |
| `editAlarmHour`, `editAlarmMinute` | `int` | Giờ/phút báo thức tạm khi chỉnh |
| `editAlarmEnable` | `uint8_t` | Bật/tắt tạm khi chỉnh |
| `editAlarmField` | `int` | Trường đang chỉnh: 0 = giờ, 1 = phút, 2 = bật/tắt |
| `alarmRinging` | `uint8_t` | Cờ chuông đang reo |
| `alarmTriggered` | `uint8_t` | Cờ đã kích hoạt trong phút báo thức hiện tại |
| `menuItem` | `int` | Mục menu đang chọn: 0 = Giờ thực, 1 = Báo thức, 2 = Thoát |

## Hàm dùng chung (định nghĩa trong `main.c`)

| Hàm | Mô tả |
|---|---|
| `void updateDisplay(void)` | Vẽ lại màn hình theo trạng thái hiện tại |
| `void Clock_GetTime(uint8_t *h, uint8_t *m, uint8_t *s)` | Đọc giờ từ RTC |
| `void Clock_SetTime(uint8_t h, uint8_t m, uint8_t s)` | Ghi giờ vào RTC |

## API của module (định nghĩa trong `button.c`)

| Hàm | Mô tả |
|---|---|
| `void Button_Init(void)` | Đọc trạng thái ban đầu của encoder và nút, gọi một lần trước vòng lặp |
| `void Button_Handle(void)` | Quét nút bấm, phân biệt nhấn ngắn / nhấn giữ, gọi trong `while(1)` |
| `void Encoder_Handle(void)` | Quét encoder xoay, gọi trong `while(1)` |

## Lưu ý

- Các macro chân ở đây phải **khớp với cấu hình trong `MX_GPIO_Init()`** của `main.c` (nơi các chân đang được ghi cứng: PA12 output, PB3/PB4/PB5 input pull-up). Muốn đổi chân thì sửa cả hai nơi (hoặc sửa trong CubeMX rồi cập nhật lại macro).
- `alarmStartMillis` (mốc bắt đầu nhịp còi) chỉ dùng trong `main.c` nên không được khai báo ở đây.
