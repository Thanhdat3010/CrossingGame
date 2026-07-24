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
    *   Người chơi điều khiển nhân vật tránh né chướng ngại vật di chuyển qua lại trên đường.
    *   Có cơ chế tự động tăng cấp độ khó (tăng tốc độ quái vật) và reset lại màn chơi khi bị va chạm hoặc hoàn thành màn chơi chiến dịch.
    *   Hỗ trợ **Chế độ chơi vô tận (Infinite Mode)** với cơ chế camera cuộn mượt mà theo nhân vật lên phía trên.
*   **Giao diện thực đơn menu lúc mới vào (Yêu cầu 4.2 - 1 điểm)**:
    *   Thiết kế Menu chính mang phong cách anime SAO với các tùy chọn: `NEW GAME`, `LOAD GAME`, `SETTINGS`.
    *   Hỗ trợ màn hình **Chọn Nhân Vật (Character Select)** giữa Kirito (Dual Wielder) và Asuna (Flash Rapier) với các chỉ số RPG khác nhau.
    *   Hỗ trợ màn hình **Chọn Chế Độ Chơi (Stage Select)**: Chiến dịch (Tutorial) hoặc Sinh tồn vô tận (Infinite).
*   **Giao diện đồ họa UI/UX (Yêu cầu 4.6 - 0.5 điểm)**:
    *   Sử dụng hình vẽ và kết cấu hình ảnh (textures) chi tiết cho các nhân vật, xe cộ, quái vật.
    *   Phông chữ dạng Pixel vẽ bằng code độc đáo.
    *   Sử dụng dải nền gradient bầu trời đêm anime cùng hiệu ứng sao nhấp nháy sinh động.

---

## 3. Những phần CHƯA LÀM ĐƯỢC / CẦN HOÀN THIỆN (Uncompleted & Future Work)

*   **Xử lý Lưu / Tải trò chơi (Yêu cầu 4.3 - 3 điểm)**:
    *   **Hiện trạng**: Chưa được triển khai. Menu chính khi chọn `LOAD GAME` hoặc nhấn phím khi đang chơi sẽ hiện cảnh báo *"NOT SUPPORTED YET!"*.
    *   **Hướng giải quyết đợt tiếp theo**: Cần bổ sung các phương thức `saveGame()` và `loadGame()` trong lớp `CGAME` để xuất/nhập trạng thái game (tọa độ người chơi, hướng đi, các đối tượng quái vật/xe cộ hiện tại trên màn hình) ra tệp tin văn bản hoặc nhị phân.
*   **Xử lý Tạm dừng phương tiện / Hệ thống Đèn giao thông (Yêu cầu 4.4 - 2 điểm)**:
    *   **Hiện trạng**: Các phương tiện và quái vật hiện tại di chuyển liên tục qua lại không ngừng nghỉ. Chưa có lớp `CTRAFFICLIGHT` hay cơ chế đèn tín hiệu xanh/đỏ để tạm dừng xe cộ.
    *   **Hướng giải quyết đợt tiếp theo**: Cần bổ sung lớp `CTRAFFICLIGHT` quản lý trạng thái đèn tín hiệu và liên kết vào luồng cập nhật của `CVEHICLE` để tạm dừng xe khi đèn đỏ.
*   **Kỹ thuật Đa tiểu trình (Mục 3.3)**:
    *   **Hiện trạng**: Phiên bản hiện tại sử dụng vòng lặp đơn tiểu trình regulated bằng delta-time (FPS cap ~60Hz) để tránh xung đột ngữ cảnh vẽ đồ họa của SDL3. Kỹ thuật chia luồng xử lý riêng biệt (`std::thread` chạy hàm cập nhật song song với luồng sự kiện chính) như trong hướng dẫn console chưa được triển khai.
    *   **Hướng giải quyết đợt tiếp theo**: Xem xét tách luồng xử lý vật lý/cập nhật vị trí sang một thread phụ và đồng bộ hóa dữ liệu với thread render chính để đúng với yêu cầu đa luồng của đồ án.
*   **Âm thanh & Hiệu ứng âm thanh khi va chạm (Yêu cầu 4.5 - 0.5 điểm)**:
    *   **Hiện trạng**: Hệ thống âm thanh `SDL_mixer` mới chỉ được khởi tạo trong `CGAME::init()` nhưng chưa thực hiện nạp tệp âm thanh (WAV/MP3) và chưa có âm thanh va chạm hay tiếng kêu của quái vật (`CANIMAL::Tell()`). Hiệu ứng va chạm hiện tại mới chỉ dừng lại ở hiệu ứng hiển thị đỏ màn hình "YOU DIED".
    *   **Hướng giải quyết đợt tiếp theo**: Cài đặt hàm `Tell()` phát ra âm thanh cho mỗi loài quái vật, nạp nhạc nền và phát hiệu ứng âm thanh nổ/va đập khi nhân vật bị tông trúng.

---

*Báo cáo này giúp bạn có cái nhìn tổng quan để hoàn thiện bản báo cáo tiến độ chi tiết gửi giảng viên chấm đồ án đợt 1.*
