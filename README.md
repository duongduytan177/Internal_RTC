# EC11E Encoder and Push Button Test - STM32

## 1. Giới thiệu
Dự án kiểm tra hoạt động của encoder xoay EC11E và nút nhấn tích hợp, sử dụng STM32 và thư viện STM32 HAL. Chương trình đọc hai pha A/B để phản hồi khi xoay encoder, đồng thời phân biệt thao tác nhấn ngắn và nhấn giữ.

## 2. Kết nối phần cứng

| Tín hiệu | Chân STM32 | Cấu hình |
|---|---|---|
| Encoder pha A | PB3 | GPIO Input, Pull-up |
| Encoder pha B | PB4 | GPIO Input, Pull-up |
| Nút nhấn encoder | PB5 | GPIO Input, Pull-up |
| LED kiểm tra | PC13 | GPIO Output Push-Pull |

Với cấu hình Pull-up, trạng thái mặc định của các đầu vào thường là mức cao (`GPIO_PIN_SET`). Khi công tắc nối chân tín hiệu xuống GND, đầu vào đọc được mức thấp (`GPIO_PIN_RESET`).

## 3. Kiểm tra xoay encoder

Hàm `readEncoder()` đọc trạng thái pha A và so sánh với `last_state_A` để phát hiện thay đổi trạng thái.

- Chương trình chỉ xử lý khi pha A chuyển lên mức cao (`GPIO_PIN_SET`).
- Sau khi phát hiện cạnh này, chương trình kiểm tra thời gian kể từ lần xử lý trước bằng `HAL_GetTick()`.
- Chỉ khi khoảng thời gian lớn hơn 3 ms, chương trình mới đọc pha B.
- Nếu pha B ở mức thấp (`GPIO_PIN_RESET`), PC13 được đặt mức cao.
- Nếu pha B ở mức cao (`GPIO_PIN_SET`), PC13 được đặt mức thấp.
- `last_state_A` được cập nhật để lần gọi tiếp theo có thể phát hiện thay đổi mới.

Trong code hiện tại, thao tác xoay encoder điều khiển trạng thái PC13 dựa trên mức pha B tại thời điểm pha A lên mức cao. Đây là logic kiểm tra tín hiệu, chưa phải bộ đếm số nấc xoay hoàn chỉnh.

## 4. Kiểm tra nút nhấn

Hàm `handleButton()` đọc trạng thái nút tại PB5 và nhận biết ba giai đoạn: bắt đầu nhấn, giữ nút và nhả nút.

### Nhấn bắt đầu
Khi trạng thái chuyển từ `GPIO_PIN_SET` sang `GPIO_PIN_RESET`, chương trình lưu thời điểm nhấn vào `time_nhan_nut` và đặt `done_nhan_dai = 0`.

### Nhấn giữ
Nếu nút vẫn ở mức thấp và thời gian giữ đạt `dem_nhan_dai` (1500 ms), chương trình đặt `done_nhan_dai = 1` rồi gọi `longPress()`.

Hàm `longPress()` đặt PC13 lên mức cao.

### Nhả nút
Khi nút chuyển từ mức thấp lên mức cao, chương trình tính thời gian nhấn:
- Nếu thời gian lớn hơn 30 ms và nhỏ hơn 1500 ms, đồng thời chưa xử lý nhấn giữ, chương trình gọi `shortPress()`.
- Hàm `shortPress()` đặt PC13 xuống mức thấp.

Việc kiểm tra thời gian nhấn giúp phân biệt nhấn ngắn với nhấn giữ. Trong code hiện tại, ngưỡng 30 ms là điều kiện chấp nhận nhấn ngắn; đây không phải bộ lọc dội tiếp điểm đầy đủ cho mọi loại nút.

## 5. Các biến chính

| Biến | Chức năng |
|---|---|
| `last_state_A` | Lưu trạng thái trước đó của pha A |
| `last_rotate_time` | Thời điểm xử lý encoder gần nhất |
| `lastButtonState` | Lưu trạng thái nút nhấn ở lần gọi trước |
| `time_nhan_nut` | Thời điểm bắt đầu nhấn nút |
| `done_nhan_dai` | Đánh dấu thao tác nhấn giữ đã được xử lý |
| `dem_nhan_dai` | Ngưỡng nhấn giữ, đặt bằng 1500 ms |

## 6. Luồng chương trình
1. Khởi tạo HAL và clock hệ thống.
2. Khởi tạo GPIO, đặt PC13 làm đầu ra và PB3/PB4/PB5 làm đầu vào Pull-up.
3. Đọc trạng thái ban đầu của pha A.
4. Trong vòng lặp chính, liên tục gọi `handleButton()` và `readEncoder()`.
5. Các thao tác xoay hoặc nhấn nút điều khiển trạng thái chân PC13 để kiểm tra phản hồi.

## 7. Công cụ phát triển
- STM32CubeIDE
- STM32CubeMX
- STM32 HAL Driver
- Ngôn ngữ lập trình C

## 8. Phạm vi thử nghiệm
- Kiểm tra đọc tín hiệu hai pha A/B của encoder.
- Quan sát phản hồi khi xoay encoder.
- Phân biệt nhấn ngắn và nhấn giữ nút encoder.
- Dùng PC13 làm đầu ra kiểm tra.

Lưu ý: chương trình hiện tại dùng chung PC13 cho phản hồi xoay encoder, nhấn ngắn và nhấn giữ. Do đó, trạng thái đầu ra cuối cùng phụ thuộc vào thao tác được xử lý sau cùng trong vòng lặp.
