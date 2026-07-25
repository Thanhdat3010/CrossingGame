# BÁO CÁO TIẾN ĐỘ DO ÁN: TRÒ CHƠI BĂNG QUA ĐƯỜNG (CROSSING GAME)

Dưới đây là tổng hợp chi tiết những hạng mục **Đã hoàn thành (Done)** và **Chưa hoàn thành / Cần cải thiện (Todo)** để phục vụ việc viết báo cáo tiến độ đợt 1.

---

## 1. Các điểm cải tiến về mặt Kiến trúc hệ thống (So với tài liệu gốc)
*   **Chuyển đổi từ Console sang SDL3 (Đồ họa 2D hiện đại)**: Dự án đã chuyển đổi hoàn toàn từ giao diện Console văn bản (sử dụng các kỹ thuật như `FixConsoleWindow`, `GotoXY`) sang thư viện đồ họa **SDL3**. Điều này mang lại hiệu năng vẽ cực cao, hỗ trợ ảnh PNG chất lượng và khả năng mở rộng tốt hơn.
*   **Tuân thủ nghiêm ngặt chuẩn OOP**: Codebase đã được tái cấu trúc thành công. Mỗi lớp đối tượng (Class) đều có một cặp tệp nguồn tương ứng trùng tên lớp:
    *   **Header (.h)** nằm trong thư mục [src/include/](file:///d:/Code/CrossingGame/src/include/)
    *   **Implementation (.cpp)** nằm trong thư mục [src/source/](file:///d:/Code/CrossingGame/src/source/)

---

## 2. Những phần ĐÃ LÀM ĐƯỢC (Completed)

### 2.1 Cài đặt các Lớp đối tượng cốt lõi (Mục 3.4)
*   **Lớp `CPEOPLE`**: 
    *   Quản lý tọa độ `mX`, `mY`, trạng thái sống chết `mState`.
    *   Hỗ trợ di chuyển 4 hướng thông qua `Up()`, `Down()`, `Left()`, `Right()`.
    *   Kiểm tra va chạm AABB bằng `isImpact()` và kiểm tra trạng thái về đích `isFinish()`.
*   **Hệ thống `CVEHICLE` (Phương tiện)**:
    *   Lớp cha `CVEHICLE` định nghĩa phương thức ảo `Move()`.
    *   Hai lớp con thực tế được xây dựng là `CBLUEWING` và `CSKYARMOR` (thay thế cho `CTRUCK` và `CCAR` để phù hợp với chủ đề Sword Art Online).
*   **Hệ thống `CANIMAL` (Quái vật)**:
    *   Lớp cha `CANIMAL` định nghĩa phương thức ảo `Move()`.
    *   Bốn lớp con quái vật đại diện cho Boss trong SAO được phát triển là: `CILLFANG`, `CICEDRAGON`, `CHEATHCLIFF`, `CGLEAMEYES` (thay thế cho `CBIRD` và `CDINAUSOR`).
*   **Lớp quản lý `CGAME`**:
    *   Đóng vai trò điều phối trung tâm.
    *   Quản lý vòng lặp game chính (`run()`), xử lý sự kiện người dùng (`handleInput()`), cập nhật trạng thái (`update()`) và kết xuất đồ họa (`render()`).

### 2.2 Các tính năng Yêu cầu Đồ án (Mục 4)
*   **Cài đặt chạy được giống kịch bản mô tả (Yêu cầu 4.1 - 3 điểm)**:
    *   Người chơi điều khiển nhân vật tránh né chướng ngại vật di chuyển qua lại trên đường (`W/A/S/D` hoặc phím mũi tên).
    *   Cơ chế chống đè phím (Single-Tap Input): Bắt buộc bấm nhả từng phím để nhảy từng bước.
    *   Có cơ chế tự động tăng cấp độ khó và reset lại màn chơi khi bị va chạm hoặc hoàn thành màn chơi chiến dịch.
    *   Hỗ trợ **Chế độ chơi vô tận (Infinite Mode)** tính điểm chuẩn xác (+1 điểm/làn vượt qua) với cơ chế camera cuộn mượt mà theo nhân vật.
*   **Giao diện thực đơn menu lúc mới vào (Yêu cầu 4.2 - 1 điểm)**:
    *   Thiết kế Menu phong cách **Light Mode SAO Aincrad (Frosted Glass)** với các tùy chọn: `NEW GAME`, `LOAD GAME`, `SETTINGS`.
    *   Hỗ trợ màn hình **Chọn Nhân Vật (Character Select)** giữa Kirito và Asuna.
    *   Hỗ trợ màn hình **Chọn Chế Độ Chơi (Stage Select)**: Chiến dịch (Tutorial) hoặc Sinh tồn vô tận (Infinite).
    *   Hỗ trợ màn hình **Cài Đặt (Settings Menu)**: Cho phép Tắt/Bật Nhạc nền (`MUSIC BGM`) và Tắt/Bật Âm thanh (`SOUND SFX`) riêng biệt.
*   **Hệ thống Đèn giao thông tạm dừng xe cộ (Yêu cầu 4.4 - 2 điểm)**:
    *   Xây dựng lớp `CTRAFFICLIGHT` với bộ đếm 3s Đỏ / 5s Xanh.
    *   Xe cộ dừng di chuyển khi gặp đèn đỏ. Cột đèn vẽ Pixel Art phát sáng Neon ở 2 bên vỉa hè.
*   **Hiệu ứng âm thanh & va chạm (Yêu cầu 4.5 - 0.5 điểm)**:
    *   Màn hình chớp mờ đỏ khi va chạm (`YOU DIED`).
    *   Phát hiệu ứng âm thanh va chạm `sfx_hit.mp3` và tiếng nhảy `sfx_jump.wav`/`.mp3` nhịp nhàng khi nhân vật di chuyển.
    *   Phát nhạc nền `bgm_menu.mp3` mượt mà xuyên suốt màn chơi.
*   **Kỹ thuật Đa tiêu trình (Mục 3.3)**:
    *   Sử dụng `std::thread mPhysicsThread` chạy luồng vật lý & kiểm tra va chạm tách biệt 100% với luồng vẽ giao diện UI.
    *   Sử dụng `std::mutex mGameMutex` đồng bộ dữ liệu Thread-safe an toàn 100%.
*   **Giao diện đồ họa UI/UX (Yêu cầu 4.6 - 0.5 điểm)**:
    *   Sử dụng hình vẽ kết cấu (textures) chi tiết cho các nhân vật Kirito/Asuna, 4 loại quái Boss SAO, 2 loại xe và Đèn giao thông.
    *   Hỗ trợ ảnh nền tự định nghĩa `bg_menu.png` giao diện sáng SAO Aincrad.

---

## 3. Những phần CHƯA LÀM ĐƯỢC / CẦN HOÀN THIỆN (Uncompleted & Future Work)

*   **Xử lý Lưu / Tải trò chơi (Yêu cầu 4.3 - 3 điểm)**:
    *   **Hiện trạng**: Chưa được triển khai. Phím `L` (Save), `T` (Load) và Menu `LOAD GAME` hiện đang cảnh báo *"NOT SUPPORTED YET!"*.
    *   **Hướng giải quyết đợt tiếp theo**: Cần bổ sung các phương thức `saveGame()` và `loadGame()` trong lớp `CGAME` để xuất/nhập trạng thái game (tọa độ người chơi, hướng đi, các đối tượng quái vật/xe cộ hiện tại trên màn hình) ra tệp tin văn bản trong thư mục `saves/`.

---

*Báo cáo đã được cập nhật đầy đủ thông tin tính năng mới nhất.*
