# Context - CrossingGame (tong hop yeu cau va cau truc)

## Tom tat yeu cau va quy dinh
- Game 2D pixel theo de bai CrossingGame (OOP).
- Giao vien chap nhan GUI 2D pixel.
- Thu vien giao dien phai co file .DLL.
- Theme/giao dien mau sang (khong dung theme toi).
- Logic theo de: nhan vat di bang W/A/S/D, va cham thi hoi Y de choi tiep, phim khac de thoat, ve dich thi len cap va tang do kho.
- Menu ban dau: New Game / Load Game / Settings (ban dau chi xu ly New Game, muc khac thong bao).
- Save/Load: nhan L (save) va T (load) khi dang choi, tam dung va nhap duong dan file.
- Xe co the dung theo khoang thoi gian (goi y CTRAFFICLIGHT).
- Co hieu ung va cham + am thanh thu (don gian).
- Giao dien bo tri thong tin hop ly, co hinh ve xe/thu/nguoi/den tin hieu.

## Cong nghe da chot
- SDL3, build bang Visual Studio Build Tools (MSVC) thong qua MSBuild.

## Quy dinh nop bai (3 lan)
- Moi lan nop: 1 file .txt chua link Google Drive/OneDrive.
- Drive gom: report.pdf, slide.pptx, src.zip (code + DLL).
- Lan 3: them link YouTube trong file .txt.
- Code lam tren master. Khi du dieu kien nop lan 1/2/3 thi moi tao branch nop (user se bao).

## Muc tieu vuot yeu cau
- Lan 1: menu + 1 man choi duoc, co va cham, win/lose, len cap 1.
- Lan 2: them save/load, tam dung xe, HUD + FX + am thanh.
- Lan 3: hoan thien + dong goi chay tren may khac.

## Cau truc thu muc hien tai (Da tai cau truc theo OOP)
- `d:\Code\CrossingGame\src` - Thu muc chua ma nguon chinh.
  - `src/MainProg.cpp` - Diem khoi chay chinh (Entry point).
  - `src/include/` - Thu muc chua toan bo cac tep tieu de (.h) cua tung Class rieng biet.
  - `src/source/` - Thu muc chua toan bo cac tep thuc thi (.cpp) cua tung Class rieng biet.
- `d:\Code\CrossingGame\extern\sdl3\include` - SDK cua SDL3.
- `d:\Code\CrossingGame\extern\sdl3\lib` - Library (.lib) cua SDL3.
- `d:\Code\CrossingGame\extern\sdl3\bin` - Dynamic Library (.dll) cua SDL3.

## Luu y
- Neu can file ke hoach chi tiet, xem PROJECT_PLAN.md trong workspace goc.
- Tranh lay asset khong ro license; uu tien kenney.nl, itch.io (assets), opengameart.org (kiem tra license).

---

## TRANG THAI HIEN TAI (Cap nhat: 2026-06-12)

**Phase 1 DA HOAN THANH VA TAI CAU TRUC OOP**
- Da dong bo va hoan tat tai cau truc thu muc theo dung quy tac OOP: **Moi class co 1 cap file .h/.cpp dat ten trung khop hoan toan voi ten class**.
- Da tao branch `dot-1` va push remote de nop bai lan 1, dong thoi loai bo cac thu muc tam (`build`, `.vs`) va file script trong branch nay theo yeu cau nop bai cua Giao vien.
- Nhánh lam viec chinh tiep theo: `master`.

---

## Nhung gi da lam duoc trong Phase 1 (tom tat)

### 1. Setup du an tu dau va Tai cau truc
- Cau hinh SDL3 va MSBuild, tich hop qua tệp project `.vcxproj` va `.vcxproj.filters`.
- Setup script `run.ps1` (PowerShell) va `compile.bat` de tu dong hoa trinh compile va chay game tren master.
- Tai cau truc toan bo file .h/.cpp theo dung ten lop vao `src/include` va `src/source`.

### 2. Game Loop va cua so game
- Cua so 1280x720 pixel, fixed size.
- Vong lap chinh: `handleInput()` → `update(deltaTime)` → `render()`, 60 FPS (SDL_Delay).
- GameState machine: MENU → CHAR_SELECT → STAGE_SELECT → PLAYING → PAUSED → GAMEOVER.

### 3. He thong Menu (SAO Theme)
- 3 man hinh: Menu chinh → Chon nhan vat → Chon man choi.
- Menu: New Game / Load Game / Settings (Load va Settings hien thong bao "Chua ho tro").
- Chon nhan vat: Kirito hoac Asuna (co preview anh PNG).
- Chon man: Stage 1 (Tutorial) va Chế độ vo tan (Infinite Mode).
- Trang tri: 2 thanh kiem (swords.png), hieu ung nhap nhay, hoat anh menu dac sac.

