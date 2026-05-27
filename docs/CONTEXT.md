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
- SDL3, build bang Visual Studio Build Tools (MSVC).

## Quy dinh nop bai (3 lan)
- Moi lan nop: 1 file .txt chua link Google Drive/OneDrive.
- Drive gom: report.pdf, slide.pptx, src.zip (code + DLL).
- Lan 3: them link YouTube trong file .txt.
- Code lam tren master. Khi du dieu kien nop lan 1/2/3 thi moi tao branch nop (user se bao).

## Muc tieu vuot yeu cau
- Lan 1: menu + 1 man choi duoc, co va cham, win/lose, len cap 1.
- Lan 2: them save/load, tam dung xe, HUD + FX + am thanh.
- Lan 3: hoan thien + dong goi chay tren may khac.

## Cau truc thu muc du kien (da tao)
- d:\Code\CrossingGame\extern\sdl3\include
- d:\Code\CrossingGame\extern\sdl3\lib
- d:\Code\CrossingGame\extern\sdl3\bin

## Huong dan dat file SDL3 vao dung cho
- Tai: SDL3-3.4.8-win32-x64.zip
- Copy thu muc include -> extern\sdl3\include
- Copy thu muc lib (hoac lib\x64) -> extern\sdl3\lib
- Copy SDL3.dll -> extern\sdl3\bin\SDL3.dll

## Luu y
- Neu can file ke hoach chi tiet, xem PROJECT_PLAN.md trong workspace goc.
- Tranh lay asset khong ro license; uu tien kenney.nl, itch.io (assets), opengameart.org (kiem tra license).

---

## TRANG THAI HIEN TAI (Cap nhat: 2026-05-27)

**Phase 1 DA HOAN THANH** - Da tao branch `dot-1` va push remote de nop bai lan 1.
- Branch hien tai: `dot-1` (code on dinh nop bai). Branch `master` la branch lam viec chinh.

---

## Nhung gi da lam duoc trong Phase 1 (tom tat tu cuoc chat)

### 1. Setup du an tu dau
- Tao project C++ voi CMake, cau hinh SDL3 (include, lib, bin).
- Tao script `run.ps1` (PowerShell) de tu dong cmake → build → copy DLL → chay game.
- Tao `compile.bat` backup.
- Tao `README.md` huong dan chay project tren GitHub.

### 2. Game Loop va cua so game
- Cua so 1280x720 pixel, fixed size.
- Vong lap chinh: `handleInput()` → `update(deltaTime)` → `render()`, 60 FPS (SDL_Delay).
- GameState machine: MENU → CHAR_SELECT → STAGE_SELECT → PLAYING → PAUSED → GAMEOVER.

### 3. He thong Menu (SAO Theme)
- 3 man hinh: Menu chinh → Chon nhan vat → Chon man choi.
- Menu: New Game / Load Game / Settings (Load va Settings hien thong bao "Chua ho tro").
- Chon nhan vat: Kirito hoac Asuna (co preview anh PNG).
- Chon man: Stage 1, 2, 3 va Infinity Mode.
- Trang tri: 2 thanh kiem (swords.png), hieu ung nhap nhay, animation menu.

### 4. He thong nhan vat (CPEOPLE)
- 2 nhan vat: Kirito (manh hon, cham hon) va Asuna (nhanh hon, nhe hon).
- Di chuyen theo luoi 80px bang W/A/S/D.
- Noi suy LERP (`mDrawX`, `mDrawY`) de di chuyen muot ma (khong bi giat).
- Hieu ung "nhip tho" (bob animation) khi dung yen.
- Hitbox co 20% moi ben (sprite 80x80 → hitbox ~48x48) de va cham cong bang.
- Load texture tu file PNG (kirito.png, asuna.png) voi fallback neu thieu anh.

### 5. He thong chuong ngai vat - Xe co (CVEHICLE → CTRUCK, CCAR)
- CVEHICLE la abstract class (pure virtual: Move, draw).
- CTRUCK: to hon, cham hon, co 2 frame hoat anh (truck1.png, truck2.png). Frame divisor = 90.
- CCAR: nho hon, nhanh hon, co 2 frame hoat anh (car1.png, car2.png). Frame divisor = 100.
- **Visual Scaling**: Ve to hon hitbox 1.4x-1.5x de bu vien trong suot cua anh AI tao, nhung hitbox van giu chuan 80px. Nhu vay nhin to ma va cham van cong bang.
- Tu dong lat anh (SDL_FLIP_HORIZONTAL) theo huong di chuyen.
- Khi di ra khoi man hinh thi wrap lai phia ben kia.

