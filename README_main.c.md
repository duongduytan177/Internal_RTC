# main.c

Điểm vào của chương trình **đồng hồ báo thức STM32F103** (RTC, OLED SH1106, encoder + nút bấm, còi). File sinh từ STM32CubeMX (thư viện HAL), phần do người dùng viết nằm trong các vùng `USER CODE BEGIN ... END`.

> Code nằm ngoài các vùng `USER CODE` sẽ bị CubeMX ghi đè khi sinh lại project.

## Nhiệm vụ

1. Khởi tạo HAL, xung nhịp và ngoại vi (GPIO, I2C1, RTC).
2. Khai báo các biến toàn cục dùng chung với `button.c`.
3. Đọc/ghi giờ RTC (`Clock_GetTime`, `Clock_SetTime`).
4. Vẽ lại màn hình theo trạng thái (`updateDisplay`).
5. Kiểm tra giờ báo thức (`checkAlarm`) và tạo nhịp còi (`updateBuzzer`).
6. Chạy vòng lặp chính gọi lần lượt các module.

## Quan hệ với các file khác

| File | Quan hệ |
|---|---|
| `button.c/.h` | Được main gọi `Button_Init`, `Button_Handle`, `Encoder_Handle`; dùng lại biến và hàm của main qua `extern` |
| `screen.c/.h` | `updateDisplay()` và `checkAlarm()` gọi các hàm `Screen_...` |
| `oled.c/.h` | main gọi `OLED_Init()`; `oled.c` dùng `hi2c1` của main qua `extern` |

## Sơ đồ chân

| Chân | Chế độ | Chức năng |
|---|---|---|
| PA12 | Output push-pull | Còi (mức cao = kêu) |
| PB3 | Input pull-up | Encoder kênh A |
| PB4 | Input pull-up | Encoder kênh B |
| PB5 | Input pull-up | Nút nhấn encoder (nhấn = mức thấp) |
| PB6 / PB7 | I2C1 SCL / SDA | OLED (theo ghi chú trong `oled.h`) |

## Biến toàn cục

| Biến | Giá trị đầu | Ý nghĩa |
|---|---|---|
| `screen` | `SCREEN_CLOCK` | Màn hình hiện tại |
| `editHour`, `editMinute`, `editSecond` | 0 | Giờ tạm khi chỉnh giờ thực |
| `editTimeField` | 0 | Trường đang chỉnh (0 giờ, 1 phút, 2 giây) |
| `alarmHour`, `alarmMinute` | 6, 30 | Giờ báo thức đã lưu (mặc định 06:30) |
| `alarmEnable` | 1 | Báo thức đang bật |
| `editAlarmHour`, `editAlarmMinute`, `editAlarmEnable` | 6, 30, 1 | Giá trị báo thức tạm khi chỉnh |
| `editAlarmField` | 0 | Trường đang chỉnh (0 giờ, 1 phút, 2 bật/tắt) |
| `alarmRinging` | 0 | Chuông đang reo |
| `alarmTriggered` | 0 | Đã kích hoạt trong phút báo thức này (chống kích hoạt lặp) |
| `alarmStartMillis` | 0 | Mốc `HAL_GetTick()` lúc chuông bắt đầu reo |
| `menuItem` | 0 | Mục menu đang chọn |
| `lastDisplayUpdate` | 0 | Mốc lần vẽ lại đồng hồ gần nhất |
| `hi2c1`, `hrtc` | — | Handle I2C1 và RTC (CubeMX sinh) |

Các biến `edit...` là bản tạm: chỉ khi nhấn giữ để lưu thì mới ghi vào RTC hoặc biến báo thức chính.

## Hàm do người dùng viết

| Hàm | Chức năng |
|---|---|
| `Clock_SetTime(h, m, s)` | Ghi giờ vào RTC bằng `HAL_RTC_SetTime` (định dạng BIN); lỗi thì vào `Error_Handler()` |
| `Clock_GetTime(*h, *m, *s)` | Đọc giờ từ RTC bằng `HAL_RTC_GetTime`; chỉ ghi kết quả nếu đọc thành công |
| `updateDisplay()` | Nếu `alarmRinging` thì vẽ màn hình chuông reo; ngược lại vẽ theo `screen` (đồng hồ, menu, chỉnh giờ, chỉnh báo thức) |
| `checkAlarm()` | Kiểm tra đến giờ báo thức và kích hoạt chuông |
| `updateBuzzer()` | Điều khiển chân còi theo nhịp |