### 4. He thong nhan vat (CPEOPLE)
- 2 nhan vat: Kirito (HP cao, Speed nhanh 20) va Asuna (HP 100, Speed thuong 16).
- Di chuyen theo grid 80px bang W/A/S/D.
- Noi suy LERP (`mDrawX`, `mDrawY`) de di chuyen muot ma.
- Hitbox co 20% pad moi ben de va cham cong bang.
- Load texture tu file PNG (kirito.png, asuna.png).

### 5. He thong chuong ngai vat - Xe co (CVEHICLE → CBLUEWING, CSKYARMOR)
- CVEHICLE la abstract class (pure virtual: Move, draw).
- CBLUEWING: xe bay nhanh (assets/cbluewing.png).
- CSKYARMOR: xe bay cham (assets/cskyarmor.png).
- Visual Scaling 1.4x-1.5x de hop ly hoa hinh anh nhung hitbox van giu chuan 80px.
- Tu dong lat anh (flip ngang) theo huong di chuyen (anh mac dinh huong sang trai).

### 6. He thong chuong ngai vat - Dong vat (CANIMAL → CILLFANG, CICEDRAGON, CHEATHCLIFF, CGLEAMEYES)
- CANIMAL la abstract class (pure virtual: Move, draw).
- 4 loai Boss SAO ho tro di chuyen va hoat anh 2 frame tuong ung.
- Asset quai vat mac dinh huong mat sang phai. Code tu dong flip trai khi di nguoc chieu.

### 7. He thong ban do (Stage/Lane System)
- Cac lane texture lap lai: sidewalk_top → forest (quai vat) → road (xe bay) → rest lane → road → forest → sidewalk_bottom.
- **Infinity Mode**: sinh lane moi phia tren va tu dong xoa cac lane loi thoi ben duoi, camera cuon tu dong theo nguoi choi.

### 8. He thong va cham (AABB Collision)
- Va cham AABB dung hitbox cua nguoi choi va chuong ngai vat.
- Khi chet: hien thi red overlay "YOU DIED" va hoi Y de tiep tuc.

---

## Cau truc file sau khi tai cau truc OOP

### Headers (src/include/)
| File | Class | Mo ta |
|------|-------|-------|
| CGAME.h | CGAME | Quan ly game loop, state machine, texture, obstacle lists |
| CPEOPLE.h | CPEOPLE | Nguoi choi, di chuyen, va cham, animation |
| CVEHICLE.h | CVEHICLE | Lop cha xe co |
| CBLUEWING.h | CBLUEWING | Xe bay xanh |
| CSKYARMOR.h | CSKYARMOR | Giap bay bau troi |
| CANIMAL.h | CANIMAL | Lop cha quai vat/thu |
| CILLFANG.h | CILLFANG | Quai vat Dino Cillfang |
| CICEDRAGON.h | CICEDRAGON | Rong bang Icedragon |
| CHEATHCLIFF.h | CHEATHCLIFF | Heathcliff |
| CGLEAMEYES.h | CGLEAMEYES | Quy mat xanh Gleameyes |
| CFont.h | CFont | Bitmap font pixel rendering |

### Sources (src/source/ va src/)
- `src/MainProg.cpp` - Entry point chinh.
- Cac file implementation `.cpp` tuong ung nam trong `src/source/` nhu `CGAME.cpp`, `CPEOPLE.cpp`, `CFont.cpp`,...

---

## Con lai can lam (Phase 2 va Phase 3)

### Phase 2 - Nop bai lan 2
- [ ] Save/Load game: nhan L (save), T (load), tam dung va nhap duong dan file.
- [ ] CTRAFFICLIGHT: lop den tin hieu, dung xe/quai vat theo khoang thoi gian.
- [ ] HUD: hien thi thong tin (Stage, Lives, Score...) tren man hinh choi.
- [ ] Hieu ung va cham: animation khi va cham (nhap nhay, no...).
- [ ] Am thanh: nhac nen + sound effects (SDL3_mixer).
- [ ] Da luong (muc 3.3): dung `std::thread` cho tac vu nen (am thanh moi truong, su kien ngau nhien).

### Phase 3 - Nop bai lan 3
- [ ] Hoan thien toan bo tinh nang.
- [ ] Dong goi chay tren may khac (zip voi DLL).
- [ ] Quay video YouTube demo.
- [ ] Report PDF + Slide PPTX.

---

*Ghi chu: Doc `docs/CrossingGame.md` de xem de bai goc. Doc `docs/PROJECT_PLAN.md` de xem ke hoach tong the.*