### 6. He thong chuong ngai vat - Dong vat (CANIMAL → CDINAUSOR, CBIRD)
- CANIMAL la abstract class (pure virtual: Move, Tell, draw).
- CDINAUSOR: chay tren dat (lane forest), 2 frame hoat anh (dino1.png, dino2.png). Frame divisor = 120 (cham, dam).
- CBIRD: bay tren troi (lane forest), 2 frame hoat anh vo canh (bird1.png, bird2.png). Frame divisor = 100.
- Visual Scaling 1.3x tuong tu xe co.
- **Quy tac anh**: Anh goc luon huong mat sang **PHAI**. Code tu dong flip khi `mDirection == -1`.

### 7. He thong ban do (Stage/Lane System)
- Cau truc lane tu tren xuong: sidewalk_top → forest (Dino) → road (Truck/Car) → rest lane → forest (Bird) → sidewalk_bottom.
- Moi lane co texture rieng (lane_road.png, lane_forest.png, lane_rest.png, sidewalk_top/bottom.png).
- Stage 1-3: tang so luong va toc do quai vat moi cap.
- **Infinity Mode**: tang do kho lien tuc moi khi win, khong co gioi han.
- **Dieu kien Win**: nhan vat dat `mY <= 40` (phai chan hoan toan len via he tren - sidewalk_top).

### 8. He thong va cham (AABB Collision)
- Va cham AABB giua `getHitbox()` cua nguoi choi va `getHitbox()` cua obstacle.
- Hitbox nguoi choi co 20% moi ben, obstacle co 15% moi ben → tranh "chet oan".
- Khi va cham: hien thong bao "Ban da chet! Nhan Y de choi lai..." (fallback text bitmap font).
- Nhan Y: reset vi tri nguoi choi, choi tiep. Phim khac: quay ve menu.

### 9. He thong do hoa va Fallback
- Uu tien load anh PNG tu `assets/` (dung SDL3_image).
- Neu thieu anh: game van chay duoc voi hinh fallback (SDL rect mau + text bitmap font).
- Background ve tung lane voi texture tuong ung, lap lai theo chieu ngang.
- Bitmap Font tu lam (CFont class) - ve text pixel khong can thu vien font ngoai.

### 10. Quan ly du an
- Gom cac file planning/docs vao thu muc `docs/` (CONTEXT.md, PROJECT_PLAN.md, SKILL.md, CrossingGame.md).
- Tao branch `dot-1` tren Git de luu trang thai nop bai lan 1, da push remote.
- README.md day du huong dan build va chay.

---

## Cac quyet dinh ky thuat quan trong (da thong nhat trong chat)

### Theme Sword Art Online
- Nhan vat la Kirito/Asuna thay vi nhan vat mac dinh.
- Quai vat (Truck/Car/Dino/Bird) thay cho xe co truyen thong, nhung van giu 4 loai voi 2 toc do (nhanh/cham) va 2 moi truong (road/forest).
- Background/lane duoc thiet ke theo phong cach SAO (rung, duong, via he).

### Hitbox tach biet Visual
- Sprite ve to (visual scaling 1.3x-1.5x) nhung hitbox giu nguyen 80px.
- Dieu nay giup nhin dep hon (anh AI thuong co vien trong suot lon) ma van va cham cong bang.
- Khi sua code lien quan va cham: **chi dung `getHitbox()`**, KHONG dung `getWidth()/getHeight()` truc tiep.

### Frame animation bang vi tri X
- 2 frame luan phien dua tren `abs(mX) / divisor % 2`.
- Divisor cang nho = chuyen frame cang nhanh. Gia tri hien tai: Dino=120, Truck=90, Car=100, Bird=100.
- Da duoc tinh chinh de toc do chuyen frame phu hop (khong qua nhanh, khong qua cham).

### Di chuyen luoi (Grid Movement)
- Nguoi choi nhay 80px moi buoc (toa do logic `mX`, `mY`).
- Toa do ve (`mDrawX`, `mDrawY`) noi suy LERP de muot ma.
- Toc do LERP: 10.0f * deltaTime.

### Muc 3.3 - Da luong (std::thread)
- **Chua lam** - da thao luan ky trong chat.
- Thay giao yeu cau dung `std::thread` (muc 3.3 trong de bai).
- KHONG duoc goi bat ky ham SDL Renderer/Texture tu thread phu (se crash driver do hoa).
- Phuong an da chot: dung thread phu cho tac vu nen **huu ich thuc su** (vd: dao dien am thanh moi truong, event ngau nhien). Thread phu chi set co/du lieu, Main Thread doc va ve.
- Phai co tac dung that, khong duoc de thread rong khong lam gi.