### `checkAlarm()`

1. Báo thức tắt → xóa `alarmTriggered`, thoát.
2. Đọc giờ RTC; nếu giờ hoặc phút khác giờ báo thức → xóa `alarmTriggered`.
3. Nếu giờ và phút trùng, giây `<= 2` và chưa kích hoạt: đặt `alarmTriggered = 1`, `alarmRinging = 1`, ghi `alarmStartMillis`, về `SCREEN_CLOCK`, vẽ màn hình chuông reo.

Khoảng dung sai 3 giây tránh bỏ lỡ báo thức khi vòng lặp bị chặn bởi `HAL_Delay`. Chuông chỉ tắt khi nhấn ngắn nút (xử lý trong `button.c`).

### `updateBuzzer()`

Khi chuông reo, tính `t = (HAL_GetTick() - alarmStartMillis) % 1500`:

| `t` (ms) | Còi |
|---|---|
| 0 đến 150 | Bật |
| 150 đến 250 | Tắt |
| 250 đến 400 | Bật |
| 400 đến 500 | Tắt |
| 500 đến 650 | Bật |
| 650 đến 1500 | Tắt |

Tức ba tiếng bíp rồi nghỉ dài, lặp lại. Không dùng `HAL_Delay` nên vòng lặp vẫn quét được nút. Khi không reo, chân còi được giữ ở mức thấp.

## Hàm `main()`

**Khởi tạo:** `HAL_Init()` → `SystemClock_Config()` → `MX_GPIO_Init()` → `MX_I2C1_Init()` → `MX_RTC_Init()` → `HAL_Delay(300)` (chờ OLED ổn định) → `OLED_Init()` → `Button_Init()` → tắt còi → `Screen_ShowWelcome()` → `screen = SCREEN_CLOCK` → `updateDisplay()`.

**Vòng lặp `while(1)`** (polling, không dùng ngắt):

1. `Encoder_Handle()`
2. `Button_Handle()`
3. `checkAlarm()`
4. `updateBuzzer()`
5. Nếu đang ở `SCREEN_CLOCK` và chuông không reo: cứ mỗi 200 ms đọc RTC và vẽ lại đồng hồ.
6. `HAL_Delay(1)`

Các màn hình menu và chỉnh giá trị chỉ được vẽ lại khi có thao tác, không vẽ định kỳ.

## Cấu hình CubeMX (các hàm `MX_...`)

| Hàm | Cấu hình |
|---|---|
| `SystemClock_Config` | HSI 8 MHz và LSI bật, không dùng PLL; SYSCLK = HCLK = PCLK1 = PCLK2 = 8 MHz; `FLASH_LATENCY_0`; nguồn xung RTC là LSI |
| `MX_I2C1_Init` | I2C1, 100 kHz, địa chỉ 7 bit, tắt dual address và general call, cho phép clock stretching |
| `MX_RTC_Init` | `RTC_AUTO_1_SECOND`, ngõ ra RTC là Alarm (chương trình không dùng); đặt giờ ban đầu 00:00:00, ngày 01/01 (thứ Hai) |
| `MX_GPIO_Init` | Bật clock GPIOA, GPIOB; PA12 output push-pull mức đầu thấp; PB3, PB4, PB5 input pull-up |

## Lưu ý

- `MX_RTC_Init()` luôn đặt giờ về 00:00:00, và vùng `Check_RTC_BKUP` đang để trống, nên **giờ bị đặt lại sau mỗi lần reset**. Muốn giữ giờ cần lưu cờ vào thanh ghi backup và bỏ qua bước đặt giờ nếu cờ đã có.
- RTC dùng LSI (RC nội) nên độ chính xác kém hơn thạch anh 32,768 kHz (LSE).
- PB3 và PB4 mặc định là chân JTAG; để dùng làm GPIO cho encoder cần chọn Debug = Serial Wire trong CubeMX (System Core → SYS).
- Báo thức được so sánh bằng phần mềm trong `checkAlarm()`, chưa dùng ngắt báo thức của RTC.
- Chuông không tự tắt; chỉ tắt khi nhấn ngắn nút.
- `#include <stdio.h>` trong file hiện chưa được dùng trực tiếp (`sprintf` nằm trong `screen.c`).
