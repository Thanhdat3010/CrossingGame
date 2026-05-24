#include "Game.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <iostream>
#include <cmath> // Dùng cho sin() tạo animation nhấp nháy

CGAME::CGAME() 
    : mWindow(nullptr), mRenderer(nullptr), mIsRunning(false), mState(GameState::MENU),
      mSwordTexture(nullptr),
      mSelectedMenuOption(0), mShowMenuWarning(false), mWarningTimer(0.0f),
      mMenuAnimTimer(0.0f) {}

CGAME::~CGAME() {
    exitGame();
}

bool CGAME::init(const char* title, int width, int height) {
    // 1. Khởi tạo SDL3
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // 2. Khởi tạo SDL3_mixer
    if (!MIX_Init()) {
        std::cerr << "MIX_Init Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // 3. Tạo cửa sổ game (Window - Fixed Size)
    mWindow = SDL_CreateWindow(title, width, height, 0);
    if (!mWindow) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        MIX_Quit();
        SDL_Quit();
        return false;
    }

    // 4. Tạo bộ vẽ (Renderer)
    mRenderer = SDL_CreateRenderer(mWindow, nullptr);
    if (!mRenderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(mWindow);
        MIX_Quit();
        SDL_Quit();
        return false;
    }

    // 5. Khởi tạo Bitmap Font vẽ chữ pixel
    if (!mFont.init(mRenderer)) {
        std::cerr << "Failed to initialize font!" << std::endl;
        SDL_DestroyRenderer(mRenderer);
        SDL_DestroyWindow(mWindow);
        MIX_Quit();
        SDL_Quit();
        return false;
    }

    // 6. Load ảnh hai thanh kiếm
    mSwordTexture = IMG_LoadTexture(mRenderer, "assets/swords.png");
    if (!mSwordTexture) {
        std::cerr << "Failed to load assets/swords.png! Error: " << SDL_GetError() << std::endl;
    }

    mIsRunning = true;
    mState = GameState::MENU; // Bắt đầu ở trạng thái Menu chính

    std::cout << "CGAME initialized successfully!" << std::endl;
    return true;
}

void CGAME::run() {
    Uint64 lastTime = SDL_GetTicks();

    while (mIsRunning) {
        // Tính toán deltaTime (khoảng thời gian trôi qua giữa 2 khung hình)
        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // Đổi sang giây
        lastTime = currentTime;

        // Giới hạn deltaTime tối đa để tránh giật lag quá mức (ví dụ khi kéo thả cửa sổ)
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }

        // 3 Bước cốt lõi của Game Loop
        handleInput();
        update(deltaTime);
        render();

        // Giới hạn tốc độ khung hình (FPS) ở mức ~60 FPS để tiết kiệm pin/CPU
        SDL_Delay(16); 
    }
}

void CGAME::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            mIsRunning = false;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN) {
            SDL_Keycode key = event.key.key;

            if (mState == GameState::MENU) {
                // Điều khiển Menu bằng phím di chuyển
                if (key == SDLK_W || key == SDLK_UP) {
                    mSelectedMenuOption = (mSelectedMenuOption - 1 + 3) % 3;
                }
                else if (key == SDLK_S || key == SDLK_DOWN) {
                    mSelectedMenuOption = (mSelectedMenuOption + 1) % 3;
                }
                else if (key == SDLK_RETURN || key == SDLK_SPACE) {
                    if (mSelectedMenuOption == 0) {
                        // Bắt đầu game
                        startGame();
                    } else {
                        // Hiển thị cảnh báo "Chưa hỗ trợ"
                        mShowMenuWarning = true;
                        mWarningTimer = 2.5f; // Hiện cảnh báo trong 2.5 giây
                    }
                }
            }
            else if (mState == GameState::PLAYING) {
                // Di chuyển nhân vật bằng W/A/S/D hoặc các phím mũi tên
                if (key == SDLK_W || key == SDLK_UP) {
                    mPlayer.Up(80); // Biên trên (vỉa hè về đích)
                }
                else if (key == SDLK_S || key == SDLK_DOWN) {
                    mPlayer.Down(650); // Biên dưới (vỉa hè xuất phát)
                }
                else if (key == SDLK_A || key == SDLK_LEFT) {
                    mPlayer.Left(0); // Biên trái
                }
                else if (key == SDLK_D || key == SDLK_RIGHT) {
                    mPlayer.Right(1280 - mPlayer.getWidth()); // Biên phải
                }
                else if (key == SDLK_ESCAPE) {
                    mState = GameState::MENU; // Nhấn ESC quay lại Menu
                }
            }
        }
    }
}

