# Kế hoạch dự án CrossingGame (cập nhật theo CrossingGame.md)

Ngày cập nhật: 2026-06-12 (Sau khi hoàn tất Phase 1 và Tái cấu trúc OOP)

## 0) Nguồn yêu cầu
- Tài liệu: CrossingGame.md (Khoa CNTT - OOP, 01/10/2025).
- Yêu cầu riêng của bạn: game 2D pixel, thư viện giao diện có file .DLL, theme giao diện màu sáng.
- Quy định nộp bài 3 lần và cấu trúc file nộp (txt chứa link drive).

## 1) Yêu cầu bắt buộc từ tài liệu
- Kịch bản: người chơi điều khiển nhân vật (mặc định ký tự Y) bằng W/A/S/D để vượt đường.
- Va chạm xe/thú: thông báo, nhấn Y để chơi tiếp (reset), phím bất kỳ để thoát.
- Qua hết làn xe/thú: lên cấp, tăng độ khó (tăng số xe/thú), đặt lại vị trí.
- Menu ban đầu: New Game / Load Game / Settings (ban đầu chỉ xử lý New Game, các mục khác yêu cầu chọn lại).
- Lưu/tải game:
  - Load: chọn từ menu hoặc nhấn T khi đang chơi; tạm dừng và yêu cầu nhập đường dẫn file.
  - Save: nhấn L khi đang chơi; tạm dừng và yêu cầu nhập đường dẫn file, lưu xong chơi tiếp (có thể hỏi tiếp tục).
- Tạm dừng xe: xe có thể dừng trong khoảng thời gian ngẫu nhiên (gợi ý: lớp CTRAFFICLIGHT).
- Hiệu ứng va chạm + âm thanh thú (đơn giản).
- Giao diện: bố trí thông tin hợp lý, có hình vẽ xe/thú/người/đèn tín hiệu.

## 2) Ràng buộc riêng của bạn
- Game 2D pixel, thư viện GUI có .DLL.
- Theme/tông màu sáng (không dùng theme tối).
- Toàn bộ code nằm ở master; khi đủ điều kiện nộp lần 1/2/3 thì tôi sẽ báo để bạn tạo branch nộp.
- **Quy tắc OOP nghiêm ngặt**: Tên tệp nguồn trùng khớp hoàn toàn với tên Class tương ứng (Phân biệt hoa/thường). Một tệp `.h` và một tệp `.cpp` riêng biệt cho mỗi lớp.

## 3) Quyết định kỹ thuật (đã chốt)
- Giáo viên chấp nhận game GUI 2D pixel.
- Chọn SDL3 và build bằng Visual Studio (MSBuild), có .DLL.
- Giữ logic theo tài liệu, giao diện tổng thể màu sáng.
- Thiết lập thư mục mã nguồn:
  - Đầu vào chính: `src/MainProg.cpp`.
  - Các lớp tiêu đề (.h): `src/include/`.
  - Các lớp thực thi (.cpp): `src/source/`.

## 4) Mục tiêu nộp bài (đặt cao hơn yêu cầu)
### Lần 1 (mục tiêu: menu + 1 màn cơ bản chơi được) -> **ĐÃ HOÀN THÀNH**
- Menu hoàn chỉnh (New Game hoạt động, Load/Settings báo chưa hỗ trợ).
- Game loop + input + va chạm + win/lose.
- 1 màn có xe + thú, có reset khi va chạm, có lên cấp 1 và chế độ Infinite.
- Tái cấu trúc OOP hoàn chỉnh: chia nhỏ các lớp `CBLUEWING`, `CSKYARMOR`, `CILLFANG`, `CICEDRAGON`, `CHEATHCLIFF`, `CGLEAMEYES` thành các tệp riêng tương ứng.
- Report: kế hoạch, nghiên cứu, kiến trúc, danh sách asset.
- Tiêu chí đạt: chơi được 1 màn từ đầu đến cuối, không crash, cấu trúc thư mục sạch đẹp.

### Lần 2 (mục tiêu: chơi được đầy đủ) -> **ĐANG THỰC HIỆN**
- Lưu/Tải game đúng phím L/T theo yêu cầu.
- Tạm dừng xe theo thời gian (traffic light/logic dừng khoảng).
- HUD/thông tin hợp lý, thêm hiệu ứng va chạm, âm thanh thú.
- Slide thuyết trình + report cập nhật tiến độ.
- Tiêu chí đạt: có ít nhất 2 cấp độ khó, hoạt động ổn định.

### Lần 3 (mục tiêu: hoàn thiện)
- Hoàn chỉnh tất cả chức năng + polish.
- Report + slide cuối + src.zip + video YouTube.
- Tiêu chí đạt: đóng gói đầy đủ DLL, chạy trên máy khác.

## 5) Work breakdown theo mục tiêu (có kiểm tra)
1. Chốt hướng kỹ thuật và cấu trúc project -> kiểm tra: thống nhất công nghệ. **(Xong)**
2. Định nghĩa rule & luồng game -> kiểm tra: có spec ngắn. **(Xong)**
3. Cài đặt nền (loop, input, render, state menu/play/pause) -> kiểm tra: vào được menu và thoát. **(Xong)**
4. Hệ thống đối tượng (CPEOPLE, CVEHICLE, CANIMAL, CGAME cùng các lớp con) -> kiểm tra: di chuyển + va chạm + cấu trúc file OOP. **(Xong)**
5. Save/Load -> kiểm tra: lưu file và tải lại đúng trạng thái. (Chưa thực hiện)
6. Tạm dừng xe -> kiểm tra: xe dừng lại theo khoảng thời gian. (Chưa thực hiện)
7. UI/FX -> kiểm tra: có HUD + hiệu ứng va chạm + âm thanh thú. (Đã có giao diện, thiếu âm thanh)
8. Đóng gói -> kiểm tra: src.zip chạy trên máy khác. (Đã tạo file zip thử nghiệm trên branch `dot-1`)

## 6) Quy trình nộp bài
- Mỗi lần nộp: 1 file .txt chứa link Google Drive/OneDrive.
- Drive gồm: report.pdf, slide.pptx, src.zip (code + DLL).
- Lần 3: thêm link YouTube trong file .txt.

## 7) Quy tắc branch
- Làm việc trên master.
- Khi tôi báo “đủ điều kiện nộp lần N”, bạn tạo branch nộp lần N, đóng gói, sau đó tiếp tục trên master.
