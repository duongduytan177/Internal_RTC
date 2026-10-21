# README: Chương trình test RTC (`main_1.c`)

Chương trình nhỏ dùng để **kiểm tra RTC của STM32F103** hoạt động đúng trước khi ghép vào đồng hồ báo thức hoàn chỉnh. Một nút bấm đưa chương trình qua bốn bước: màn hình chào, ghi giờ vào RTC, thông báo đã ghi, rồi hiển thị giờ đọc từ RTC theo thời gian thực trên OLED.

## Mục đích

- Xác nhận RTC chạy từ nguồn LSI và đếm đúng 1 giây.
- Xác nhận hai hàm `Clock_SetTime()` và `Clock_GetTime()` ghi/đọc giờ đúng.
- Xác nhận OLED SH1106 hiển thị được số giờ (font scale 2).
- Đo sơ bộ độ lệch của RTC so với đồng hồ chuẩn.

## Phần cứng

| Thành phần | Kết nối |
|---|---|
| OLED SH1106 128x64 | I2C1: PB6 (SCL), PB7 (SDA), địa chỉ 0x3C |
| Nút nhấn | PB5, nối xuống GND (chân cấu hình input pull-up, nhấn = mức thấp) |
| Xung nhịp | HSI 8 MHz, RTC chạy bằng LSI (không dùng thạch anh ngoài) |

Ngoài ra `MX_GPIO_Init()` còn cấu hình PC13 là output (mức đầu thấp), nhưng chương trình test **không dùng** chân này.

## Các file cần có trong project

`main_1.c` (đổi tên thành `main.c` khi biên dịch), `oled.c/.h` (driver OLED). Chương trình test không dùng `button.c` hay `screen.c`.

## Cách chương trình hoạt động

Biến `cnt` là trạng thái. Mỗi lần nhấn nút (cạnh xuống trên PB5), `Button_Read()` trả về 1 và `cnt` tăng thêm 1.

| `cnt` | Màn hình OLED | Việc làm với RTC |
|---|---|---|
| 0 | "ALARM CLOCK" / "RTC STM32F103" | Không |
| 1 | `06:30:00` (giá trị đặt sẵn) | **Ghi liên tục** `06:30:00` vào RTC mỗi vòng lặp |
| 2 | "DA LUU" | Không ghi nữa, **RTC bắt đầu đếm** từ giá trị vừa ghi |
| 3 | Giờ đọc từ RTC, làm mới mỗi 200 ms | Đọc `Clock_GetTime()` |
| 4 trở lên | (không vẽ) | Đặt `cnt = 0`, quay về bước đầu |

Vòng chuyển trạng thái:

```
cnt=0 (chào) --nhấn--> cnt=1 (ghi 06:30:00) --nhấn--> cnt=2 (DA LUU)
     ^                                                      |
     |                                                    nhấn
     +------ nhấn (cnt=4 tự về 0) <---- cnt=3 (hiện giờ RTC) <--+
```

Giờ đặt sẵn lấy từ ba biến `alarmHour = 6`, `alarmMinute = 30`, `alarmSecond = 0`. Trong bài test này tên "alarm" chỉ là **giờ ban đầu ghi vào RTC**, chưa có chức năng báo thức nào.

## Các hàm

| Hàm | Chức năng |
|---|---|
| `Clock_SetTime(h, m, s)` | Ghi giờ vào RTC bằng `HAL_RTC_SetTime` (định dạng BIN); lỗi thì vào `Error_Handler()` |
| `Clock_GetTime(*h, *m, *s)` | Đọc giờ bằng `HAL_RTC_GetTime`; chỉ ghi kết quả nếu đọc thành công |
| `Button_Read()` | Phát hiện cạnh xuống của PB5, trả về 1 khi vừa nhấn (không có chống dội) |

## Cấu hình RTC (CubeMX)

| Mục | Giá trị |
|---|---|
| Nguồn xung RTC | LSI (`RCC_RTCCLKSOURCE_LSI`), LSI bật |
| `AsynchPrediv` | `RTC_AUTO_1_SECOND` (HAL tự tính để đếm 1 lần/giây) |
| Ngõ ra RTC | `RTC_OUTPUTSOURCE_ALARM` (chương trình không dùng) |
| Giờ ban đầu | 00:00:00, ngày 01/01 (thứ Hai), đặt trong `MX_RTC_Init()` |

## Quy trình test

1. Nạp chương trình, cấp nguồn. OLED hiện **ALARM CLOCK / RTC STM32F103**.
2. Nhấn nút (giữ khoảng nửa giây cho chắc): OLED hiện **06:30:00** đứng yên. Lúc này RTC đang bị ghi liên tục nên giá trị không chạy.
3. Nhấn nút lần nữa: OLED hiện **DA LUU**. RTC bắt đầu đếm từ lúc này.
4. Nhấn nút lần nữa: OLED hiện giờ đọc từ RTC dạng `HH:MM:SS`.
5. Quan sát: số giây phải tăng **một đơn vị mỗi giây**, từ giá trị lớn hơn `06:30:00` một chút (bằng thời gian bạn đã ở bước 3).
6. Nhấn tiếp để quay về màn hình chào và lặp lại nếu cần.

