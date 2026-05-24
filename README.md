# ⚔️ Crossing Game - Sword Art Online Edition ⚔️

Chào mừng bạn đến với dự án **Crossing Game** (Trò chơi băng qua đường) phiên bản đặc biệt lấy cảm hứng từ thế giới **Sword Art Online (SAO)**. Đây là đồ án môn học **Lập trình hướng đối tượng (OOP)** được xây dựng bằng ngôn ngữ C++ và thư viện đồ họa **SDL3**.

---

## 🚀 Hướng Dẫn Biên Dịch Và Chạy Game (1 Click Duy Nhất)

Dự án đã được cấu hình tối ưu để người chấm điểm (Thầy giáo) hoặc người chơi có thể biên dịch và khởi chạy game **ngay lập tức** chỉ bằng một thao tác duy nhất trên Windows:

### Cách 1: Chạy bằng file Batch (Khuyên Dùng)
1. Mở thư mục gốc của dự án `CrossingGame`.
2. Nhấp đúp chuột (Double click) vào file **`compile.bat`**.
3. Hệ thống sẽ tự động tìm kiếm bộ cài đặt Visual Studio (CMake/MSBuild), tự động cấu hình, biên dịch mã nguồn, copy các file thư viện `.dll` cần thiết và khởi chạy game ngay lập tức!

### Cách 2: Chạy trong PowerShell
1. Mở PowerShell trong thư mục dự án.
2. Gõ lệnh duy nhất sau và nhấn Enter:
   ```powershell
   .\run.ps1
   ```

---

## 🗂️ Sơ Đồ Cấu Trúc Thư Mục & Tra Cứu Tính Năng

Để thuận tiện cho việc báo cáo và giải trình với Thầy giáo khi được hỏi về các tính năng, cấu trúc thư mục của dự án đã được phân chia vô cùng sạch sẽ, khoa học theo mô hình chuẩn của một dự án C++ chuyên nghiệp:

```text
CrossingGame/
├── compile.bat                  # File chạy nhanh (Double-click để biên dịch + chạy game)
├── run.ps1                      # Script tự động tìm kiếm bộ biên dịch và khởi chạy
├── CMakeLists.txt               # File cấu hình build dự án bằng CMake
├── assets/                      # Thư mục chứa tài nguyên hình ảnh (PNG, JPG,...)
│   └── swords.png               # Ảnh 2 thanh kiếm Elucidator & Lambent Light của SAO ở Menu chính
├── extern/                      # Thư mục chứa thư viện liên kết ngoài
│   └── sdl3/                    # Các file Header và thư viện tĩnh (.lib, .dll) của SDL3, Image, Mixer
├── include/                     # Nơi chứa các file khai báo lớp (Header Files - *.h)
│   ├── Game.h                   # Khai báo lớp quản lý vòng lặp game chính (CGAME)
│   ├── Font.h                   # Khai báo hệ thống vẽ chữ Pixel Art (CFont)
│   ├── People.h                 # Khai báo lớp nhân vật người chơi (CPEOPLE)
│   ├── Vehicle.h                # Khai báo lớp xe cộ/quái vật trừu tượng (CVEHICLE, CTRUCK, CCAR)
│   └── Animal.h                 # Khai báo lớp động vật trừu tượng (CANIMAL, CDINAUSOR, CBIRD)
└── src/                         # Nơi chứa các file triển khai logic code (Source Files - *.cpp)
    ├── main.cpp                 # Điểm khởi đầu của chương trình (Entry Point)
    ├── Game.cpp                 # Quản lý game loop, chuyển màn (Stage), va chạm và Game Over
    ├── Font.cpp                 # Vẽ chữ pixel từ mảng nhị phân 100% bằng code thuần
    ├── People.cpp               # Xử lý di chuyển W/A/S/D của nhân vật và va chạm hộp biên (AABB)
    ├── Vehicle.cpp              # Xử lý hoạt ảnh vẽ xe cộ (Truck, Car) theo hướng di chuyển
    └── Animal.cpp               # Xử lý hoạt ảnh Khủng long chạy bộ, Chim vỗ cánh nhấp nhô
```

---

## 📌 Tra Cứu Nhanh Các Tính Năng (Khi Thầy Hỏi Code Ở Đâu)

