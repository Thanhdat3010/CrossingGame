# TRƯỜNG ĐẠI HỌC KHOA HỌC TỰ NHIÊN - ĐHQG TP.HCM
## KHOA CÔNG NGHỆ THÔNG TIN
### BỘ MÔN CÔNG NGHỆ PHẦN MỀM

---

# BÁO CÁO CHI TIẾT ĐỒ ÁN MÔN HỌC
## LẬP TRÌNH HƯỚNG ĐỐI TƯỢNG (OBJECT-ORIENTED PROGRAMMING)

### ĐỀ TÀI: TRÒ CHƠI BĂNG QUA ĐƯỜNG (CROSSING GAME)
### PHIÊN BẢN: SWORD ART ONLINE EDITION

* **Giảng viên hướng dẫn**: Thầy/Cô Bộ môn Lập trình Hướng đối tượng
* **Sinh viên thực hiện**: 
  * Họ và tên: `[Điền Họ và Tên sinh viên tại đây]`
  * Mã số sinh viên: `[Điền MSSV tại đây]`
  * Lớp: `[Điền Lớp tại đây]`
* **Thời gian hoàn thành**: Tháng 07/2026

---

## MỤC LỤC

1. [GIỚI THIỆU TỔNG QUAN & MỤC TIÊU ĐỒ ÁN](#1-giới-thiệu-tổng-quan--mục-tiêu-đồ-án)
   - 1.1 Tổng quan đề tài Trò chơi Băng qua đường (Road Crossing Game)
   - 1.2 Ý tưởng chủ đề Sword Art Online Edition
   - 1.3 Mục tiêu môn học & Các kỹ thuật OOP áp dụng
2. [KIẾN TRÚC HỆ THỐNG & CÔNG NGHỆ SỬ DỤNG](#2-kiến-trúc-hệ-thống--công-nghệ-sử-dụng)
   - 2.1 Ngôn ngữ C++17 & Đồ họa SDL3 Framework
   - 2.2 Mô hình Đa tiểu trình (Multi-threading Engine với `std::thread` & `std::mutex`)
   - 2.3 Quản lý bộ nhớ, Con trỏ và Đảm bảo Thread-Safety
3. [THIẾT KẾ CÁC LỚP HƯỚNG ĐỐI TƯỢNG (OOP CLASS DESIGN)](#3-thiết-kế-các-lớp-hướng-đối-tượng-oop-class-design)
   - 3.1 Sơ đồ Phân cấp Lớp UML (Class Hierarchy Diagram)
   - 3.2 Phân tích Chi tiết Lớp `CPEOPLE` (Nhân vật người chơi)
   - 3.3 Hệ thống `CVEHICLE` & Các Lớp Con (`CBLUEWING`, `CSKYARMOR`)
   - 3.4 Hệ thống `CANIMAL` & Các Lớp Quái Vật Boss (`CGLEAMEYES`, `CHEATHCLIFF`, `CILLFANG`, `CICEDRAGON`)
   - 3.5 Lớp `CTRAFFICLIGHT` (Cột Đèn Giao Thông Lệch Pha Làn Đường)
   - 3.6 Lớp `CFont` (Phông chữ Bitmap Pixel Custom)
   - 3.7 Lớp `CGAME` (Trung tâm Điều phối Game Engine, Save/Load Manager & Sound Pool)
4. [CHI TIẾT MỌI TÍNH NĂNG GAMEPLAY, THUẬT TOÁN VÀ UI/UX HỆ THỐNG NÂNG CAO](#4-chi-tiết-mọi-tính-năng-gameplay-thuật-toán-và-uiux-hệ-thống-nâng-cao)
   - 4.1 Cơ chế Điều khiển Single-Tap & Chống Đè Phím
   - 4.2 Cơ chế Đèn Giao Thông Lệch Pha Thời Gian (Multi-threaded Asynchronous Phase Shift)
   - 4.3 Chế độ chơi Easy Mode (Chiến dịch Cố định 1 Map Safe Start & Chuẩn hóa Làn Đường)
   - 4.4 Chế độ chơi Infinite Mode (Procedural Lane Spawning, Memory Pruning & Camera Tracking)
   - 4.5 Thuật toán Tính điểm (+1 Score / Lane) & Va chạm AABB Hitbox
   - 4.6 Hệ thống Lưu/Tải Game Visual Novel 5-Slot Fixed & Hộp Thoại Xác Nhận (Confirmation Dialogs)
   - 4.7 Hệ thống Âm thanh Tách Kênh Độc Lập (Decoupled Multi-Channel Audio Pool & Continuous BGM)
   - 4.8 Thiết kế Giao diện Light Theme SAO Aincrad, Settings 3 Cột & Lưới Slot 3 Cột Thẳng Hàng
5. [ĐÁNH GIÁ MỨC ĐỘ HOÀN THÀNH SO VỚI ĐỀ BÀI](#5-đánh-giá-mức-độ-hoàn-thành-so-với-đề-bài)
   - Bảng tổng hợp 6 Tiêu chí Chấm điểm của Giảng viên (4.1 ➔ 4.6)
   - Các kỹ thuật Nâng cao Vượt Yêu Cầu
6. [HƯỚNG DẪN BIÊN DỊCH VÀ CHẠY DỰ ÁN](#6-hướng-dẫn-biên-dịch-và-chạy-dự-án)
   - Cấu trúc thư mục mã nguồn
   - Hướng dẫn Biên dịch tự động (`compile.bat`, Visual Studio, PowerShell)

---

## 1. GIỚI THIỆU TỔNG QUAN & MỤC TIÊU ĐỒ ÁN

### 1.1 Tổng quan đề tài Trò chơi Băng qua đường (Road Crossing Game)
Trò chơi Băng qua đường (Road Crossing Game) là một tựa game arcade kinh điển năng động. Trong trò chơi, người chơi điều khiển nhân vật chính tìm cách di chuyển vượt qua chuỗi các làn đường chứa đầy chướng ngại vật (phương tiện giao thông, thú dữ/quái vật) đang di chuyển liên tục với tốc độ và hướng đi khác nhau. Mục tiêu cốt lõi của người chơi là quan sát, tính toán thời điểm và điều khiển bước nhảy chính xác để tránh bị va chạm, tiến lên bờ bên kia an toàn.

### 1.2 Ý tưởng chủ đề Sword Art Online Edition
Thay vì sử dụng giao diện văn bản console đơn điệu hay đồ họa mặc định sơ xài, đồ án được đầu tư thiết kế hoàn toàn theo chủ đề thế giới **Sword Art Online (SAO - Aincrad Edition)**:
* **Nhân vật đại diện (Hero Skins)**: Người chơi được lựa chọn 1 trong 2 anh hùng huyền thoại: **Kirito (The Black Swordsman)** với khả năng di chuyển linh hoạt, hoặc **Asuna (The Flash)** với tốc độ lướt tuyệt vời.
* **Phương tiện & Quái vật (SAO Bosses & Ships)**: Các quái vật Boss khét tiếng trong thế giới SAO như *Gleameyes* (Tầng 74 Boss), *Heathcliff* (Kị sĩ Thánh kiếm), *Cillfang* (Tầng 1 Boss), *Icedragon* (Băng Long) cùng các chiến hạm bay *Bluewing* và *Skyarmor* đóng vai trò là các chướng ngại vật trên đường.
* **Giao diện Light Theme SAO Aincrad**: Sử dụng ngôn ngữ thiết kế kính mờ bán trong suốt (**Frosted Glass / Glassmorphism**) viền xanh rêu đậm nổi bật trên phông nền phong cảnh Aincrad thiên nhiên tươi sáng.

### 1.3 Mục tiêu môn học & Các kỹ thuật OOP áp dụng
Đồ án chứng minh việc áp dụng nhuần nhuyễn 4 trụ cột cốt lõi của **Lập trình Hướng đối tượng (OOP)**:
1. **Tính Đóng gói (Encapsulation)**: Che giấu dữ liệu nội bộ bằng phạm vi `private`/`protected` (tọa độ `mX`, `mY`, máu/trạng thái `mState`, kết cấu ảnh `mTexture`, danh sách khe lưu `mSaveSlots[5]`). Các lớp chỉ giao tiếp thông qua các phương thức Getter/Setter và Interface công khai.
2. **Tính Kế thừa (Inheritance)**: Thiết lập cấu trúc phân cấp lớp rõ ràng. Lớp cha `CVEHICLE` kế thừa sang `CBLUEWING`, `CSKYARMOR`. Lớp cha `CANIMAL` kế thừa sang `CGLEAMEYES`, `CHEATHCLIFF`, `CILLFANG`, `CICEDRAGON`.
3. **Tính Đa hình (Polymorphism)**: Định nghĩa các phương thức thuần ảo `virtual void Move(int limitX1, int limitX2) = 0;` và `virtual void draw(...) = 0;` ở lớp cơ sở. Các lớp con ghi đè (`override`) hành vi di chuyển và hiển thị hình ảnh hoạt họa riêng biệt.
4. **Tính Trừu tượng (Abstraction)**: Mô hình hóa các thực thể thế giới thực (người, xe cộ, sinh vật, cột đèn giao thông, hệ thống lưu slot Visual Novel, kênh âm thanh audio pool) thành các lớp đối tượng trừu tượng gọn gàng, tách biệt trách nhiệm.

---

## 2. KIẾN TRÚC HỆ THỐNG & CÔNG NGHỆ SỬ DỤNG

### 2.1 Ngôn ngữ C++17 & Đồ họa SDL3 Framework
* **C++17**: Chuẩn ngôn ngữ C++ hiện đại giúp quản lý bộ nhớ an toàn, hỗ trợ bộ thư viện chuẩn phong phú (`std::thread`, `std::mutex`, `std::atomic`, `std::vector`, `std::unique_ptr`, `std::filesystem`).
* **SDL3 (Simple DirectMedia Layer v3.0)**: Thư viện phần mềm đồ họa 2D mới nhất hỗ trợ kết xuất tăng tốc phần cứng (Hardware-accelerated rendering) đạt tốc độ 60 FPS mượt mà.
* **SDL3_mixer**: Bộ thư viện trộn và phát âm thanh đa kênh, xử lý nhạc nền MP3 liên tục (Continuous BGM Looping) và hiệu ứng âm thanh bước nhảy/va chạm với cơ chế Tách Kênh Độc Lập (Audio Channel Decoupling).

### 2.2 Mô hình Đa tiểu trình (Multi-threading Engine với `std::thread` & `std::mutex`)
Dự án được thiết kế theo kiến trúc **Engine Đa tiểu trình (Multi-threaded Engine)** song song chuẩn xác:
* **Luồng chính (Main UI & Render Thread)**: Tiếp nhận sự kiện bàn phím/chuột từ người dùng (`handleInput()`) và vẽ giao diện đồ họa 60Hz lên màn hình (`render()`).
* **Luồng phụ vật lý (Physics Worker Thread - `std::thread mPhysicsThread`)**: Vận hành vòng lặp vật lý độc lập 100Hz (`physicsWorkerFunc()`), tính toán tọa độ di chuyển của chướng ngại vật, đếm thời gian tín hiệu đèn giao thông lệch pha, cuộn camera mượt mà và kiểm tra va chạm AABB Hitbox.

> 📍 **[VỊ TRÍ CHÈN HÌNH ÁNH 1: Sơ đồ luồng đa tiểu trình (Multi-threading Diagram)]**
> 
> *Hướng dẫn chèn hình*: Tạo sơ đồ minh họa luồng Main Thread và Physics Thread chạy song song giao tiếp qua `mGameMutex`.
> 
> ![Sơ đồ Luồng Đa Tiểu Trình](images/ui_multithreading_diagram.png)
> *Hình 1: Mô hình kiến trúc Đa tiểu trình (Multi-threading Architecture) sử dụng std::thread và std::mutex*

### 2.3 Quản lý bộ nhớ, Con trỏ và Đảm bảo Thread-Safety
* **Giải quyết xung đột dữ liệu (Thread-Safety)**: Để ngăn chặn tuyệt đối lỗi sập game do xung đột vùng nhớ (Access Violation `0xc0000005`), tất cả các truy xuất đọc/ghi vào mảng cấu trúc chung (`mLanes`, `mBluewings`, `mGleameyes`, `mPlayer`, `mTrafficLights`, `mSaveSlots`,...) giữa 2 luồng đều được bảo vệ bằng `std::lock_guard<std::mutex> lock(mGameMutex);`.
* **Quản lý bộ nhớ RAII**: Destructor `~CGAME()` tự động hủy các kết cấu kết xuất `SDL_Texture*`, giải phóng tài nguyên sound tracks `MIX_Audio*`, hủy luồng `mPhysicsThread.join()` và giải phóng bộ nhớ con trỏ động, đảm bảo không rò rỉ bộ nhớ (Memory Leak).

---

## 3. THIẾT KẾ CÁC LỚP HƯỚNG ĐỐI TƯỢNG (OOP CLASS DESIGN)

### 3.1 Sơ đồ Phân cấp Lớp UML (Class Hierarchy Diagram)

> 📍 **[VỊ TRÍ CHÈN HÌNH ÁNH 2: Sơ đồ Lớp UML Tổng Quan (UML Class Diagram)]**
> 
> *Hướng dẫn chèn hình*: Bạn có thể xuất sơ đồ bên dưới ra file ảnh `images/uml_class_diagram.png` và chèn vào báo cáo.
> 
> ![Sơ đồ Lớp UML Tổng Quan](images/uml_class_diagram.png)
> *Hình 2: Sơ đồ phân cấp lớp (UML Class Diagram) của dự án Crossing Game*

```mermaid
classDiagram
    struct SaveSlotInfo {
        +string filename
        +bool exists
        +string timestamp
        +int score
        +string mode
        +int stage
    }

    class CGAME {
        -SDL_Window* mWindow
        -SDL_Renderer* mRenderer
        -GameState mState
        -CPEOPLE mPlayer
        -vector~CVEHICLE*~ mBluewings
        -vector~CVEHICLE*~ mSkyarmors
        -vector~CANIMAL*~ mGleameyes
        -vector~CANIMAL*~ mCheathcliffs
        -vector~CANIMAL*~ mCillfangs
        -vector~CANIMAL*~ mCicedragons
        -vector~CTRAFFICLIGHT~ mTrafficLights
        -SaveSlotInfo mSaveSlots[5]
        -MIX_Audio* mSfxTracks[4]
        -int mPendingSaveSlotIndex
        -int mPendingLoadSlotIndex
        -string mPendingDeleteFileName
        -thread mPhysicsThread
        -mutex mGameMutex
        +init()
        +run()
        +handleInput()
        +update()
        +render()
        +resetEasyMode()
        +resetInfinite()
        +saveGame(slotIndex)
        +loadGame(slotIndex)
        +scanSaveSlots()
        +updateVolumeSettings()
        +playSFX(sfx)
    }

    class CPEOPLE {
        -int mX, mY
        -bool mState
        -int mSpeed
        -CharacterType mCharType
        +Up()
        +Down()
        +Left()
        +Right()
        +isImpact(CVEHICLE*)
        +isImpact(CANIMAL*)
        +isFinish()
    }

    class CVEHICLE {
        <<abstract>>
        #int mX, mY
        #int mSpeed, mDirection
        #SDL_Texture* mTexture
        +Move(limitX1, limitX2)*
        +draw()*
        +getHitbox()
    }

    class CBLUEWING {
        +Move()
        +draw()
    }

    class CSKYARMOR {
        +Move()
        +draw()
    }

    class CANIMAL {
        <<abstract>>
        #int mX, mY
        #int mSpeed, mDirection
        #SDL_Texture* mTexture1, mTexture2
        +Move(limitX1, limitX2)*
        +draw()*
        +Tell()
    }

    class CGLEAMEYES {
        +Move()
        +draw()
    }

    class CHEATHCLIFF {
        +Move()
        +draw()
    }

    class CILLFANG {
        +Move()
        +draw()
    }

    class CICEDRAGON {
        +Move()
        +draw()
    }

    class CTRAFFICLIGHT {
        -int mLaneY
        -bool mIsRed
        -float mTimer
        -float mRedDuration
        -float mGreenDuration
        +update()
        +draw()
        +isRed()
    }

    CVEHICLE <|-- CBLUEWING
    CVEHICLE <|-- CSKYARMOR
    CANIMAL <|-- CGLEAMEYES
    CANIMAL <|-- CHEATHCLIFF
    CANIMAL <|-- CILLFANG
    CANIMAL <|-- CICEDRAGON

    CGAME *-- CPEOPLE
    CGAME *-- SaveSlotInfo
    CGAME o-- CVEHICLE
    CGAME o-- CANIMAL
    CGAME o-- CTRAFFICLIGHT
```

---

### 3.2 Phân tích Chi tiết Lớp `CPEOPLE` (Nhân vật người chơi)
* **Tệp mã nguồn**: `src/include/CPEOPLE.h` và `src/source/CPEOPLE.cpp`
* **Nhiệm vụ**: Đại diện cho nhân vật người chơi di chuyển trên bản đồ.

> 📍 **[VỊ TRÍ CHÈN HÌNH ÁNH 3: Sprite Nhân Vật Kirito & Asuna]**
> 
> ![Sprite Nhân Vật Kirito và Asuna](images/sprite_characters.png)
> *Hình 3: Sprite nhân vật Kirito (The Black Swordsman) và Asuna (The Flash)*

* **Đóng gói & Chỉ số RPG**:
  ```cpp
  enum class CharacterType { KIRITO, ASUNA };
  ```
  * **Kirito**: Tốc độ di chuyển tiêu chuẩn ($Speed = 80px$), Hitbox nhỏ gọn.
  * **Asuna**: Tốc độ lướt cực nhanh ($Speed = 80px$), hoạt họa mượt mà.
* **Các phương thức chính**:
  * `Up(limitY)`, `Down(limitY)`, `Left(limitX)`, `Right(limitX)`: Cập nhật tọa độ di chuyển 4 hướng có kiểm tra ranh giới màn hình.
  * `getHitbox()`: Trả về khung hình học va chạm thu nhỏ 20% giúp tính toán va chạm chính xác, công bằng:
    ```cpp
    SDL_FRect getHitbox() const {
        const float PAD_X = mWidth  * 0.20f;
        const float PAD_Y = mHeight * 0.20f;
        return { (float)mX + PAD_X, (float)mY + PAD_Y,
                 (float)mWidth - 2 * PAD_X, (float)mHeight - 2 * PAD_Y };
    }
    ```

---

### 3.3 Hệ thống `CVEHICLE` & Các Lớp Con (`CBLUEWING`, `CSKYARMOR`)
* **Lớp cha trừu tượng `CVEHICLE`**:
  * `src/include/CVEHICLE.h` và `src/source/CVEHICLE.cpp`
  * Chứa các thuộc tính chung: `mX`, `mY`, `mWidth`, `mHeight`, `mSpeed`, `mDirection`, `mTexture`.
  * Khai báo phương thức thuần ảo: `virtual void Move(int limitX1, int limitX2) = 0;` và `virtual void draw(...) = 0;`.
* **Lớp con `CBLUEWING`**:
  * Phương tiện bay tầm trung dạng chiến hạm xanh. Di chuyển ngang làn đường cao tốc và biết dừng lại khi gặp đèn đỏ.
* **Lớp con `CSKYARMOR`**:
  * Phương tiện giáp sắt bay tầm cao. Di chuyển với tốc độ biến thiên trên làn xe.

---

### 3.4 Hệ thống `CANIMAL` & Các Lớp Quái Vật Boss
* **Lớp cha trừu tượng `CANIMAL`**:
  * `src/include/CANIMAL.h` và `src/source/CANIMAL.cpp`
  * Lưu trữ 2 kết cấu ảnh (`mTexture1`, `mTexture2`) để tạo hiệu ứng hoạt họa vỗ cánh/bước đi sinh động.
  * Phương thức `Tell()` cho phép phát ra âm thanh gầm kêu đặc trưng của từng quái thú.

> 📍 **[VỊ TRÍ CHÈN HÌNH ÁNH 4: Sprite Quái Vật Boss & Phương Tiện]**
> 
> ![Sprite Quái Vật Boss và Phương Tiện](images/sprite_monsters_vehicles.png)
> *Hình 4: Sprite 4 Boss SAO (Gleameyes, Heathcliff, Cillfang, Icedragon) và 2 phương tiện (Bluewing, Skyarmor)*

* **4 Lớp con Quái vật Boss SAO**:
  1. `CGLEAMEYES` (Ác quỷ mắt xanh Tầng 74): Tốc độ di chuyển nhanh trên làn rừng rậm.
  2. `CHEATHCLIFF` (Kị sĩ Thánh kiếm): Di chuyển ngang vững chắc trên làn quái vật.
  3. `CILLFANG` (Chúa tể Răng nanh Tầng 1): Di chuyển liên tục theo nhịp vỗ cánh.
  4. `CICEDRAGON` (Băng Long): Bay lượn tầm cao với sải cánh rộng.

---

### 3.5 Lớp `CTRAFFICLIGHT` (Cột Đèn Giao Thông Lệch Pha Làn Đường)
* **Tệp mã nguồn**: `src/include/CTRAFFICLIGHT.h` và `src/source/CTRAFFICLIGHT.cpp`
* **Nhiệm vụ**: Quản lý tín hiệu dừng xe tự động trên các làn đường giao thông.

> 📍 **[VỊ TRÍ CHÈN HÌNH ÁNH 5: Cột Đèn Giao Thông Neon]**
> 
> ![Cột Đèn Giao Thông Neon](images/ui_traffic_light.png)
> *Hình 5: Cột đèn giao thông Pixel Art phát sáng Neon ở 2 bên vỉa hè*

* **Cơ chế hoạt động**:
  * Đếm ngược thời gian `mTimer` theo delta-time trong luồng vật lý độc lập.
  * Tự động luân chuyển giữa **Đèn Xanh (5.0s)** và **Đèn Đỏ (3.0s)**.
  * Khi `isRed() == true`, tất cả phương tiện `CVEHICLE` (`CBLUEWING`, `CSKYARMOR`) thuộc làn đường đó sẽ tự động hãm vận tốc về 0.
  * Hiển thị cột đèn Pixel Art có hiệu ứng Glow Neon phát sáng ở cả 2 bên vỉa hè (`X = 15px` và `X = 1225px`).

---

### 3.6 Lớp `CFont` (Phông chữ Bitmap Pixel Custom)
* **Tệp mã nguồn**: `src/include/CFont.h` và `src/source/CFont.cpp`
* **Đặc điểm**: Tự cài đặt bộ phông chữ Bitmap Pixel 8x8 trực tiếp bằng thuật toán mã hóa mảng bit (Bit-mask encoding) trong mã C++, không cần nạp các file phông chữ bên ngoài (`.ttf`).
* **Tính năng**: Hỗ trợ vẽ chữ chuẩn ASCII, căn giữa văn bản `drawTextCentered()`, căn giữa trong khung hình học `drawTextCenteredInBox()`, thay đổi tỷ lệ kích thước (scale factor) và tô màu linh hoạt.

---

### 3.7 Lớp `CGAME` (Trung tâm Điều phối Game Engine, Save/Load Manager & Sound Pool)
* **Tệp mã nguồn**: `src/include/CGAME.h` và `src/source/CGAME.cpp`
* **Nhiệm vụ**: Đóng vai trò là Game Manager trung tâm điều phối toàn bộ vòng lặp ứng dụng, quản lý tài nguyên, xử lý sự kiện và kết xuất đồ họa.
* **Quản lý Trạng thái GameState**:
  * `MENU`: Màn hình thực đơn chính.
  * `CHAR_SELECT`: Màn hình chọn nhân vật (Kirito / Asuna).
  * `STAGE_SELECT`: Màn hình chọn chế độ chơi (Easy Mode / Infinite Mode).
  * `SETTINGS`: Màn hình cài đặt âm thanh 3 cột chuẩn đẹp.
  * `PLAYING`: Màn chơi đang diễn ra.
  * `PAUSED`: Tạm dừng màn chơi.
  * `SAVE_DIALOG` / `LOAD_DIALOG`: Màn hình lưu/tải game 5 slot Visual Novel với lưới 3 cột thẳng hàng.
  * `GAMEOVER`: Màn hình thông báo kết thúc (Thắng/Thua).
* **Quản lý Hộp thoại Xác nhận (Confirmation Modals)**: Quản lý biến trạng thái `mPendingSaveSlotIndex`, `mPendingLoadSlotIndex`, `mPendingDeleteFileName` để bật/tắt các Popup xác nhận Đồng ý `[Y]` / Hủy `[N]`.

---

## 4. CHI TIẾT MỌI TÍNH NĂNG GAMEPLAY, THUẬT TOÁN VÀ UI/UX HỆ THỐNG NÂNG CAO

### 4.1 Cơ chế Điều khiển Single-Tap & Chống Đè Phím
* **Phím di chuyển**: Hỗ trợ cụm phím **`W`, `A`, `S`, `D`** và các phím **Mũi tên** (`UP`, `DOWN`, `LEFT`, `RIGHT`).
* **Thuật toán Chống đè phím (Single-Tap Movement)**:
  * Trong `handleInput()`, hệ thống kiểm tra cờ `event.key.repeat`.
  * Nếu người chơi bấm giữ đè phím, sự kiện lặp phím tự động của OS sẽ bị bỏ qua (`if (!event.key.repeat)`). Người chơi bắt buộc phải nhả phím và bấm lại để nhảy từng bước một, bảo toàn độ chính xác khi căn thời gian nhảy né xe.

---

### 4.2 Cơ chế Đèn Giao Thông Lệch Pha Thời Gian (Multi-threaded Asynchronous Phase Shift)

#### 1. Lý do thiết kế & Ý nghĩa Gameplay
Trong thực tế giao thông cũng như trong thiết kế trò chơi, nếu tất cả các làn đường xe chạy (`VEHICLE`) đều bật Đèn Đỏ cùng một lúc và bật Đèn Xanh cùng một lúc, toàn bộ các luồng xe trên bản đồ sẽ đồng loạt dừng lại rồi đồng loạt tăng tốc. Điều này tạo ra trải nghiệm gameplay bị rập khuôn, gượng gạo và thiếu tính tự nhiên. 

Do đó, đồ án đã cài đặt **Cơ chế Đèn Giao Thông Lệch Pha Thời Gian (Asynchronous Phase Shift Traffic Lights)**. Mỗi cột đèn trên các làn đường giao thông khác nhau sẽ có chu kỳ chuyển màu xanh/đỏ lệch nhau một khoảng thời gian ngẫu nhiên hoặc cố định, tạo ra luồng giao thông nhịp nhàng, đa dạng và tăng tính thử thách chiến thuật cho người chơi khi tính toán bước nhảy.

#### 2. Công thức Toán học & Thuật toán Cài đặt Kỹ thuật
* **Thông số Chu kỳ Đèn**:
  * Thời gian Đèn Đỏ ($T_{red}$): $3.0$ giây.
  * Thời gian Đèn Xanh ($T_{green}$): $5.0$ giây.
  * Tổng chu kỳ luân chuyển ($T_{total} = T_{red} + T_{green}$): $8.0$ giây.
* **Tạo độ trễ lệch pha (Phase Offset Initialization)**:
  Khi khởi tạo các làn đường xe chạy trong `resetEasyMode()` hoặc `initInfiniteLanes()`, mỗi đối tượng `CTRAFFICLIGHT` tự động tính toán một biến độ trễ lệch pha ban đầu `initialOffset` dựa trên vị trí tọa độ làn `mLaneY` trong hàm khởi tạo constructor:
  ```cpp
  // Tính độ trễ lệch pha tự động từ tọa độ làn laneY trong constructor CTRAFFICLIGHT
  float totalCycle = redDur + greenDur;
  float initialOffset = fmodf((float)std::abs(laneY * 17 + 13), totalCycle);
  if (initialOffset < redDur) {
      mIsRed = true;
      mTimer = initialOffset;
  } else {
      mIsRed = false;
      mTimer = initialOffset - redDur;
  }
  ```
* **Cập nhật trạng thái trong Luồng Vật lý 100Hz (Thread-Safe Update)**:
  Trong luồng vật lý độc lập `mPhysicsThread`, thời gian tích lũy `mTimer` tăng lên theo `deltaTime`. Khi `mTimer` vượt quá thời hạn giữ đèn hiện tại `currentLimit` (`mRedDuration` 3.0s hoặc `mGreenDuration` 5.0s), màu đèn `mIsRed` tự động đảo ngược trạng thái và trừ lùi thời gian dư `mTimer -= currentLimit`.

#### 3. Tương tác Luồng Xe & Kết xuất Đồ họa Neon
* Khi `t.isRed() == true`, hàm vật lý kiểm tra làn xe tương ứng và gọi `vehicle->setSpeed(0)`, khiến luồng xe bay `CBLUEWING` và `CSKYARMOR` dừng lại hoàn toàn trước vạch dừng. Khi `t.isRed() == false`, luồng xe tự động phục hồi vận tốc di chuyển ban đầu `vehicle->restoreSpeed()`.
* **Hiệu ứng Glow Neon**: Cột đèn giao thông được vẽ ở 2 bên mép màn hình (`X = 15px` và `X = 1225px`). Khi đèn đỏ bật, đồ họa quầng sáng màu đỏ mờ rực rỡ tỏa ra xung quanh cột đèn; khi đèn xanh bật, quầng sáng chuyển sang màu xanh ngọc bích Neon, mang lại hiệu ứng thị giác hiện đại.

---

### 4.3 Chế độ chơi Easy Mode (Chiến dịch Cố định 1 Map Safe Start & Chuẩn hóa Làn Đường)
Chế độ Easy Mode cung cấp cho người chơi mới một màn chơi chiến dịch 1 map cố định với tốc độ an toàn (Safe Start) để làm quen với cơ chế di chuyển, quan sát xe cộ và đèn giao thông:
* **Chuẩn hóa Phân loại 6 Làn Đường Cố định**:
  1. **Tọa độ Y = 0.0f ➔ 120.0f (Vỉa Hè An Toàn Phía Trên - Safe Zone Destination)**: Dải cỏ thiên nhiên Aincrad trang trí hoa rực rỡ. Khi nhân vật bước vào dải này, hàm `isFinish()` trả về `true` và kích hoạt chiến thắng `VICTORY!`.
  2. **Tọa độ Y = 120.0f (Làn Xe Đường Bộ 1 - `VEHICLE`)**: Kết cấu mặt đường nhựa `mLaneRoadTexture` có vạch kẻ đường, kết hợp Cột Đèn Giao Thông `t1` điều khiển luồng xe bay `CBLUEWING` / `CSKYARMOR` di chuyển từ phải sang trái.
  3. **Tọa độ Y = 200.0f (Làn Rừng Quái Vật 1 - `MONSTER`)**: Kết cấu rừng xanh `mLaneForestTexture` chứa các quái vật Boss SAO (`CILLFANG`, `CICEDRAGON`, `CHEATHCLIFF`, `CGLEAMEYES`) di chuyển từ trái sang phải.
  4. **Tọa độ Y = 280.0f (Làn Nghỉ An Toàn Trung Tâm - `REST`)**: Kết cấu vỉa hè đá mờ an toàn, là nơi người chơi có thể tạm dừng chân nghỉ ngơi căn thời gian nhảy tiếp mà không sợ bị va chạm.
  5. **Tọa độ Y = 360.0f (Làn Rừng Quái Vật 2 - `MONSTER`)**: Kết cấu rừng xanh `mLaneForestTexture` chứa quái vật SAO di chuyển từ trái sang phải.
  6. **Tọa độ Y = 440.0f (Làn Xe Đường Bộ 2 - `VEHICLE`)**: Kết cấu mặt đường nhựa `mLaneRoadTexture` kết hợp Cột Đèn Giao Thông `t2` điều khiển luồng xe bay di chuyển từ phải sang trái.
  7. **Tọa độ Y = 520.0f (Làn Rừng Quái Vật 3 - `MONSTER`)**: Kết cấu rừng xanh `mLaneForestTexture` chứa quái vật SAO.
  8. **Tọa độ Y = 600.0f ➔ 720.0f (Vỉa Hè An Toàn Phía Dưới - Safe Zone Start)**: Dải xuất phát ban đầu của nhân vật.

---

### 4.4 Chế độ chơi Infinite Mode (Procedural Lane Spawning, Memory Pruning & Camera Tracking)
Chế độ chơi vô tận (Infinite Mode) cung cấp thử thách chinh phục điểm số không giới hạn:
* **Sinh Làn Đường Tự Động (Procedural Generation Algorithm)**:
  * Khởi tạo mảng làn đường `mLanes`. Khi nhân vật di chuyển lên cao, hàm `addLaneAbove()` tự động sinh ngẫu nhiên loại làn đường mới ở tọa độ phía trên (`worldY` giảm dần).
  * Tỷ lệ xuất hiện ngẫu nhiên: $40\%$ Làn Xe (`VEHICLE`), $40\%$ Làn Quái Vật (`MONSTER`), và $20\%$ Làn Nghỉ An Toàn (`REST`).
  * Tốc độ di chuyển và số lượng chướng ngại vật trên mỗi làn được tính toán tăng dần theo điểm số `mScore`, nâng cao độ khó thử thách theo thời gian chơi.
* **Giải phóng Bộ nhớ Tự động (Memory Pruning)**:
  * Để ngăn chặn rò rỉ bộ nhớ RAM khi người chơi di chuyển lên hàng ngàn làn đường, hàm `pruneLanes()` trong luồng vật lý liên tục kiểm tra tọa độ các làn đường và chướng ngại vật.
  * Bất kỳ làn đường hoặc chướng ngại vật nào trôi xuống quá mép dưới màn hình (`worldY > mCameraY + 800.0f`) sẽ tự động được `delete` giải phóng con trỏ và xóa khỏi mảng `std::vector`, giữ cho RAM luôn ổn định ở mức thấp.
* **Thuật toán Cuộn Camera Mượt (Smooth Camera Tracking)**:
  * Khi vị trí màn hình của nhân vật nhảy vượt qua mốc trung tâm phía trên ($Y_{screen} < 200px$), tọa độ `mCameraY` tự động cập nhật cuộn lên phía trên theo công thức:
    $$mCameraY = mPlayer.getY() - 200.0f$$
  * Camera chỉ cuộn tiến lên phía trên, không bao giờ cuộn lùi xuống dưới, ép buộc người chơi phải liên tục tiến về phía trước.

---

### 4.5 Thuật toán Tính điểm (+1 Score / Lane) & Va chạm AABB Hitbox

#### 1. Thuật toán Tính điểm Chống Gian lận
* Để ngăn chặn hành vi nhảy lùi lại rồi nhảy tiến lên ở cùng một làn đường để gian lận tích điểm, hệ thống duy trì biến `mMaxReachedY` ghi nhận tọa độ $Y$ cao nhất mà nhân vật từng đạt được.
* Điểm số `mScore` chỉ được cộng $+1$ khi người người chơi thực sự vượt qua một cột mốc vị trí mới:
  ```cpp
  if (mPlayer.getY() < mMaxReachedY) {
      int lanesPassed = (mMaxReachedY - mPlayer.getY()) / mLaneHeight;
      mScore += lanesPassed;
      mMaxReachedY = mPlayer.getY(); // Cập nhật mốc kỷ lục mới
  }
  ```

#### 2. Thuật toán Va chạm AABB Hitbox (Axis-Aligned Bounding Box)
Va chạm giữa nhân vật `CPEOPLE` và các chướng ngại vật (`CVEHICLE`, `CANIMAL`) được tính toán bằng thuật toán giao nhau giữa 2 hình chữ nhật song song với trục tọa độ:
```cpp
bool checkAABB(const SDL_FRect& a, const SDL_FRect& b) {
    return (a.x < b.x + b.w && 
            a.x + a.w > b.x &&
            a.y < b.y + b.h && 
            a.y + a.h > b.y);
}
```
Khung Hitbox của người chơi được thu nhỏ $20\%$ lề biên (`PAD_X`, `PAD_Y`) so với kích thước kết cấu ảnh sprite thực tế, tạo khoảng dung sai hợp lý giúp trải nghiệm chơi né đạn/né xe trở nên công bằng và hấp dẫn hơn.

---

### 4.6 Hệ thống Lưu/Tải Game Visual Novel 5-Slot Fixed & Hộp Thoại Xác Nhận (Confirmation Dialogs)

#### 1. Hệ thống 5 Slot Lưu Cố Định (`slot1.txt` ➔ `slot5.txt`)
Thay vì sử dụng hộp thoại nhập tên file tự do bằng bàn phím dễ gây ra lỗi gõ sai đường dẫn, trùng lặp tên file hoặc tràn bộ đệm văn bản, trò chơi quy hoạch thành **5 khe lưu cố định chuẩn phong cách Visual Novel**:
* Dữ liệu các slot lưu trữ trong thư mục `saves/` dưới dạng các tệp văn bản `slot1.txt` đến `slot5.txt`.
* Cấu trúc siêu dữ liệu chuẩn hóa trong khối `[HEADER]`:
  ```ini
  [HEADER]
  version=1
  date=07-27 09:51
  mode=1
  stage=1
  score=99
  ```
* **Khóa Lưu ở Chế độ Easy Mode**: Tiến trình chơi màn Easy Mode là cố định ngắn hạn. Để tránh rác dữ liệu lưu và bảo toàn logic màn chơi, hệ thống kiểm tra `if (!mIsInfinityMode)` để vô hiệu hóa tính năng Save ở chế độ Easy Mode, đồng thời hiển thị thông báo cảnh báo màu đỏ `"EASY MODE CANNOT BE SAVED! INFINITE MODE ONLY"`.

#### 2. Hộp thoại Xác nhận An toàn 3 Trạng thái (Confirmation Modal Dialogs)
Để ngăn ngừa tuyệt đối hành vi lỡ tay xóa nhầm file save quý giá hoặc ghi đè đè đè tiến trình đang chơi, hệ thống xây dựng 3 bảng Popup nổi xác nhận chuyên biệt (`renderSaveConfirmDialog()`, `renderLoadConfirmDialog()`, `renderDeleteConfirmDialog()`):
* **Cấu trúc Giao diện Modal**: Sử dụng lớp phủ tối `SDL_BLENDMODE_BLEND` với độ mờ $70\%$ (Alpha 180) bao phủ toàn màn hình. Bảng Popup chính ở vị trí trung tâm ($600 \times 280 px$) có đường viền viền nổi rực rỡ màu chủ đề:
  * **Popup Xóa File (`DELETE SAVE FILE`)**: Viền màu Đỏ tươi, hiển thị rõ tên file `"ARE YOU SURE YOU WANT TO DELETE: [ slot1.txt ]"` cùng cảnh báo `"THIS ACTION CANNOT BE UNDONE!"`.
  * **Popup Ghi Lưu (`SAVE GAME CONFIRMATION`)**: Viền màu Xanh lục rêu, hiển thị tên slot `"OVERWRITE GAME DATA IN: [ SLOT 1 : 07-27 09:51 ]"` cùng cảnh báo `"PREVIOUS DATA IN THIS SLOT WILL BE REPLACED!"`.
  * **Popup Nạp Bài (`LOAD GAME CONFIRMATION`)**: Viền màu Xanh dương, hiển thị slot nạp `"LOAD SAVED PROGRESS FROM: [ SLOT 1 : 07-27 09:51 ]"` cùng cảnh báo `"UNSAVED CURRENT PROGRESS WILL BE LOST!"`.
* **Cơ chế Điều khiển Đa năng**: Hỗ trợ đồng thời cả phím tắt (`Y` / `ENTER` / `SPACE` để Đồng ý; `N` / `ESC` để Hủy) và sự kiện nhấp chuột trái trực tiếp vào tọa độ các hình chữ nhật đại diện cho Nút `[ Y ] YES` và Nút `[ N ] CANCEL`.

---

### 4.7 Hệ thống Âm thanh Tách Kênh Độc Lập (Decoupled Multi-Channel Audio Pool & Continuous BGM)

#### 1. Nguyên lý Kiến trúc Audio Decoupling Pool
Trong các phiên bản phát triển ban đầu, việc điều chỉnh master gain duy nhất làm nảy sinh bất cập: khi người chơi muốn tắt hiệu ứng sound SFX (tiếng bước nhảy), nhạc nền BGM cũng bị tắt theo do sử dụng chung kênh gain.

Đồ án đã giải quyết triệt để vấn đề này bằng kiến trúc **Sound Channel Pool Độc Lập**:
* Khởi tạo một mảng gồm **4 Audio Track SFX chuyên dụng** (`MIX_Audio* mSfxTracks[4]`) trong `CGAME::init()`, hoạt động hoàn toàn độc lập với kênh phát Nhạc Nền chính (`MIX_Audio* mBgmTrack`).
* Biến `mBgmVolume` ($0 \rightarrow 100$) chỉ tác động duy nhất lên kênh BGM thông qua hàm `MIX_SetTrackGain(mBgmTrack, bgmGain)`.
* Biến `mSfxVolume` ($0 \rightarrow 100$) chỉ tác động duy nhất lên 4 kênh SFX thông qua vòng lặp `MIX_SetTrackGain(mSfxTracks[i], sfxGain)`.
* Nhờ cơ chế tách kênh này, người chơi có thể tự do tùy chỉnh tắt sạch tiếng hiệu ứng bước nhảy mà vẫn thưởng thức nhạc nền BGM bình thường, hoặc ngược lại.

#### 2. Thuật toán Xoay Vòng Kênh SFX (Round-Robin Track Allocation) & Loop Nhạc Nền
* Khi phát các âm thanh ngắn phát sinh liên tục như bước nhảy `sfx_jump`, va chạm `sfx_hit` hay tiếng gầm quái thú `Tell()`, hàm `playSFX(MIX_Audio* sfx)` sử dụng thuật toán phân bổ kênh xoay vòng Round-Robin:
  ```cpp
  void CGAME::playSFX(MIX_Audio* sfx) {
      if (!sfx || mSfxMuted || mSfxVolume <= 0) return;
      mSfxTrackIndex = (mSfxTrackIndex + 1) % 4; // Xoay vòng qua 4 track 0, 1, 2, 3
      MIX_PlayAudio(mSfxTracks[mSfxTrackIndex], sfx);
  }
  ```
  Thuật toán này đảm bảo các âm thanh phát đè lên nhau (ví dụ: vừa nhảy vừa va chạm hoặc nhiều quái gầm cùng lúc) đều được phát tròn tiếng mượt mà, không bị ngắt nén hoặc nghẽn kênh.
* **Tự động Phát Lặp Nhạc Nền Liền Mạch (Continuous BGM Looping)**: Trong hàm `update()`, hệ thống liên tục kiểm tra `MIX_TrackPlaying(mBgmTrack)`. Khi bản nhạc `bgm_menu.mp3` chạy tới giây cuối cùng, hệ thống tự động gọi `playBGM()` để phát lại ngay lập tức mà không để lại khoảng lặng ngắt quãng.

---

### 4.8 Thiết kế Giao diện Light Theme SAO Aincrad, Settings 3 Cột & Lưới Slot 3 Cột Thẳng Hàng

> 📍 **[VỊ TRÍ CHÈN HÌNH ÁNH 6: Giao diện Menu Chính Light Theme]**
> 
> ![Giao diện Menu Chính Light Theme](images/ui_main_menu.png)
> *Hình 6: Giao diện Menu chính phong cách Light Theme Frosted Glass SAO Aincrad*

> 📍 **[VỊ TRÍ CHÈN HÌNH ÁNH 7: Màn hình Chọn Nhân Vật SAO]**
> 
> ![Màn hình Chọn Nhân Vật SAO](images/ui_char_select.png)
> *Hình 7: Màn hình Chọn Nhân Vật (Kirito vs Asuna)*

> 📍 **[VỊ TRÍ CHÈN HÌNH ÁNH 8: Màn hình Chọn Chế Độ Chơi]**
> 
> ![Màn hình Chọn Chế Độ Chơi](images/ui_stage_select.png)
> *Hình 8: Màn hình Chọn Chế Độ Chơi (Easy Mode vs Infinite Mode)*

> 📍 **[VỊ TRÍ CHÈN HÌNH ÁNH 9: Giao diện Màn chơi Infinite Mode]**
> 
> ![Giao diện Màn chơi Infinite Mode](images/ui_gameplay_infinite.png)
> *Hình 9: Giao diện màn chơi Infinite Mode với Đèn giao thông và HUD Score*

> 📍 **[VỊ TRÍ CHÈN HÌNH ÁNH 10: Giao diện Cài Đặt Settings 3 Cột]**
> 
> ![Giao diện Cài Đặt Settings 3 Cột](images/ui_settings.png)
> *Hình 10: Màn hình Cài Đặt (Settings Menu) 3 cột gọn gàng điều chỉnh Music BGM & Sound SFX*

> 📍 **[VỊ TRÍ CHÈN HÌNH ÁNH 11: Màn hình Save/Load Slot Lưới 3 Cột & Confirm Modal]**
> 
> ![Màn hình Save/Load Slot 3 Cột](images/ui_saveload_modal.png)
> *Hình 11: Màn hình Save/Load với Lưới 3 Cột thẳng hàng và Hộp thoại Xác nhận Confirmation Modal*

#### Các Điểm Nổi Bật Trong Thiết Kế UI/UX:

1. **Settings Menu Cấu Trúc 3 Cột Chuẩn Xác**:
   Bảng cài đặt âm thanh được thiết kế bố cục thành 3 cột riêng biệt tính toán theo hình học độ phân giải $1280 \times 720$:
   * **Cột 1 (Tên Tính Năng)**: Bắt đầu tại $X = 320px$ (`panelX + 80`), hiển thị tên `1. MUSIC VOLUME`, `2. SOUND SFX`, `3. BACK`.
   * **Cột 2 (Thanh Progress Bar)**: Bắt đầu tại $X = 620px$ (`panelX + 380`), chiều rộng thanh `barW = 160px`.
   * **Cột 3 (Phần Trăm Âm Lượng)**: Bắt đầu tại $X = 800px$ (`panelX + 560`), hiển thị `100%`, `75%`, `50%`, `25%`, `OFF`.
   * Thiết kế toán học này đảm bảo chỉ số phần trăm lớn nhất kết thúc tại $X = 1016px$, nằm lọt lòng hoàn toàn bên trong mép phải của bảng $1040px$ (`panelX + panelW`).

2. **Lưới 3 Cột Slot Lưu/Tải Thẳng Hàng Dọc (3-Column Aligned Slot Grid)**:
   Để khắc phục triệt để hiện tượng chữ điểm số tràn hoặc đè lên nút xóa màu đỏ `[X]`, giao diện Save/Load Dialog được chia thành 3 cột căn chỉnh tọa độ cố định:
   ```
   |---------------------------------------------------------------------------------------|
   |  > SLOT 1        07-27 09:51          SCORE: 99                      [ X ]            |
   |    (310px)       (460px)              (720px)                       (965px)           |
   |---------------------------------------------------------------------------------------|
   ```
   * **Cột 1 (Tên Slot - `X = 310px`)**: Hiển thị `SLOT 1` đến `SLOT 5` gióng thẳng hàng dọc (chiều rộng $108px$, kết thúc tại $418px$).
   * **Cột 2 (Ngày Giờ Rút Gọn - `X = 460px`)**: Hiển thị `07-27 09:51` (chiều rộng $198px$, kết thúc tại $658px$, tạo khoảng lề thoáng $42px$ với Cột 1 và $62px$ với Cột 3).
   * **Cột 3 (Điểm Số - `X = 720px`)**: Hiển thị `SCORE: 99` (chiều rộng $162px$, kết thúc tại $882px$).
   * **Nút Xóa Red `[X]` (`X = 965px`)**: Nút có kích thước $45 \times 35px$. Khoảng cách từ điểm kết thúc Cột 3 đến Nút Xóa là $965 - 882 = 83px$, xóa bỏ hoàn toàn nguy cơ chèn chữ hay va chạm đồ họa.

---

## 5. ĐÁNH GIÁ MỨC ĐỘ HOÀN THÀNH SO VỚI ĐỀ BÀI

### Bảng tổng hợp 6 Tiêu chí Chấm điểm của Giảng viên (Yêu cầu 4.1 ➔ 4.6)

| Trang Đề Bài | Tiêu Chí Chấm Điểm | Mức Độ Hoàn Thành & Chi Tiết Cài Đặt | Điểm Đánh Giá |
|---|---|---|---|
| **Trang 11** | **4.1 Cài đặt chạy đúng kịch bản (3.0đ)** | **HOÀN THÀNH 100%**<br>- Di chuyển W/A/S/D & Mũi tên mượt mà Single-Tap.<br>- Xử lý va chạm chớp đỏ màn hình + âm thanh `sfx_hit`.<br>- Màn hình GameOver hỏi chơi lại (`Y`) hoặc thoát (`N`/`ESC`).<br>- Chế độ Easy Mode cố định 1 map safe start & Infinite vô tận. | **3.0 / 3.0đ** |
| **Trang 11** | **4.2 Thực đơn Menu khởi đầu (1.0đ)** | **HOÀN THÀNH 100%**<br>- Menu chính phong cách SAO Aincrad Frosted Glass.<br>- Màn hình Chọn Nhân vật (Kirito/Asuna), Chọn chế độ (Easy Mode/Infinite) và Cài đặt âm thanh 3 cột. | **1.0 / 1.0đ** |
| **Trang 11-12**| **4.3 Xử lý Lưu/Tải trò chơi (3.0đ)** | **HOÀN THÀNH 100%**<br>- Hệ thống 5 Slot cố định Visual Novel (`slot1.txt` ➔ `slot5.txt`) trình bày lưới 3 cột thẳng hàng.<br>- Có Hộp thoại Xác nhận (Confirmation Modals) khi Save/Load/Delete.<br>- Khôi phục 100% vị trí nhân vật, hướng đi, điểm số và màn chơi. | **3.0 / 3.0đ** |
| **Trang 12** | **4.4 Xử lý Tạm dừng xe bằng Đèn giao thông (2.0đ)** | **HOÀN THÀNH 100%**<br>- Lớp `CTRAFFICLIGHT` quản lý đếm giờ Đỏ (3s) và Xanh (5s) có Lệch pha thời gian (Asynchronous Phase Shift).<br>- Tự động hãm dừng luồng xe `CVEHICLE` khi gặp đèn đỏ. | **2.0 / 2.0đ** |
| **Trang 12** | **4.5 Hiệu ứng khi va chạm & Âm thanh (0.5đ)** | **HOÀN THÀNH 100%**<br>- Màn hình chớp mờ đỏ va chạm 0.5s.<br>- Kênh âm thanh Sound Pool 4-track tách độc lập BGM & SFX. Nhạc nền lặp liên tục mượt mà. | **0.5 / 0.5đ** |
| **Trang 12** | **4.6 Giao diện đồ họa UI/UX (0.5đ)** | **HOÀN THÀNH 100%**<br>- Bố trí giao diện đồ họa SDL3 60FPS mượt mà.<br>- Đầy đủ sprite Kirito, Asuna, 4 loại quái Boss SAO, 2 loại xe và Đèn giao thông Neon. | **0.5 / 0.5đ** |
| **Trang 5** | **3.3 Kỹ thuật Đa tiểu trình (Điểm cộng nâng cao)** | **HOÀN THÀNH VƯỢT YÊU CẦU**<br>- Tách luồng vật lý `std::thread mPhysicsThread` 100Hz chạy độc lập với luồng render 60Hz.<br>- Bảo vệ vùng nhớ tuyệt đối bằng `std::mutex mGameMutex`. | **CỘNG ĐIỂM NÂNG CAO** |

---

## 6. HƯỚNG DẪN BIÊN DỊCH VÀ CHẠY DỰ ÁN

### 6.1 Cấu trúc thư mục mã nguồn

```
CrossingGame/
├── assets/                  # Tài nguyên trò chơi
│   ├── audio/               # Tệp âm thanh (bgm_menu.mp3, sfx_hit.mp3, sfx_jump.mp3)
│   └── images/              # Tệp hình ảnh kết cấu (characters, monsters, vehicles, ui)
├── docs/                    # Tài liệu báo cáo & Đề bài đồ án
│   ├── CrossingGame.md
│   ├── BAO_CAO_TIEN_DO.md
│   └── BAO_CAO_CHI_TIET_DO_AN.md
├── extern/                  # Thư viện liên kết ngoài (SDL3, SDL3_mixer)
├── saves/                   # Thư mục chứa 5 file lưu slot (slot1.txt .. slot5.txt)
├── src/                     # Mã nguồn C++
│   ├── include/             # Các tệp tiêu đề (.h)
│   │   ├── CANIMAL.h
│   │   ├── CBLUEWING.h
│   │   ├── CFont.h
│   │   ├── CGAME.h
│   │   ├── CGLEAMEYES.h
│   │   ├── CHEATHCLIFF.h
│   │   ├── CICEDRAGON.h
│   │   ├── CILLFANG.h
│   │   ├── CPEOPLE.h
│   │   ├── CSKYARMOR.h
│   │   ├── CTRAFFICLIGHT.h
│   │   └── CVEHICLE.h
│   └── source/              # Các tệp cài đặt (.cpp)
│       ├── CANIMAL.cpp
│       ├── CBLUEWING.cpp
│       ├── CFont.cpp
│       ├── CGAME.cpp
│       ├── CGLEAMEYES.cpp
│       ├── CHEATHCLIFF.cpp
│       ├── CICEDRAGON.cpp
│       ├── CILLFANG.cpp
│       ├── CPEOPLE.cpp
│       ├── CSKYARMOR.cpp
│       ├── CTRAFFICLIGHT.cpp
│       └── CVEHICLE.cpp
├── MainProg.cpp             # Tệp chứa hàm main() chạy ứng dụng
├── compile.bat              # Script biên dịch tự động MSBuild / Visual Studio
├── run.ps1                  # Script PowerShell khởi chạy nhanh
├── CrossingGame.sln         # Visual Studio Solution File
└── CrossingGame.vcxproj     # Visual Studio C++ Project File
```

### 6.2 Hướng dẫn Biên dịch tự động

#### Cách 1: Chạy script `compile.bat` (Khuyên dùng)
1. Mở cửa sổ Terminal / PowerShell tại thư mục gốc dự án.
2. Chạy lệnh:
   ```cmd
   compile.bat
   ```
3. Script sẽ tự động gọi MSBuild Visual Studio 2022, biên dịch toàn bộ file C++, copy các tệp DLL/assets và khởi chạy trò chơi ngay lập tức.

#### Cách 2: Mở bằng Visual Studio IDE
1. Mở file `CrossingGame.sln` bằng Visual Studio 2022.
2. Chọn cấu hình `Debug` hoặc `Release` (x64).
3. Nhấn `F5` để biên dịch và chạy game.