## Kết quả mong đợi

- Giây tăng đều, không đứng, không nhảy cóc.
- Hết 59 giây thì phút tăng 1, giây về 00 (ví dụ `06:30:59` sang `06:31:00`).
- Khi đứng ở bước 3 càng lâu thì giờ hiển thị ở bước 4 càng lớn hơn `06:30:00`.

## Đo độ lệch của RTC

1. Ở bước 4, bấm đồng hồ bấm giây (hoặc ghi lại giờ thực) đúng lúc OLED chuyển sang hiển thị giờ.
2. Để chạy 10 phút trở lên (càng lâu càng chính xác).
3. Ghi lại giờ trên OLED và thời gian thực.

| Thời gian thực (s) | Thời gian RTC hiển thị (s) | Lệch (s) | Lệch (%) |
|---|---|---|---|
| | | | |

Công thức:

```
Lệch (%)        = (T_RTC - T_thực) / T_thực x 100
Lệch mỗi ngày   = (T_RTC - T_thực) / T_thực x 86400   (giây/ngày)
```

Vì RTC dùng LSI (RC nội, theo datasheet dòng F1 nằm trong khoảng 30 đến 60 kHz, danh định 40 kHz) và HAL tính hệ số chia theo 40 kHz, **độ lệch có thể rõ rệt**, lớn hơn nhiều so với dùng thạch anh LSE 32,768 kHz. Đây là điều cần ghi nhận trong báo cáo, không phải lỗi chương trình.

## Test mở rộng: tràn giờ và tràn phút

Sửa ba biến ở đầu file rồi nạp lại:

| Mục đích | `alarmHour` | `alarmMinute` | `alarmSecond` |
|---|---|---|---|
| Kiểm tra tràn phút | 6 | 30 | 55 |
| Kiểm tra tràn giờ | 6 | 59 | 55 |
| Kiểm tra qua nửa đêm | 23 | 59 | 55 |

Sau khi vào bước 4, chờ vài giây và kiểm tra giờ chuyển đúng (ví dụ `23:59:59` sang `00:00:00`).

## Lỗi thường gặp

| Hiện tượng | Nguyên nhân có thể | Cách xử lý |
|---|---|---|
| OLED trống hoàn toàn | Sai dây SDA/SCL, sai địa chỉ, OLED chưa kịp khởi động | Kiểm tra PB6/PB7 và địa chỉ 0x3C; thêm `HAL_Delay(300)` **trước** `OLED_Init()` (chương trình chính làm theo thứ tự này) |
| Chương trình treo ngay khi bật, không hiện gì | Một hàm HAL của RTC/I2C/clock báo lỗi nên rơi vào `Error_Handler()` | Chạy debug, đặt breakpoint ở `Error_Handler()` và xem hàm nào gọi tới |
| Giờ đứng yên ở `06:30:00` | Đang ở bước 1, RTC bị ghi liên tục | Nhấn tiếp sang bước 2 rồi 3 |
| Giờ không chạy ở bước 3 | RTC không đếm: kiểm tra LSI đã bật và nguồn xung RTC là LSI | Xem `SystemClock_Config()` |
| Nhấn một lần mà nhảy hai bước | Nút bị dội, `Button_Read()` không có chống dội | Thêm tụ nhỏ cho nút hoặc thêm trễ ngắn sau khi nhận nhấn |
| Nhấn mà không có phản ứng | Vòng lặp đang bị chặn bởi `HAL_Delay(200)` ở bước 0 hoặc `OLED_UpdateScreen` (khoảng 100 ms), nhấn quá nhanh nên bị bỏ lỡ | Giữ nút lâu hơn khoảng nửa giây |

## Hạn chế của bài test và khác biệt với chương trình chính

- Bước 1 ghi RTC **liên tục** mỗi vòng lặp; bài test chỉ cần một lần ghi, nhưng cách này đơn giản và vẫn cho kết quả đúng.
- Chưa test việc giữ giờ khi reset, vì `MX_RTC_Init()` luôn đặt lại 00:00:00 mỗi lần khởi động và bước 1 luôn ghi đè lại giờ.
- Chưa có encoder, còi, menu, báo thức; `PC13` cấu hình nhưng không dùng.
- `textBuffer` và việc vẽ màn hình nằm ngay trong `main.c`; ở chương trình chính, phần này được tách sang `screen.c`.
- `Clock_SetTime()` và `Clock_GetTime()` giống hệt bản trong chương trình chính, nên kết quả test này áp dụng trực tiếp cho bản đầy đủ.