---

## Cau truc file hien tai

### Headers (include/)
| File | Class | Mo ta |
|------|-------|-------|
| Game.h | CGAME | Quan ly game loop, state machine, texture, obstacle lists |
| People.h | CPEOPLE | Nguoi choi, di chuyen, va cham, animation |
| Vehicle.h | CVEHICLE, CTRUCK, CCAR | Xe co/quai vat (abstract → concrete) |
| Animal.h | CANIMAL, CDINAUSOR, CBIRD | Dong vat (abstract → concrete) |
| Font.h | CFont | Bitmap font pixel rendering |

### Sources (src/)
| File | Size | Mo ta |
|------|------|-------|
| main.cpp | 392B | Entry point, tao CGAME va chay |
| Game.cpp | ~56KB | File lon nhat - chua toan bo logic game |
| People.cpp | ~9KB | Logic nguoi choi |
| Vehicle.cpp | ~8KB | Logic xe co |
| Animal.cpp | ~9KB | Logic dong vat |
| Font.cpp | ~9KB | Bitmap font rendering |

### Assets (assets/)
| File | Mo ta | Ghi chu |
|------|-------|---------|
| kirito.png | Nhan vat Kirito | Pixel art, nen trong suot |
| asuna.png | Nhan vat Asuna | Pixel art, nen trong suot |
| truck1.png, truck2.png | Truck frame 1 & 2 | 2 frame hoat anh |
| car1.png, car2.png | Car frame 1 & 2 | 2 frame hoat anh |
| dino1.png, dino2.png | Dino frame 1 & 2 | Huong phai, 2 frame |
| bird1.png, bird2.png | Bird frame 1 & 2 | Huong phai, vo canh |
| swords.png | Hai thanh kiem | Trang tri menu |
| lane_road.png | Background lane duong | Tile ngang |
| lane_forest.png | Background lane rung | Tile ngang |
| lane_rest.png | Background lane nghi | Tile ngang |
| sidewalk_top.png | Via he tren (dich den) | Dich den de win |
| sidewalk_bottom.png | Via he duoi (xuat phat) | Diem xuat phat |

---

## Con lai can lam (Phase 2 va Phase 3)

### Phase 2 - Nop bai lan 2
- [ ] Save/Load game: nhan L (save), T (load), tam dung va nhap duong dan file.
- [ ] CTRAFFICLIGHT: lop den tin hieu, dung xe/quai vat theo khoang thoi gian.
- [ ] HUD: hien thi thong tin (Stage, Lives, Score...) tren man hinh choi.
- [ ] Hieu ung va cham: animation khi va cham (nhap nhay, no...).
- [ ] Am thanh: nhac nen + sound effects (SDL3_mixer).
- [ ] Da luong (muc 3.3): dung `std::thread` cho tac vu nen huu ich (vd: am thanh moi truong, su kien ngau nhien).

### Phase 3 - Nop bai lan 3
- [ ] Hoan thien toan bo tinh nang.
- [ ] Dong goi chay tren may khac (zip voi DLL).
- [ ] Quay video YouTube demo.
- [ ] Report PDF + Slide PPTX.

---

## Luu y ky thuat cho AI tiep theo

1. **Main Thread only**: Moi thao tac `SDL_Renderer`, `SDL_Texture`, `SDL_CreateTextureFromSurface`... phai goi tu Main Thread. Thread phu chi duoc xu ly data/logic, KHONG ve.
2. **Visual Scaling**: Obstacle ve to hon hitbox (1.3x-1.5x). Va cham chi dung `getHitbox()`.
3. **Fallback rendering**: Thieu PNG → game van chay voi hinh ve SDL don gian (rect + text).
4. **Frame animation**: 2 frame luan phien theo `abs(mX) / divisor % 2`. Dino=120, Truck=90, Car=100, Bird=100.
5. **Grid movement**: Nguoi choi di 80px/buoc. Logic (`mX`,`mY`) nhay tuc thi, ve (`mDrawX`,`mDrawY`) LERP muot.
6. **File Game.cpp** rat lon (~56KB). Doc ky truoc khi chinh sua.
7. **Huong anh goc**: Asset quai vat huong **PHAI**. Code flip khi `mDirection == -1`.
8. **KHONG dung getch()** hay ham Console trong moi truong GUI SDL3.
9. **Theme sang**: Giao dien phai dung mau sang (yeu cau cua thay).

---

*Ghi chu: Doc `docs/CrossingGame.md` de xem de bai goc. Doc `docs/PROJECT_PLAN.md` de xem ke hoach tong the.*