| Tính năng chính | File Header cần mở | File Source cần mở | Tên hàm / Thuộc tính tương ứng |
|---|---|---|---|
| **Khởi tạo Game & Cửa sổ** | [Game.h](file:///d:/Code/CrossingGame/include/Game.h) | [Game.cpp](file:///d:/Code/CrossingGame/src/Game.cpp) | Hàm `CGAME::init` |
| **Vòng lặp chính (Game Loop)** | [Game.h](file:///d:/Code/CrossingGame/include/Game.h) | [Game.cpp](file:///d:/Code/CrossingGame/src/Game.cpp) | Vòng lặp `while(mIsRunning)` trong hàm `CGAME::run` |
| **Bắt sự kiện bàn phím** | [Game.h](file:///d:/Code/CrossingGame/include/Game.h) | [Game.cpp](file:///d:/Code/CrossingGame/src/Game.cpp) | Hàm `CGAME::handleInput` |
| **Tính toán logic game** | [Game.h](file:///d:/Code/CrossingGame/include/Game.h) | [Game.cpp](file:///d:/Code/CrossingGame/src/Game.cpp) | Hàm `CGAME::update` (di chuyển quái vật, check va chạm) |
| **Vẽ giao diện Menu SAO** | [Game.h](file:///d:/Code/CrossingGame/include/Game.h) | [Game.cpp](file:///d:/Code/CrossingGame/src/Game.cpp) | Hàm `CGAME::renderMenu` (vẽ nền gradient, sao nhấp nháy, kiếm chéo) |
| **Vẽ màn chơi & chướng ngại vật** | [Game.h](file:///d:/Code/CrossingGame/include/Game.h) | [Game.cpp](file:///d:/Code/CrossingGame/src/Game.cpp) | Hàm `CGAME::renderPlaying` (vẽ đường đi, HUD, gọi hàm vẽ xe/thú) |
| **Nhân vật người chơi** | [People.h](file:///d:/Code/CrossingGame/include/People.h) | [People.cpp](file:///d:/Code/CrossingGame/src/People.cpp) | Lớp `CPEOPLE`, các hàm di chuyển `Up`, `Down`, `Left`, `Right` |
| **Va chạm hộp biên (AABB)** | [People.h](file:///d:/Code/CrossingGame/include/People.h) | [People.cpp](file:///d:/Code/CrossingGame/src/People.cpp) | Hàm `CPEOPLE::isImpact(CVEHICLE*)` và `isImpact(CANIMAL*)` |
| **Stage & Tăng độ khó** | [Game.h](file:///d:/Code/CrossingGame/include/Game.h) | [Game.cpp](file:///d:/Code/CrossingGame/src/Game.cpp) | Biến `mStage` và hàm `CGAME::resetGame` (tính toán số lượng + tốc độ) |
| **Dọn dẹp chướng ngại vật** | [Game.h](file:///d:/Code/CrossingGame/include/Game.h) | [Game.cpp](file:///d:/Code/CrossingGame/src/Game.cpp) | Hàm `CGAME::clearObstacles` (phòng chống rò rỉ bộ nhớ - leak memory) |

---

## 🛠️ Các Nguyên Lý Lập Trình Hướng Đối Tượng (OOP) Được Áp Dụng

Dự án này là minh chứng hoàn hảo cho việc áp dụng các nguyên lý cốt lõi của OOP:

1. **Tính Đóng Gói (Encapsulation)**: 
   - Tất cả dữ liệu của nhân vật (`mX`, `mY`, `mState`) hay phương tiện đều được khai báo dưới dạng `private` hoặc `protected` để bảo vệ dữ liệu.
   - Các lớp bên ngoài chỉ có thể truy xuất thông qua các hàm Getter/Setter công khai (`getX()`, `getY()`, `setDead()`).

2. **Tính Kế Thừa (Inheritance)**:
   - Lớp `CVEHICLE` là lớp cha cho `CTRUCK` và `CCAR`.
   - Lớp `CANIMAL` là lớp cha cho `CDINAUSOR` và `CBIRD`.

3. **Tính Đa Hình (Polymorphism)**:
   - Các phương thức `Move` và `draw` được khai báo là hàm ảo (`virtual`) ở lớp cha và ghi đè (`override`) ở các lớp con.
   - Cho phép gọi vẽ và di chuyển các đối tượng một cách thống nhất thông qua con trỏ lớp cha.

4. **Tính Trừu Tượng (Abstraction)**:
   - Lớp `CVEHICLE` và `CANIMAL` đóng vai trò là các lớp trừu tượng (Abstract Class) chứa các hàm thuần ảo (`= 0`). Người dùng không thể khởi tạo trực tiếp lớp này mà chỉ có thể khởi tạo các lớp con cụ thể của chúng.

---

## 🎮 Cách Điều Khiển Game

*   **Tại Menu chính**:
    *   Sử dụng phím **`W`** / **`S`** (hoặc phím Mũi tên **`Lên`** / **`Xuống`**) để chọn các mục.
    *   Nhấn **`ENTER`** hoặc **`SPACE`** để xác nhận chọn.
*   **Khi đang chơi game**:
    *   Sử dụng phím **`W/A/S/D`** (hoặc phím Mũi tên) để di chuyển nhân vật **`Y`** băng qua đường.
    *   Né tránh các phương tiện và quái thú di chuyển trên 4 làn đường.
    *   Nhấn **`ESC`** bất cứ lúc nào để tạm dừng và quay lại Menu chính.
*   **Khi bị tông chết (Game Over)**:
    *   Nhấn **`Y`** để tiếp tục hồi sinh và chơi lại màn chơi hiện tại.
    *   Nhấn **`ESC`** hoặc **`N`** để thoát về Menu chính.

---

Chúc bạn có những trải nghiệm tuyệt vời cùng **Crossing Game - Sword Art Online Edition**! 🗡️✨