void CGAME::update(float deltaTime) {
    // Cập nhật bộ đếm animation menu (chạy liên tục cho hiệu ứng sin wave)
    mMenuAnimTimer += deltaTime;

    // Cập nhật bộ đếm thời gian cảnh báo của Menu
    if (mShowMenuWarning) {
        mWarningTimer -= deltaTime;
        if (mWarningTimer <= 0.0f) {
            mShowMenuWarning = false;
        }
    }

    // Logic khi chơi game
    if (mState == GameState::PLAYING) {
        // Kiểm tra nếu người chơi băng qua đường thành công (về đích)
        if (mPlayer.isFinish()) {
            // Tạm thời đưa về điểm xuất phát để chơi tiếp
            mPlayer.resetPosition();
        }
    }
}

// ====================================================================
// RENDER DISPATCHER — Quyết định vẽ gì dựa trên trạng thái game
// Flow: render() → gọi renderMenu() hoặc renderPlaying() tùy state
// ====================================================================
void CGAME::render() {
    if (mState == GameState::MENU) {
        renderMenu();
    }
    else if (mState == GameState::PLAYING) {
        renderPlaying();
    }
    // Đẩy toàn bộ nội dung đã vẽ lên màn hình
    SDL_RenderPresent(mRenderer);
}

// ====================================================================
// RENDER MENU — Giao diện Menu phong cách Anime SAO (vẽ 100% bằng code)
// ====================================================================
// Flow chi tiết:
//   1. Vẽ nền gradient (xanh đậm → đen) bằng nhiều dải rect mỏng xếp chồng
//   2. Vẽ các ngôi sao nhỏ nhấp nháy theo sin(time) → tạo cảm giác "sống"
//   3. Vẽ 2 thanh kiếm chéo nhau (pixel art) ở giữa → icon Crossing Game
//   4. Vẽ khung panel viền sáng phía dưới → chứa menu options
//   5. Vẽ tiêu đề + phụ đề phía trên panel
//   6. Vẽ menu items với hiệu ứng highlight khi được chọn
//   7. Vẽ hướng dẫn / cảnh báo ở cuối
// ====================================================================
void CGAME::renderMenu() {
    // ─── 1. NỀN GRADIENT (Deep Navy → Black) ───────────────────────
    // Vẽ từng dải ngang 4px, mỗi dải giảm dần độ sáng từ trên xuống dưới
    // Tạo hiệu ứng gradient mượt mà giống bầu trời đêm anime
    for (int y = 0; y < 720; y += 4) {
        float ratio = (float)y / 720.0f; // 0.0 (trên) → 1.0 (dưới)
        // Pha trộn từ Navy Blue (20,24,82) đến Midnight Black (5,5,15)
        Uint8 r = (Uint8)(20 - ratio * 15);
        Uint8 g = (Uint8)(24 - ratio * 19);
        Uint8 b = (Uint8)(82 - ratio * 67);
        SDL_SetRenderDrawColor(mRenderer, r, g, b, 255);
        SDL_FRect strip = { 0.0f, (float)y, 1280.0f, 4.0f };
        SDL_RenderFillRect(mRenderer, &strip);
    }

    // ─── 2. NGÔI SAO NHẤP NHÁY ─────────────────────────────────────
    // Mỗi ngôi sao là 1 pixel nhỏ, sáng/tối theo sin(time + offset)
    // offset khác nhau cho mỗi sao → nhấp nháy lệch pha rất tự nhiên
    struct Star { float x, y, speed; };
    Star stars[] = {
        {100, 50, 1.0f},  {300, 80, 1.5f},  {500, 30, 2.0f},
        {700, 90, 0.8f},  {900, 60, 1.2f},  {1100, 40, 1.8f},
        {150, 120, 2.5f}, {450, 100, 0.6f}, {750, 130, 1.4f},
        {1050, 110, 1.1f},{200, 170, 0.9f}, {600, 160, 2.2f},
        {1000, 150, 1.7f},{350, 190, 1.3f}, {850, 180, 0.7f},
        {50, 200, 2.0f},  {1200, 70, 1.6f}, {640, 45, 1.9f},
    };
    for (auto& s : stars) {
        // sin() trả về -1..1, đổi sang 0..255 để làm độ sáng alpha
        float brightness = (sinf(mMenuAnimTimer * s.speed * 3.14f) + 1.0f) / 2.0f;
        Uint8 alpha = (Uint8)(100 + brightness * 155); // Sáng tối từ 100-255
        SDL_SetRenderDrawColor(mRenderer, 200, 220, 255, alpha);
        // Vẽ ngôi sao 2x2 pixel (nhỏ xinh)
        SDL_FRect star = { s.x, s.y, 2.0f, 2.0f };
        SDL_RenderFillRect(mRenderer, &star);
    }

    // ─── 3. THANH KIẾM CHÉO (Pixel Art Icon) ───────────────────────
    // Ý nghĩa: 2 thanh kiếm chéo nhau tượng trưng cho "crossing" (băng qua)
    // Đồng thời là biểu tượng đặc trưng của Sword Art Online!
    int swordCenterX = 640; // Tâm X của cặp kiếm
    int swordCenterY = 240; // Tâm Y

    if (mSwordTexture) {
        // Vẽ texture từ ảnh swords.png của người dùng
        float w = 256.0f; // Kích thước chiều ngang hiển thị phù hợp
        float h = 256.0f; // Kích thước chiều cao hiển thị phù hợp
        SDL_FRect dstRect = { (float)swordCenterX - w / 2.0f, (float)swordCenterY - h / 2.0f, w, h };
        SDL_RenderTexture(mRenderer, mSwordTexture, NULL, &dstRect);
    } else {
        // Dự phòng: Vẽ kiếm bằng code nếu không load được file ảnh
        int swordLen = 80;      // Nửa chiều dài thanh kiếm

        // Thanh kiếm 1 — màu Cyan sáng (kiếm Elucidator style)
        SDL_SetRenderDrawColor(mRenderer, 80, 200, 255, 255);
        for (int i = -swordLen; i <= swordLen; i++) {
            // Vẽ pixel dọc theo đường chéo, dày 3px cho rõ
            SDL_FRect px = { (float)(swordCenterX + i - 1), (float)(swordCenterY + i - 1), 3.0f, 3.0f };
            SDL_RenderFillRect(mRenderer, &px);
        }
        // Chuôi kiếm 1 (hình vuông nhỏ ở góc trên-trái)
        SDL_FRect hilt1 = { (float)(swordCenterX - swordLen - 5), (float)(swordCenterY - swordLen - 5), 12.0f, 12.0f };
        SDL_SetRenderDrawColor(mRenderer, 255, 215, 0, 255); // Vàng Gold
        SDL_RenderFillRect(mRenderer, &hilt1);

        // Thanh kiếm 2 — màu Rose sáng (kiếm Lambent Light style)
        SDL_SetRenderDrawColor(mRenderer, 255, 130, 170, 255);
        for (int i = -swordLen; i <= swordLen; i++) {
            SDL_FRect px = { (float)(swordCenterX - i - 1), (float)(swordCenterY + i - 1), 3.0f, 3.0f };
            SDL_RenderFillRect(mRenderer, &px);
        }
        // Chuôi kiếm 2 (góc trên-phải)
        SDL_FRect hilt2 = { (float)(swordCenterX + swordLen - 5), (float)(swordCenterY - swordLen - 5), 12.0f, 12.0f };
        SDL_SetRenderDrawColor(mRenderer, 255, 215, 0, 255);
        SDL_RenderFillRect(mRenderer, &hilt2);
    }

    // Hiệu ứng phát sáng (glow) ở giao điểm 2 kiếm
    float glowPulse = (sinf(mMenuAnimTimer * 4.0f) + 1.0f) / 2.0f; // 0..1 nhấp nháy
    Uint8 glowAlpha = (Uint8)(80 + glowPulse * 120);
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 200, glowAlpha);
    SDL_FRect glow = { (float)(swordCenterX - 6), (float)(swordCenterY - 6), 12.0f, 12.0f };
    SDL_RenderFillRect(mRenderer, &glow);

    // ─── 4. TIÊU ĐỀ GAME ──────────────────────────────────────────
    // Chữ trắng sáng + phụ đề cyan nhạt, căn giữa phía trên panel
    SDL_Color titleColor = {255, 255, 255, 255};    // Trắng tinh khiết
    SDL_Color subtitleColor = {100, 200, 255, 255};  // Cyan nhạt SAO-style
    mFont.drawTextCentered(mRenderer, "CROSSING GAME", 350, 5, titleColor);
    mFont.drawTextCentered(mRenderer, "~ SWORD ART ONLINE EDITION ~", 400, 2, subtitleColor);

    // ─── 5. KHUNG PANEL MENU ───────────────────────────────────────
    // Vẽ một khung chữ nhật bán trong suốt → chứa các menu options bên trong
    // Tạo cảm giác "UI panel" trôi nổi như trong game SAO
    float panelX = 340.0f, panelY = 430.0f;
    float panelW = 600.0f, panelH = 230.0f;

    // Nền panel (xanh đậm bán trong suốt)
    SDL_SetRenderDrawColor(mRenderer, 10, 15, 40, 180);
    SDL_FRect panelBg = { panelX, panelY, panelW, panelH };
    SDL_RenderFillRect(mRenderer, &panelBg);

    // Viền panel 4 cạnh — Cyan sáng (tạo hiệu ứng hologram UI)
    SDL_SetRenderDrawColor(mRenderer, 80, 200, 255, 200);
    SDL_FRect borderTop    = { panelX, panelY, panelW, 2.0f };
    SDL_FRect borderBottom = { panelX, panelY + panelH - 2, panelW, 2.0f };
    SDL_FRect borderLeft   = { panelX, panelY, 2.0f, panelH };
    SDL_FRect borderRight  = { panelX + panelW - 2, panelY, 2.0f, panelH };
    SDL_RenderFillRect(mRenderer, &borderTop);
    SDL_RenderFillRect(mRenderer, &borderBottom);
    SDL_RenderFillRect(mRenderer, &borderLeft);
    SDL_RenderFillRect(mRenderer, &borderRight);

    // Góc trang trí (4 hình vuông nhỏ sáng ở 4 góc panel)
    SDL_SetRenderDrawColor(mRenderer, 80, 200, 255, 255);
    float cs = 6.0f; // Corner square size
    SDL_FRect c1 = { panelX - 2, panelY - 2, cs, cs };
    SDL_FRect c2 = { panelX + panelW - cs + 2, panelY - 2, cs, cs };
    SDL_FRect c3 = { panelX - 2, panelY + panelH - cs + 2, cs, cs };
    SDL_FRect c4 = { panelX + panelW - cs + 2, panelY + panelH - cs + 2, cs, cs };
    SDL_RenderFillRect(mRenderer, &c1);
    SDL_RenderFillRect(mRenderer, &c2);
    SDL_RenderFillRect(mRenderer, &c3);
    SDL_RenderFillRect(mRenderer, &c4);

    // ─── 6. MENU OPTIONS ───────────────────────────────────────────
    // 3 lựa chọn: NEW GAME / LOAD GAME / SETTINGS
    // Option được chọn có: highlight bar sáng + chữ vàng gold + chỉ thị ">"
    // Option thường: chữ xám nhạt
    SDL_Color normalColor  = {150, 160, 180, 255}; // Xám xanh nhạt
    SDL_Color selectColor  = {255, 215, 0, 255};   // Vàng Gold SAO-style

    std::string menuOptions[3] = {
        "NEW GAME",
        "LOAD GAME",
        "SETTINGS"
    };

    for (int i = 0; i < 3; ++i) {
        int yPos = 455 + i * 60;

        if (mSelectedMenuOption == i) {
            // Vẽ thanh highlight phía sau option được chọn
            SDL_SetRenderDrawColor(mRenderer, 80, 200, 255, 40);
            SDL_FRect highlight = { panelX + 10, (float)yPos - 5, panelW - 20, 40.0f };
            SDL_RenderFillRect(mRenderer, &highlight);

            // Viền trái highlight (vạch sáng 3px)
            SDL_SetRenderDrawColor(mRenderer, 80, 200, 255, 200);
            SDL_FRect hlBorder = { panelX + 10, (float)yPos - 5, 3.0f, 40.0f };
            SDL_RenderFillRect(mRenderer, &hlBorder);

            // Ký tự chỉ hướng ">" nhấp nháy theo sin(time)
            float arrowOffset = sinf(mMenuAnimTimer * 5.0f) * 5.0f;
            mFont.drawText(mRenderer, ">", (int)(panelX + 30 + arrowOffset), yPos + 4, 3, selectColor);
            mFont.drawText(mRenderer, menuOptions[i], (int)(panelX + 70), yPos + 4, 3, selectColor);
        } else {
            mFont.drawText(mRenderer, menuOptions[i], (int)(panelX + 70), yPos + 4, 3, normalColor);
        }
    }

    // ─── 7. HƯỚNG DẪN / CẢNH BÁO ──────────────────────────────────
    if (mShowMenuWarning) {
        SDL_Color warnColor = {255, 80, 80, 255}; // Đỏ sáng
        mFont.drawTextCentered(mRenderer, "NOT SUPPORTED YET!", 680, 2, warnColor);
    } else {
        SDL_Color guideColor = {80, 100, 120, 255};
        mFont.drawTextCentered(mRenderer, "W/S TO SELECT  -  ENTER TO CHOOSE", 685, 1, guideColor);
    }
}

// ====================================================================
// RENDER PLAYING — Giao diện khi đang chơi game
// ====================================================================
// Flow:
//   1. Xóa nền → trắng ngà
//   2. Vẽ vỉa hè trên/dưới → xanh lá nhạt
//   3. Vẽ vạch phân làn đường → xám nhạt đứt nét
//   4. Vẽ người chơi (CPEOPLE)
//   5. Vẽ HUD (thông tin stage, hướng dẫn ESC)
// ====================================================================
void CGAME::renderPlaying() {
    // Xóa nền với tông màu sáng dịu mắt (Light Theme - Cream White)
    SDL_SetRenderDrawColor(mRenderer, 245, 245, 240, 255);
    SDL_RenderClear(mRenderer);

    // 1. Vẽ Vỉa hè trên (Đích đến)
    SDL_FRect topSidewalk = { 0.0f, 0.0f, 1280.0f, 80.0f };
    SDL_SetRenderDrawColor(mRenderer, 200, 230, 201, 255);
    SDL_RenderFillRect(mRenderer, &topSidewalk);

    // 2. Vẽ Vỉa hè dưới (Xuất phát)
    SDL_FRect bottomSidewalk = { 0.0f, 640.0f, 1280.0f, 80.0f };
    SDL_SetRenderDrawColor(mRenderer, 200, 230, 201, 255);
    SDL_RenderFillRect(mRenderer, &bottomSidewalk);

    // 3. Vẽ 3 đường phân làn (vạch đứt)
    SDL_SetRenderDrawColor(mRenderer, 220, 220, 220, 255);
    for (int i = 1; i <= 3; ++i) {
        int yPos = 80 + i * 140;
        for (int x = 0; x < 1280; x += 40) {
            SDL_FRect laneLine = { (float)x, (float)yPos, 20.0f, 4.0f };
            SDL_RenderFillRect(mRenderer, &laneLine);
        }
    }

    // 4. Vẽ Người chơi
    mPlayer.draw(mRenderer, mFont);

    // 5. Vẽ HUD
    SDL_Color hudColor = {40, 44, 52, 255};
    mFont.drawText(mRenderer, "STAGE: 1", 20, 24, 2, hudColor);
    mFont.drawText(mRenderer, "PRESS ESC TO RETURN TO MENU", 880, 24, 2, hudColor);
}

void CGAME::startGame() {
    mState = GameState::PLAYING;
    resetGame();
}

void CGAME::resetGame() {
    mPlayer.resetPosition();
}

void CGAME::pauseGame() {
    if (mState == GameState::PLAYING) {
        mState = GameState::PAUSED;
    }
}

void CGAME::resumeGame() {
    if (mState == GameState::PAUSED) {
        mState = GameState::PLAYING;
    }
}

void CGAME::exitGame() {
    mIsRunning = false;
    
    if (mSwordTexture) {
        SDL_DestroyTexture(mSwordTexture);
        mSwordTexture = nullptr;
    }
    
    if (mRenderer) {
        SDL_DestroyRenderer(mRenderer);
        mRenderer = nullptr;
    }
    if (mWindow) {
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
    }
    
    MIX_Quit();
    SDL_Quit();
    std::cout << "CGAME resources cleaned up successfully!" << std::endl;
}
