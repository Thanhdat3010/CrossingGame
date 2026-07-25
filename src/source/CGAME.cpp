#include "CGAME.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <iostream>
#include <cmath> // Dùng cho sin() tạo animation nhấp nháy

namespace {
    template <typename T>
    void moveObstacleList(std::vector<T*>& items, int limitX1, int limitX2) {
        for (auto* item : items) {
            item->Move(limitX1, limitX2);
        }
    }

    template <typename T>
    void drawObstacleList(std::vector<T*>& items, SDL_Renderer* renderer, CFont& font, float cameraY) {
        for (auto* item : items) {
            item->draw(renderer, font, cameraY);
        }
    }

    template <typename T>
    void clearObstacleList(std::vector<T*>& items) {
        for (auto* item : items) {
            delete item;
        }
        items.clear();
    }

    template <typename T>
    void pruneObstacleList(std::vector<T*>& items, float cameraY, float lowerBound, float upperBound) {
        for (size_t i = 0; i < items.size();) {
            float y = (float)items[i]->getY() - cameraY;
            if (y < lowerBound || y > upperBound) {
                delete items[i];
                items.erase(items.begin() + i);
            } else {
                ++i;
            }
        }
    }

    template <typename T>
    bool hitPlayerAgainstList(CPEOPLE& player, std::vector<T*>& items) {
        for (auto* item : items) {
            if (player.isImpact(item)) {
                return true;
            }
        }
        return false;
    }
}

namespace {
    MIX_Audio* loadAudioFlexible(MIX_Mixer* mixer, const std::string& basePath) {
        if (!mixer) return nullptr;
        const char* exts[] = { ".mp3", ".wav", ".ogg" };
        for (const char* ext : exts) {
            std::string fullPath = basePath + ext;
            MIX_Audio* a = MIX_LoadAudio(mixer, fullPath.c_str(), true);
            if (a) return a;
        }
        return nullptr;
    }

    void playBGM(MIX_Track* track, MIX_Audio* audio) {
        if (track && audio) {
            MIX_SetTrackAudio(track, audio);
            MIX_SetTrackLoops(track, -1);
            MIX_PlayTrack(track, 0);
        }
    }
}

CGAME::CGAME() 
    : mWindow(nullptr), mRenderer(nullptr), mIsRunning(false), mState(GameState::MENU),
      mSwordTexture(nullptr), 
      mCGleameyesTexture1(nullptr), mCGleameyesTexture2(nullptr),
      mCheathcliffTexture1(nullptr), mCheathcliffTexture2(nullptr),
      mCillfangTexture1(nullptr), mCillfangTexture2(nullptr),
      mCicedragonTexture1(nullptr), mCicedragonTexture2(nullptr),
      mBgMenuTexture(nullptr), mBgPlayingTexture(nullptr), mSidewalkTopTexture(nullptr), mSidewalkBottomTexture(nullptr),
      mLaneRestTexture(nullptr), mLaneForestTexture(nullptr), mLaneRoadTexture(nullptr),
      mCbluewingTexture(nullptr), mCskyarmorTexture(nullptr),
      mStage(1), mIsInfinityMode(false),
      mCameraY(0.0f), mLaneHeight(80), mInfiniteLevel(1), mLanePatternIndex(0),
      mSelectedMenuOption(0), mSelectedCharOption(0), mSelectedStageOption(0), mSelectedSettingsOption(0),
      mScore(0), mMaxReachedY(0),
      mShowMenuWarning(false), mWarningTimer(0.0f), mMenuAnimTimer(0.0f),
      mMixer(nullptr), mBgmTrack(nullptr), mBgmMenu(nullptr),
      mSfxHit(nullptr), mSfxJump(nullptr), mAudioMuted(false), mSfxMuted(false),
      mFlashTimer(0.0f), mIsThreadRunning(false) {}

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

    mMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

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

    // 6. Load âm thanh từ thư mục assets/audio/ (.mp3, .wav, .ogg)
    if (mMixer) {
        mBgmTrack = MIX_CreateTrack(mMixer);
        mBgmMenu = loadAudioFlexible(mMixer, "assets/audio/bgm_menu");
        mSfxHit = loadAudioFlexible(mMixer, "assets/audio/sfx_hit");
        mSfxJump = loadAudioFlexible(mMixer, "assets/audio/sfx_jump");

        if (mBgmMenu && mBgmTrack) {
            playBGM(mBgmTrack, mBgmMenu);
        }
    }

    // 6. Load ảnh hai thanh kiếm
    mSwordTexture = IMG_LoadTexture(mRenderer, "assets/images/ui/swords.png");
    if (!mSwordTexture) {
        std::cerr << "Failed to load assets/images/ui/swords.png! Error: " << SDL_GetError() << std::endl;
    }

    // Load các ảnh PNG tùy chọn cho bản đồ & quái vật (SAO theme)
    // 1. CGLEAMEYES
    mCGleameyesTexture1 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cgleameyes1.png");
    mCGleameyesTexture2 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cgleameyes2.png");
    if (!mCGleameyesTexture2) mCGleameyesTexture2 = mCGleameyesTexture1;

    // 2. CHEATHCLIFF
    mCheathcliffTexture1 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cheathcliff1.png");
    mCheathcliffTexture2 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cheathcliff2.png");
    if (!mCheathcliffTexture2) mCheathcliffTexture2 = mCheathcliffTexture1;

    // 3. CILLFANG
    mCillfangTexture1 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cillfang1.png");
    mCillfangTexture2 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cillfang2.png");
    if (!mCillfangTexture2) mCillfangTexture2 = mCillfangTexture1;

    // 4. CICEDRAGON
    mCicedragonTexture1 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cicedragon1.png");
    mCicedragonTexture2 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cicedragon2.png");
    if (!mCicedragonTexture2) mCicedragonTexture2 = mCicedragonTexture1;
    mBgMenuTexture = IMG_LoadTexture(mRenderer, "assets/images/ui/bg_menu.png");
    mBgPlayingTexture = IMG_LoadTexture(mRenderer, "assets/images/ui/bg_playing.png");
    mSidewalkTopTexture = IMG_LoadTexture(mRenderer, "assets/images/environment/sidewalk_top.png");
    mSidewalkBottomTexture = IMG_LoadTexture(mRenderer, "assets/images/environment/sidewalk_bottom.png");
    mLaneRestTexture = IMG_LoadTexture(mRenderer, "assets/images/environment/lane_rest.png");
    mLaneForestTexture = IMG_LoadTexture(mRenderer, "assets/images/environment/lane_forest.png");
    mLaneRoadTexture = IMG_LoadTexture(mRenderer, "assets/images/environment/lane_road.png");
    mCbluewingTexture = IMG_LoadTexture(mRenderer, "assets/images/vehicles/cbluewing.png");
    mCskyarmorTexture = IMG_LoadTexture(mRenderer, "assets/images/vehicles/cskyarmor.png");

    // Load ảnh các nhân vật Kirito & Asuna
    mPlayer.loadTextures(mRenderer);

    mIsRunning = true;
    mIsThreadRunning = true;
    mPhysicsThread = std::thread(&CGAME::physicsWorkerFunc, this);
    mState = GameState::MENU; // Bắt đầu ở trạng thái Menu chính

    srand((unsigned)SDL_GetTicks());

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
                        startGame();
                    } else if (mSelectedMenuOption == 1) {
                        mShowMenuWarning = true;
                        mWarningTimer = 2.5f;
                    } else if (mSelectedMenuOption == 2) {
                        mState = GameState::SETTINGS;
                    }
                }
            }
            else if (mState == GameState::CHAR_SELECT) {
                // Dùng A/D hoặc Trái/Phải để chọn Kirito/Asuna dạng 2 cột
                if (key == SDLK_A || key == SDLK_LEFT || key == SDLK_D || key == SDLK_RIGHT) {
                    mSelectedCharOption = (mSelectedCharOption == 0) ? 1 : 0;
                }
                else if (key == SDLK_RETURN || key == SDLK_SPACE) {
                    // Áp dụng nhân vật được chọn và chỉ số (Speed, Hitbox)
                    mPlayer.setCharacter(mSelectedCharOption == 0 ? CPEOPLE::CharacterType::KIRITO : CPEOPLE::CharacterType::ASUNA);
                    mState = GameState::STAGE_SELECT; // Chuyển sang màn hình chọn Stage
                }
                else if (key == SDLK_ESCAPE) {
                    mState = GameState::MENU; // Quay lại menu chính
                }
            }
            else if (mState == GameState::STAGE_SELECT) {
                // Dùng W/S hoặc Lên/Xuống để chọn chế độ
                if (key == SDLK_W || key == SDLK_UP) {
                    mSelectedStageOption = (mSelectedStageOption - 1 + 2) % 2;
                }
                else if (key == SDLK_S || key == SDLK_DOWN) {
                    mSelectedStageOption = (mSelectedStageOption + 1) % 2;
                }
                else if (key == SDLK_RETURN || key == SDLK_SPACE) {
                    if (mSelectedStageOption == 0) {
                        mIsInfinityMode = false; // Tutorial
                    } else {
                        mIsInfinityMode = true; // Infinite
                    }
                    resetGame();
                    mState = GameState::PLAYING;
                }
                else if (key == SDLK_ESCAPE) {
                    mState = GameState::CHAR_SELECT;
                }
            }
            else if (mState == GameState::SETTINGS) {
                if (key == SDLK_W || key == SDLK_UP) {
                    mSelectedSettingsOption = (mSelectedSettingsOption - 1 + 2) % 2;
                }
                else if (key == SDLK_S || key == SDLK_DOWN) {
                    mSelectedSettingsOption = (mSelectedSettingsOption + 1) % 2;
                }
                else if (key == SDLK_RETURN || key == SDLK_SPACE || key == SDLK_A || key == SDLK_D || key == SDLK_LEFT || key == SDLK_RIGHT) {
                    if (mSelectedSettingsOption == 0) {
                        toggleMusic();
                    } else {
                        toggleSfx();
                    }
                }
                else if (key == SDLK_ESCAPE) {
                    mState = GameState::MENU;
                }
            }
            else if (mState == GameState::PLAYING) {
                std::lock_guard<std::mutex> lock(mGameMutex);
                int topLimit = 40;
                int bottomLimit = 600;
                if (mIsInfinityMode) {
                    topLimit = (int)mCameraY + 40;
                    bottomLimit = (int)mCameraY + 600;
                }

                bool moved = false;
                // Bắt buộc bấm nhả từng phím (không cho đè phím để di chuyển liên tục)
                if (!event.key.repeat) {
                    if (key == SDLK_W || key == SDLK_UP) {
                        mPlayer.Up(topLimit);
                        moved = true;
                    }
                    else if (key == SDLK_S || key == SDLK_DOWN) {
                        mPlayer.Down(bottomLimit);
                        moved = true;
                    }
                    else if (key == SDLK_A || key == SDLK_LEFT) {
                        mPlayer.Left(0); // Biên trái
                        moved = true;
                    }
                    else if (key == SDLK_D || key == SDLK_RIGHT) {
                        mPlayer.Right(1280 - mPlayer.getWidth()); // Biên phải
                        moved = true;
                    }
                }

                if (moved) {
                    if (!mSfxMuted && mMixer && mSfxJump) {
                        MIX_PlayAudio(mMixer, mSfxJump);
                    }
                }

                if (key == SDLK_ESCAPE) {
                    mState = GameState::MENU;
                }
            }
            else if (mState == GameState::GAMEOVER) {
                // Nhấn Y để chơi tiếp, ESC hoặc N quay lại Menu
                if (key == SDLK_Y) {
                    if (mPlayer.isDead()) {
                        resetGame();
                        mState = GameState::PLAYING;
                    } else {
                        mStage = 1;
                        resetGame();
                        mState = GameState::PLAYING;
                    }
                }
                else if (key == SDLK_ESCAPE || key == SDLK_N) {
                    mState = GameState::MENU;
                }
            }
        }
    }
}

void CGAME::physicsWorkerFunc() {
    Uint64 lastTick = SDL_GetTicks();
    while (mIsThreadRunning) {
        Uint64 currentTick = SDL_GetTicks();
        float deltaTime = (currentTick - lastTick) / 1000.0f;
        if (deltaTime > 0.1f) deltaTime = 0.1f;
        lastTick = currentTick;

        if (mState == GameState::PLAYING) {
            std::lock_guard<std::mutex> lock(mGameMutex);

            // 1. Cập nhật Đèn giao thông
            for (auto& light : mTrafficLights) {
                light.update(deltaTime);
            }

            if (mIsInfinityMode) {
                updateInfinite(deltaTime);
            } else {
                mPlayer.update(deltaTime);

                bool road1Red = false;
                bool road2Red = false;
                for (const auto& light : mTrafficLights) {
                    if (light.getLaneY() == 120 && light.isRed()) road1Red = true;
                    if (light.getLaneY() == 440 && light.isRed()) road2Red = true;
                }

                // Di chuyển quái vật
                moveObstacleList(mGleameyes, 0, 1280);
                moveObstacleList(mCheathcliffs, 0, 1280);
                moveObstacleList(mCillfangs, 0, 1280);
                moveObstacleList(mCicedragons, 0, 1280);

                // Di chuyển xe nếu đèn không đỏ
                for (auto bw : mBluewings) {
                    if (bw->getY() == 120 && road1Red) continue;
                    if (bw->getY() == 440 && road2Red) continue;
                    bw->Move(0, 1280);
                }
                for (auto sa : mSkyarmors) {
                    if (sa->getY() == 120 && road1Red) continue;
                    if (sa->getY() == 440 && road2Red) continue;
                    sa->Move(0, 1280);
                }

                if (mPlayer.isFinish()) {
                    mState = GameState::GAMEOVER;
                }
                else if (hitPlayerAgainstList(mPlayer, mGleameyes) ||
                    hitPlayerAgainstList(mPlayer, mCheathcliffs) ||
                    hitPlayerAgainstList(mPlayer, mCillfangs) ||
                    hitPlayerAgainstList(mPlayer, mCicedragons) ||
                    hitPlayerAgainstList(mPlayer, mBluewings) ||
                    hitPlayerAgainstList(mPlayer, mSkyarmors)) {
                    mPlayer.setDead(true);
                    mState = GameState::GAMEOVER;
                    mFlashTimer = 0.5f;
                    if (!mSfxMuted && mMixer && mSfxHit) MIX_PlayAudio(mMixer, mSfxHit);
                }
            }
        }

        SDL_Delay(10); // Tick rate 100Hz
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

    if (mFlashTimer > 0.0f) {
        mFlashTimer -= deltaTime;
        if (mFlashTimer < 0.0f) mFlashTimer = 0.0f;
    }
}

void CGAME::render() {
    // Xóa màn hình trước khi vẽ khung hình mới để tránh rác đồ họa (screen trails)
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
    SDL_RenderClear(mRenderer);

    if (mState == GameState::MENU) {
        renderMenu();
    }
    else if (mState == GameState::CHAR_SELECT) {
        renderCharSelect();
    }
    else if (mState == GameState::STAGE_SELECT) {
        renderStageSelect();
    }
    else if (mState == GameState::SETTINGS) {
        renderSettings();
    }
    else if (mState == GameState::PLAYING || mState == GameState::GAMEOVER) {
        renderPlaying();

        // Nếu ở Game Over state
        if (mState == GameState::GAMEOVER) {
            if (mPlayer.isDead()) {
                // TRƯỜNG HỢP 1: THẤT BẠI - Bị tông chết
                // Nền overlay đỏ tối
                SDL_SetRenderDrawColor(mRenderer, 150, 20, 20, 190);
                SDL_FRect overlay = { 340.0f, 200.0f, 600.0f, 300.0f };
                SDL_RenderFillRect(mRenderer, &overlay);

                // Viền sáng đỏ tươi
                SDL_SetRenderDrawColor(mRenderer, 230, 57, 70, 255);
                SDL_FRect oB1 = { 340.0f, 200.0f, 600.0f, 4.0f };
                SDL_FRect oB2 = { 340.0f, 496.0f, 600.0f, 4.0f };
                SDL_FRect oB3 = { 340.0f, 200.0f, 4.0f, 300.0f };
                SDL_FRect oB4 = { 936.0f, 200.0f, 4.0f, 300.0f };
                SDL_RenderFillRect(mRenderer, &oB1);
                SDL_RenderFillRect(mRenderer, &oB2);
                SDL_RenderFillRect(mRenderer, &oB3);
                SDL_RenderFillRect(mRenderer, &oB4);

                // Chữ thông báo
                SDL_Color whiteColor = {255, 255, 255, 255};
                SDL_Color goldColor = {255, 215, 0, 255};
                mFont.drawTextCentered(mRenderer, "YOU DIED", 240, 5, goldColor);
                if (mIsInfinityMode) {
                    mFont.drawTextCentered(mRenderer, "SCORE: " + std::to_string(mScore), 320, 2, whiteColor);
                } else {
                    mFont.drawTextCentered(mRenderer, "STAGE REACHED: " + std::to_string(mStage), 320, 2, whiteColor);
                }
                mFont.drawTextCentered(mRenderer, "PRESS 'Y' TO CONTINUE", 390, 2, whiteColor);
                mFont.drawTextCentered(mRenderer, "PRESS 'ESC' TO GO TO MENU", 440, 1, {180, 180, 180, 255});
            } else {
                // TRƯỜNG HỢP 2: CHIẾN THẮNG - Về đích Stage 3 thành công!
                // Nền overlay xanh lục bảo chiến thắng
                SDL_SetRenderDrawColor(mRenderer, 15, 45, 25, 210);
                SDL_FRect overlay = { 340.0f, 200.0f, 600.0f, 300.0f };
                SDL_RenderFillRect(mRenderer, &overlay);

                // Viền sáng vàng kim Gold
                SDL_SetRenderDrawColor(mRenderer, 255, 215, 0, 255);
                SDL_FRect oB1 = { 340.0f, 200.0f, 600.0f, 4.0f };
                SDL_FRect oB2 = { 340.0f, 496.0f, 600.0f, 4.0f };
                SDL_FRect oB3 = { 340.0f, 200.0f, 4.0f, 300.0f };
                SDL_FRect oB4 = { 936.0f, 200.0f, 4.0f, 300.0f };
                SDL_RenderFillRect(mRenderer, &oB1);
                SDL_RenderFillRect(mRenderer, &oB2);
                SDL_RenderFillRect(mRenderer, &oB3);
                SDL_RenderFillRect(mRenderer, &oB4);

                // Chữ thông báo chiến thắng
                SDL_Color whiteColor = {255, 255, 255, 255};
                SDL_Color goldColor = {255, 215, 0, 255};
                mFont.drawTextCentered(mRenderer, "VICTORY!", 240, 5, goldColor);
                mFont.drawTextCentered(mRenderer, "CONGRATULATIONS, HERO!", 310, 2, whiteColor);
                mFont.drawTextCentered(mRenderer, "YOU HAVE BEATEN THE CAMPAIGN!", 350, 2, whiteColor);
                mFont.drawTextCentered(mRenderer, "PRESS 'Y' TO PLAY AGAIN", 410, 2, goldColor);
                mFont.drawTextCentered(mRenderer, "PRESS 'ESC' TO GO TO MENU", 450, 1, {180, 180, 180, 255});
            }
        }
    }
    // Đẩy toàn bộ nội dung đã vẽ lên màn hình
    SDL_RenderPresent(mRenderer);
}

void CGAME::renderMenuBackground() {
    if (mBgMenuTexture) {
        SDL_FRect bgRect = { 0.0f, 0.0f, 1280.0f, 720.0f };
        SDL_RenderTexture(mRenderer, mBgMenuTexture, NULL, &bgRect);
    } else {
        // GIAO DIỆN MÀU SÁNG BAN NGÀY (Light Mode Sky Blue Gradient Fallback)
        for (int y = 0; y < 720; y += 4) {
            float ratio = (float)y / 720.0f;
            Uint8 r = (Uint8)(175 + ratio * 65);  // 175 -> 240
            Uint8 g = (Uint8)(220 + ratio * 30);  // 220 -> 250
            Uint8 b = 255;                        // 255
            SDL_SetRenderDrawColor(mRenderer, r, g, b, 255);
            SDL_FRect strip = { 0.0f, (float)y, 1280.0f, 4.0f };
            SDL_RenderFillRect(mRenderer, &strip);
        }
    }
}

void CGAME::renderMenu() {
    renderMenuBackground();

    // ─── TIÊU ĐỀ GAME ──────────────────────────────────────────────
    SDL_Color titleShadow = {0, 0, 0, 80};
    SDL_Color titleColor  = {255, 255, 255, 255};
    SDL_Color subtitleColor = {220, 245, 255, 255};
    mFont.drawTextCentered(mRenderer, "CROSSING GAME", 162, 5, titleShadow);
    mFont.drawTextCentered(mRenderer, "CROSSING GAME", 160, 5, titleColor);
    mFont.drawTextCentered(mRenderer, "~ SWORD ART ONLINE EDITION ~", 215, 2, subtitleColor);

    // ─── KHUNG PANEL MENU (FROSTED GLASS) ──────────────────────────
    float panelX = 340.0f, panelY = 380.0f;
    float panelW = 600.0f, panelH = 260.0f;

    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 160);
    SDL_FRect panelBg = { panelX, panelY, panelW, panelH };
    SDL_RenderFillRect(mRenderer, &panelBg);

    SDL_SetRenderDrawColor(mRenderer, 20, 100, 80, 200);
    SDL_FRect borderTop    = { panelX, panelY, panelW, 3.0f };
    SDL_FRect borderBottom = { panelX, panelY + panelH - 3, panelW, 3.0f };
    SDL_FRect borderLeft   = { panelX, panelY, 3.0f, panelH };
    SDL_FRect borderRight  = { panelX + panelW - 3, panelY, 3.0f, panelH };
    SDL_RenderFillRect(mRenderer, &borderTop);
    SDL_RenderFillRect(mRenderer, &borderBottom);
    SDL_RenderFillRect(mRenderer, &borderLeft);
    SDL_RenderFillRect(mRenderer, &borderRight);

    // ─── MENU OPTIONS ──────────────────────────────────────────────
    SDL_Color normalColor  = {30, 60, 50, 255};
    SDL_Color selectColor  = {10, 95, 75, 255};

    std::string menuOptions[3] = {
        "NEW GAME",
        "LOAD GAME",
        "SETTINGS"
    };

    for (int i = 0; i < 3; ++i) {
        int yPos = 410 + i * 65;

        if (mSelectedMenuOption == i) {
            SDL_SetRenderDrawColor(mRenderer, 20, 120, 100, 60);
            SDL_FRect highlight = { panelX + 10, (float)yPos - 5, panelW - 20, 45.0f };
            SDL_RenderFillRect(mRenderer, &highlight);

            SDL_SetRenderDrawColor(mRenderer, 20, 140, 100, 255);
            SDL_FRect hlBorder = { panelX + 10, (float)yPos - 5, 4.0f, 45.0f };
            SDL_RenderFillRect(mRenderer, &hlBorder);

            float arrowOffset = sinf(mMenuAnimTimer * 5.0f) * 5.0f;
            mFont.drawText(mRenderer, ">", (int)(panelX + 30 + arrowOffset), yPos + 8, 3, selectColor);
            mFont.drawText(mRenderer, menuOptions[i], (int)(panelX + 70), yPos + 8, 3, selectColor);
        } else {
            mFont.drawText(mRenderer, menuOptions[i], (int)(panelX + 70), yPos + 8, 3, normalColor);
        }
    }

    // ─── HƯỚNG DẪN ─────────────────────────────────────────────────
    if (mShowMenuWarning) {
        SDL_Color warnColor = {200, 30, 30, 255};
        mFont.drawTextCentered(mRenderer, "NOT SUPPORTED YET!", 660, 2, warnColor);
    } else {
        SDL_Color guideColor = {255, 255, 255, 200};
        mFont.drawTextCentered(mRenderer, "W/S TO SELECT  -  ENTER TO CHOOSE", 665, 1, guideColor);
    }
}

void CGAME::renderCharSelect() {
    renderMenuBackground();

    SDL_Color titleShadow = {0, 0, 0, 80};
    SDL_Color titleColor = {255, 255, 255, 255};
    SDL_Color subtitleColor = {220, 245, 255, 255};
    mFont.drawTextCentered(mRenderer, "SELECT YOUR HERO", 68, 4, titleShadow);
    mFont.drawTextCentered(mRenderer, "SELECT YOUR HERO", 66, 4, titleColor);
    mFont.drawTextCentered(mRenderer, "SWORD ART ONLINE CHARACTER SELECTION", 118, 2, subtitleColor);

    auto drawTextCenteredInBox = [&](const std::string& text, float boxX, float boxW, float y, int scale, SDL_Color color) {
        int textWidth = (int)text.length() * (8 + 1) * scale - scale;
        int targetX = (int)boxX + ((int)boxW - textWidth) / 2;
        mFont.drawText(mRenderer, text, targetX, (int)y, scale, color);
    };

    SDL_Color cyanColor = {0, 140, 200, 255};
    SDL_Color roseColor = {200, 60, 100, 255};
    SDL_Color labelColor = {40, 60, 55, 255};
    SDL_Color valColor = {30, 50, 45, 255};
    bool kSelected = (mSelectedCharOption == 0);

    SDL_Texture* kTex = mPlayer.getKiritoTexture();
    SDL_Texture* aTex = mPlayer.getAsunaTexture();

    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);

    // KIRITO
    if (kSelected) {
        float cX = 465.0f, cY = 170.0f, cW = 350.0f, cH = 420.0f;
        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 180);
        SDL_FRect panel = { cX, cY, cW, cH };
        SDL_RenderFillRect(mRenderer, &panel);

        float borderPulse = (sinf(mMenuAnimTimer * 4.0f) + 1.0f) / 2.0f;
        Uint8 borderAlpha = (Uint8)(180 + borderPulse * 75);
        SDL_SetRenderDrawColor(mRenderer, 20, 120, 100, borderAlpha);
        SDL_FRect b1 = { cX, cY, cW, 3.0f };
        SDL_FRect b2 = { cX, cY + cH - 3, cW, 3.0f };
        SDL_FRect b3 = { cX, cY, 3.0f, cH };
        SDL_FRect b4 = { cX + cW - 3, cY, 3.0f, cH };
        SDL_RenderFillRect(mRenderer, &b1);
        SDL_RenderFillRect(mRenderer, &b2);
        SDL_RenderFillRect(mRenderer, &b3);
        SDL_RenderFillRect(mRenderer, &b4);

        if (kTex) {
            SDL_FRect dstRect = { cX + (cW - 120.0f) / 2.0f, cY + 30.0f, 120.0f, 120.0f };
            SDL_RenderTexture(mRenderer, kTex, NULL, &dstRect);
        } else {
            drawTextCenteredInBox("K", cX, cW, cY + 45, 6, cyanColor);
        }
        
        drawTextCenteredInBox("KIRITO", cX, cW, cY + 180, 3, cyanColor);
        drawTextCenteredInBox("CHARACTER SKIN: KIRITO", cX, cW, cY + 240, 1, valColor);
        drawTextCenteredInBox("THE BLACK SWORDSMAN", cX, cW, cY + 275, 1, labelColor);
        drawTextCenteredInBox(">> ACTIVE HERO SKIN <<", cX, cW, cY + 350, 2, cyanColor);
    } else {
        float cX = 160.0f, cY = 230.0f, cW = 220.0f, cH = 300.0f;
        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 100);
        SDL_FRect panel = { cX, cY, cW, cH };
        SDL_RenderFillRect(mRenderer, &panel);

        SDL_SetRenderDrawColor(mRenderer, 80, 130, 120, 100);
        SDL_FRect b1 = { cX, cY, cW, 2.0f };
        SDL_FRect b2 = { cX, cY + cH - 2, cW, 2.0f };
        SDL_FRect b3 = { cX, cY, 2.0f, cH };
        SDL_FRect b4 = { cX + cW - 2, cY, 2.0f, cH };
        SDL_RenderFillRect(mRenderer, &b1);
        SDL_RenderFillRect(mRenderer, &b2);
        SDL_RenderFillRect(mRenderer, &b3);
        SDL_RenderFillRect(mRenderer, &b4);

        if (kTex) {
            SDL_FRect dstRect = { cX + (cW - 80.0f) / 2.0f, cY + 25.0f, 80.0f, 80.0f };
            SDL_SetTextureAlphaMod(kTex, 120);
            SDL_RenderTexture(mRenderer, kTex, NULL, &dstRect);
            SDL_SetTextureAlphaMod(kTex, 255);
        } else {
            drawTextCenteredInBox("K", cX, cW, cY + 35, 4, SDL_Color{0, 140, 200, 120});
        }
        
        drawTextCenteredInBox("KIRITO", cX, cW, cY + 125, 2, SDL_Color{60, 80, 75, 150});
        drawTextCenteredInBox("[ PRESS A ]", cX, cW, cY + 220, 1, SDL_Color{60, 80, 75, 130});
    }

    // ASUNA
    if (!kSelected) {
        float cX = 465.0f, cY = 170.0f, cW = 350.0f, cH = 420.0f;
        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 180);
        SDL_FRect panel = { cX, cY, cW, cH };
        SDL_RenderFillRect(mRenderer, &panel);

        float borderPulse = (sinf(mMenuAnimTimer * 4.0f) + 1.0f) / 2.0f;
        Uint8 borderAlpha = (Uint8)(180 + borderPulse * 75);
        SDL_SetRenderDrawColor(mRenderer, 200, 60, 100, borderAlpha);
        SDL_FRect b1 = { cX, cY, cW, 3.0f };
        SDL_FRect b2 = { cX, cY + cH - 3, cW, 3.0f };
        SDL_FRect b3 = { cX, cY, 3.0f, cH };
        SDL_FRect b4 = { cX + cW - 3, cY, 3.0f, cH };
        SDL_RenderFillRect(mRenderer, &b1);
        SDL_RenderFillRect(mRenderer, &b2);
        SDL_RenderFillRect(mRenderer, &b3);
        SDL_RenderFillRect(mRenderer, &b4);

        if (aTex) {
            SDL_FRect dstRect = { cX + (cW - 120.0f) / 2.0f, cY + 30.0f, 120.0f, 120.0f };
            SDL_RenderTexture(mRenderer, aTex, NULL, &dstRect);
        } else {
            drawTextCenteredInBox("A", cX, cW, cY + 45, 6, roseColor);
        }

        drawTextCenteredInBox("ASUNA", cX, cW, cY + 180, 3, roseColor);
        drawTextCenteredInBox("CHARACTER SKIN: ASUNA", cX, cW, cY + 240, 1, valColor);
        drawTextCenteredInBox("THE FLASH STRIKER", cX, cW, cY + 275, 1, labelColor);
        drawTextCenteredInBox(">> ACTIVE HERO SKIN <<", cX, cW, cY + 350, 2, roseColor);
    } else {
        float cX = 900.0f, cY = 230.0f, cW = 220.0f, cH = 300.0f;
        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 100);
        SDL_FRect panel = { cX, cY, cW, cH };
        SDL_RenderFillRect(mRenderer, &panel);

        SDL_SetRenderDrawColor(mRenderer, 140, 80, 100, 100);
        SDL_FRect b1 = { cX, cY, cW, 2.0f };
        SDL_FRect b2 = { cX, cY + cH - 2, cW, 2.0f };
        SDL_FRect b3 = { cX, cY, 2.0f, cH };
        SDL_FRect b4 = { cX + cW - 2, cY, 2.0f, cH };
        SDL_RenderFillRect(mRenderer, &b1);
        SDL_RenderFillRect(mRenderer, &b2);
        SDL_RenderFillRect(mRenderer, &b3);
        SDL_RenderFillRect(mRenderer, &b4);

        if (aTex) {
            SDL_FRect dstRect = { cX + (cW - 80.0f) / 2.0f, cY + 25.0f, 80.0f, 80.0f };
            SDL_SetTextureAlphaMod(aTex, 120);
            SDL_RenderTexture(mRenderer, aTex, NULL, &dstRect);
            SDL_SetTextureAlphaMod(aTex, 255);
        } else {
            drawTextCenteredInBox("A", cX, cW, cY + 35, 4, SDL_Color{200, 60, 100, 120});
        }

        drawTextCenteredInBox("ASUNA", cX, cW, cY + 125, 2, SDL_Color{100, 70, 80, 150});
        drawTextCenteredInBox("[ PRESS D ]", cX, cW, cY + 220, 1, SDL_Color{100, 70, 80, 130});
    }

    SDL_Color guideColor = {255, 255, 255, 200};
    mFont.drawTextCentered(mRenderer, "USE 'A'/'D' OR 'LEFT'/'RIGHT' TO SELECT  -  PRESS 'ENTER' TO CHOOSE", 640, 1, guideColor);
}

void CGAME::renderStageSelect() {
    renderMenuBackground();

    SDL_Color titleShadow = {0, 0, 0, 80};
    SDL_Color titleColor = {255, 255, 255, 255};
    SDL_Color subtitleColor = {220, 245, 255, 255};
    mFont.drawTextCentered(mRenderer, "SELECT CHALLENGE MODE", 98, 4, titleShadow);
    mFont.drawTextCentered(mRenderer, "SELECT CHALLENGE MODE", 96, 4, titleColor);
    mFont.drawTextCentered(mRenderer, "CHOOSE THE QUEST DIFFICULTY", 146, 2, subtitleColor);

    float panelX = 240.0f, panelY = 220.0f;
    float panelW = 800.0f, panelH = 380.0f;

    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 160);
    SDL_FRect panelBg = { panelX, panelY, panelW, panelH };
    SDL_RenderFillRect(mRenderer, &panelBg);

    SDL_SetRenderDrawColor(mRenderer, 20, 100, 80, 200);
    SDL_FRect borderTop    = { panelX, panelY, panelW, 3.0f };
    SDL_FRect borderBottom = { panelX, panelY + panelH - 3, panelW, 3.0f };
    SDL_FRect borderLeft   = { panelX, panelY, 3.0f, panelH };
    SDL_FRect borderRight  = { panelX + panelW - 3, panelY, 3.0f, panelH };
    SDL_RenderFillRect(mRenderer, &borderTop);
    SDL_RenderFillRect(mRenderer, &borderBottom);
    SDL_RenderFillRect(mRenderer, &borderLeft);
    SDL_RenderFillRect(mRenderer, &borderRight);

    std::string stages[2] = {
        "TUTORIAL: 1 MAP (SAFE START)",
        "INFINITE MODE: SURVIVAL"
    };

    SDL_Color normalColor  = {30, 60, 50, 255};
    SDL_Color selectColor  = {10, 95, 75, 255};

    for (int i = 0; i < 2; ++i) {
        int yPos = 280 + i * 120;

        if (mSelectedStageOption == i) {
            SDL_SetRenderDrawColor(mRenderer, 20, 120, 100, 60);
            SDL_FRect highlight = { panelX + 20, (float)yPos - 10, panelW - 40, 50.0f };
            SDL_RenderFillRect(mRenderer, &highlight);

            SDL_SetRenderDrawColor(mRenderer, 20, 140, 100, 255);
            SDL_FRect hlBorder = { panelX + 20, (float)yPos - 10, 4.0f, 50.0f };
            SDL_RenderFillRect(mRenderer, &hlBorder);

            float arrowOffset = sinf(mMenuAnimTimer * 5.0f) * 4.0f;
            mFont.drawText(mRenderer, ">", (int)(panelX + 50 + arrowOffset), yPos, 2, selectColor);
            mFont.drawText(mRenderer, stages[i], (int)(panelX + 90), yPos, 2, selectColor);
        } else {
            mFont.drawText(mRenderer, stages[i], (int)(panelX + 90), yPos, 2, normalColor);
        }
    }

    SDL_Color guideColor = {255, 255, 255, 200};
    mFont.drawTextCentered(mRenderer, "USE 'W'/'S' OR 'UP'/'DOWN' TO CHOOSE  -  ENTER TO ENTER THE QUEST", 635, 1, guideColor);
}

void CGAME::toggleMusic() {
    mAudioMuted = !mAudioMuted;
    if (mAudioMuted) {
        if (mBgmTrack) MIX_PauseTrack(mBgmTrack);
    } else {
        if (mBgmTrack) MIX_ResumeTrack(mBgmTrack);
    }
}

void CGAME::toggleSfx() {
    mSfxMuted = !mSfxMuted;
}

void CGAME::renderSettings() {
    renderMenuBackground();

    SDL_Color titleShadow = {0, 0, 0, 80};
    SDL_Color titleColor = {255, 255, 255, 255};
    SDL_Color subtitleColor = {220, 245, 255, 255};
    mFont.drawTextCentered(mRenderer, "SETTINGS", 98, 4, titleShadow);
    mFont.drawTextCentered(mRenderer, "SETTINGS", 96, 4, titleColor);
    mFont.drawTextCentered(mRenderer, "AUDIO & SOUND OPTIONS", 146, 2, subtitleColor);

    float panelX = 240.0f, panelY = 220.0f;
    float panelW = 800.0f, panelH = 380.0f;

    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 160);
    SDL_FRect panelBg = { panelX, panelY, panelW, panelH };
    SDL_RenderFillRect(mRenderer, &panelBg);

    SDL_SetRenderDrawColor(mRenderer, 20, 100, 80, 200);
    SDL_FRect borderTop    = { panelX, panelY, panelW, 3.0f };
    SDL_FRect borderBottom = { panelX, panelY + panelH - 3, panelW, 3.0f };
    SDL_FRect borderLeft   = { panelX, panelY, 3.0f, panelH };
    SDL_FRect borderRight  = { panelX + panelW - 3, panelY, 3.0f, panelH };
    SDL_RenderFillRect(mRenderer, &borderTop);
    SDL_RenderFillRect(mRenderer, &borderBottom);
    SDL_RenderFillRect(mRenderer, &borderLeft);
    SDL_RenderFillRect(mRenderer, &borderRight);

    std::string options[2] = {
        mAudioMuted ? "MUSIC BGM : [ OFF ]" : "MUSIC BGM : [ ON ]",
        mSfxMuted   ? "SOUND SFX : [ OFF ]" : "SOUND SFX : [ ON ]"
    };

    SDL_Color normalColor = {30, 60, 50, 255};
    SDL_Color selectColor = {10, 95, 75, 255};

    for (int i = 0; i < 2; ++i) {
        int yPos = 310 + i * 80;

        if (mSelectedSettingsOption == i) {
            SDL_SetRenderDrawColor(mRenderer, 20, 120, 100, 60);
            SDL_FRect highlight = { panelX + 20, (float)yPos - 15, panelW - 40, 55.0f };
            SDL_RenderFillRect(mRenderer, &highlight);

            SDL_SetRenderDrawColor(mRenderer, 20, 140, 100, 255);
            SDL_FRect hlBorder = { panelX + 20, (float)yPos - 15, 4.0f, 55.0f };
            SDL_RenderFillRect(mRenderer, &hlBorder);

            float arrowOffset = sinf(mMenuAnimTimer * 5.0f) * 4.0f;
            mFont.drawText(mRenderer, ">", (int)(panelX + 50 + arrowOffset), yPos, 3, selectColor);
            mFont.drawText(mRenderer, options[i], (int)(panelX + 90), yPos, 3, selectColor);
        } else {
            mFont.drawText(mRenderer, options[i], (int)(panelX + 90), yPos, 3, normalColor);
        }
    }

    SDL_Color guideColor = {255, 255, 255, 200};
    mFont.drawTextCentered(mRenderer, "W/S TO SELECT  -  ENTER / A / D TO TOGGLE  -  ESC TO RETURN", 635, 1, guideColor);
}

void CGAME::renderPlaying() {
    std::lock_guard<std::mutex> lock(mGameMutex);
    if (mIsInfinityMode) {
        auto tileTex = [&](SDL_Texture* tex, float laneY, float laneW, float laneH) {
            float texW = 0, texH = 0;
            SDL_GetTextureSize(tex, &texW, &texH);
            if (texW <= 0 || texH <= 0) return;

            float tileW = texW * (laneH / texH);

            for (float x = 0.0f; x < laneW; x += tileW) {
                float drawW = (x + tileW > laneW) ? (laneW - x) : tileW;
                float srcW = texW * (drawW / tileW);
                SDL_FRect srcRect = { 0.0f, 0.0f, srcW, texH };
                SDL_FRect dstRect = { x, laneY, drawW, laneH };
                SDL_RenderTexture(mRenderer, tex, &srcRect, &dstRect);
            }
        };

        for (const auto& lane : mLanes) {
            float screenY = (float)lane.worldY - mCameraY;
            if (screenY < -mLaneHeight || screenY > 720.0f) continue;

            bool isRoad = (lane.type == LaneType::VEHICLE);
            bool isForest = (lane.type == LaneType::MONSTER);

            if (lane.type == LaneType::REST) {
                if (mLaneRestTexture) {
                    tileTex(mLaneRestTexture, screenY, 1280.0f, (float)mLaneHeight);
                } else {
                    SDL_SetRenderDrawColor(mRenderer, 120, 190, 140, 255);
                    SDL_FRect restLane = { 0.0f, screenY, 1280.0f, (float)mLaneHeight };
                    SDL_RenderFillRect(mRenderer, &restLane);
                }
            } else if (isRoad) {
                if (mLaneRoadTexture) {
                    tileTex(mLaneRoadTexture, screenY, 1280.0f, (float)mLaneHeight);
                } else {
                    SDL_SetRenderDrawColor(mRenderer, 44, 52, 70, 255);
                    SDL_FRect roadLane = { 0.0f, screenY, 1280.0f, (float)mLaneHeight };
                    SDL_RenderFillRect(mRenderer, &roadLane);
                    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 200);
                    for (int x = 0; x < 1280; x += 80) {
                        SDL_FRect dash = { (float)x, screenY + 38.0f, 30.0f, 4.0f };
                        SDL_RenderFillRect(mRenderer, &dash);
                    }
                }
            } else if (isForest) {
                if (mLaneForestTexture) {
                    tileTex(mLaneForestTexture, screenY, 1280.0f, (float)mLaneHeight);
                } else {
                    SDL_SetRenderDrawColor(mRenderer, 34, 150, 72, 255);
                    SDL_FRect forestLane = { 0.0f, screenY, 1280.0f, (float)mLaneHeight };
                    SDL_RenderFillRect(mRenderer, &forestLane);
                    SDL_SetRenderDrawColor(mRenderer, 20, 105, 50, 255);
                    for (int x = 40; x < 1280; x += 140) {
                        SDL_FRect grass = { (float)x, screenY + 20.0f, 8.0f, 20.0f };
                        SDL_RenderFillRect(mRenderer, &grass);
                    }
                }
            }

        SDL_SetRenderDrawColor(mRenderer, 80, 200, 255, 120);
            SDL_FRect border = { 0.0f, screenY - 1.0f, 1280.0f, 2.0f };
            SDL_RenderFillRect(mRenderer, &border);
        }

        drawObstacleList(mGleameyes, mRenderer, mFont, mCameraY);
        drawObstacleList(mCheathcliffs, mRenderer, mFont, mCameraY);
        drawObstacleList(mCillfangs, mRenderer, mFont, mCameraY);
        drawObstacleList(mCicedragons, mRenderer, mFont, mCameraY);
        drawObstacleList(mBluewings, mRenderer, mFont, mCameraY);
        drawObstacleList(mSkyarmors, mRenderer, mFont, mCameraY);

        for (auto& tl : mTrafficLights) {
            tl.draw(mRenderer, mFont, mCameraY);
        }

        mPlayer.draw(mRenderer, mFont, mCameraY);

        if (mFlashTimer > 0.0f) {
            SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
            Uint8 alpha = (Uint8)((mFlashTimer / 0.5f) * 180.0f);
            SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, alpha);
            SDL_FRect flashRect = { 0.0f, 0.0f, 1280.0f, 720.0f };
            SDL_RenderFillRect(mRenderer, &flashRect);
        }

        SDL_Color hudColor = {255, 255, 255, 255};
        SDL_Color cyanGlow = {80, 200, 255, 255};
        SDL_Color shadow = {0, 0, 0, 180};
        std::string hudStageText = "SCORE: " + std::to_string(mScore);

        mFont.drawText(mRenderer, hudStageText, 22, 26, 2, shadow);
        mFont.drawText(mRenderer, hudStageText, 20, 24, 2, cyanGlow);

        mFont.drawText(mRenderer, "PRESS ESC TO RETURN TO MENU", 882, 26, 2, shadow);
        mFont.drawText(mRenderer, "PRESS ESC TO RETURN TO MENU", 880, 24, 2, hudColor);
        return;
    }

    auto tileTex = [&](SDL_Texture* tex, float laneY, float laneW, float laneH) {
        float texW = 0, texH = 0;
        SDL_GetTextureSize(tex, &texW, &texH);
        if (texW <= 0 || texH <= 0) return;

        float tileW = texW * (laneH / texH);

        for (float x = 0.0f; x < laneW; x += tileW) {
            float drawW = (x + tileW > laneW) ? (laneW - x) : tileW;
            float srcW = texW * (drawW / tileW);
            SDL_FRect srcRect = { 0.0f, 0.0f, srcW, texH };
            SDL_FRect dstRect = { x, laneY, drawW, laneH };
            SDL_RenderTexture(mRenderer, tex, &srcRect, &dstRect);
        }
    };

    // VỈA HÈ TRÊN (Destination Safe Zone: Y = 0 đến 120)
    if (mSidewalkTopTexture) {
        tileTex(mSidewalkTopTexture, 0.0f, 1280.0f, 120.0f);
    } else {
        SDL_FRect topSidewalk = { 0.0f, 0.0f, 1280.0f, 120.0f };
        SDL_SetRenderDrawColor(mRenderer, 34, 150, 72, 255);
        SDL_RenderFillRect(mRenderer, &topSidewalk);
        SDL_SetRenderDrawColor(mRenderer, 20, 95, 45, 255);
        SDL_FRect topBorder = { 0.0f, 116.0f, 1280.0f, 4.0f };
        SDL_RenderFillRect(mRenderer, &topBorder);
        SDL_SetRenderDrawColor(mRenderer, 255, 90, 110, 255);
        int flowerX[] = { 110, 260, 480, 620, 780, 960, 1140 };
        for (int fx : flowerX) {
            SDL_FRect f = { (float)fx, 40.0f, 5.0f, 5.0f };
            SDL_RenderFillRect(mRenderer, &f);
            SDL_FRect f2 = { (float)fx + 30.0f, 80.0f, 5.0f, 5.0f };
            SDL_RenderFillRect(mRenderer, &f2);
        }
    }

    // LÀN 1: VEHICLE
    if (mLaneRoadTexture) {
        tileTex(mLaneRoadTexture, 120.0f, 1280.0f, 80.0f);
    } else {
        SDL_SetRenderDrawColor(mRenderer, 44, 52, 70, 255);
        SDL_FRect roadLane = { 0.0f, 120.0f, 1280.0f, 80.0f };
        SDL_RenderFillRect(mRenderer, &roadLane);
        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 200);
        for (int x = 0; x < 1280; x += 80) {
            SDL_FRect dash = { (float)x, 158.0f, 30.0f, 4.0f };
            SDL_RenderFillRect(mRenderer, &dash);
        }
    }

    // LÀN 2: MONSTER
    if (mLaneForestTexture) {
        tileTex(mLaneForestTexture, 200.0f, 1280.0f, 80.0f);
    } else {
        SDL_SetRenderDrawColor(mRenderer, 34, 150, 72, 255);
        SDL_FRect forestLane = { 0.0f, 200.0f, 1280.0f, 80.0f };
        SDL_RenderFillRect(mRenderer, &forestLane);
        SDL_SetRenderDrawColor(mRenderer, 20, 105, 50, 255);
        for (int x = 40; x < 1280; x += 140) {
            SDL_FRect grass = { (float)x, 220.0f, 8.0f, 20.0f };
            SDL_RenderFillRect(mRenderer, &grass);
        }
    }

    // LÀN 3: LÀN NGHỈ
    if (mLaneRestTexture) {
        tileTex(mLaneRestTexture, 280.0f, 1280.0f, 80.0f);
    } else {
        SDL_SetRenderDrawColor(mRenderer, 120, 190, 140, 255);
        SDL_FRect restLane = { 0.0f, 280.0f, 1280.0f, 80.0f };
        SDL_RenderFillRect(mRenderer, &restLane);
    }

    // LÀN 4: VEHICLE
    if (mLaneForestTexture) {
        tileTex(mLaneForestTexture, 360.0f, 1280.0f, 80.0f);
    } else {
        SDL_SetRenderDrawColor(mRenderer, 34, 150, 72, 255);
        SDL_FRect forestLane = { 0.0f, 360.0f, 1280.0f, 80.0f };
        SDL_RenderFillRect(mRenderer, &forestLane);
        SDL_SetRenderDrawColor(mRenderer, 20, 105, 50, 255);
        for (int x = 40; x < 1280; x += 140) {
            SDL_FRect grass = { (float)x, 380.0f, 8.0f, 20.0f };
            SDL_RenderFillRect(mRenderer, &grass);
        }
    }

    // LÀN 5: MONSTER
    if (mLaneRoadTexture) {
        tileTex(mLaneRoadTexture, 440.0f, 1280.0f, 80.0f);
    } else {
        SDL_SetRenderDrawColor(mRenderer, 44, 52, 70, 255);
        SDL_FRect roadLane = { 0.0f, 440.0f, 1280.0f, 80.0f };
        SDL_RenderFillRect(mRenderer, &roadLane);
        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 200);
        for (int x = 0; x < 1280; x += 80) {
            SDL_FRect dash = { (float)x, 478.0f, 30.0f, 4.0f };
            SDL_RenderFillRect(mRenderer, &dash);
        }
    }

    // LÀN 6: MONSTER
    if (mLaneForestTexture) {
        tileTex(mLaneForestTexture, 520.0f, 1280.0f, 80.0f);
    } else {
        SDL_SetRenderDrawColor(mRenderer, 34, 150, 72, 255);
        SDL_FRect forestLane = { 0.0f, 520.0f, 1280.0f, 80.0f };
        SDL_RenderFillRect(mRenderer, &forestLane);
        SDL_SetRenderDrawColor(mRenderer, 20, 105, 50, 255);
        for (int x = 40; x < 1280; x += 140) {
            SDL_FRect grass = { (float)x, 540.0f, 8.0f, 20.0f };
            SDL_RenderFillRect(mRenderer, &grass);
        }
    }

    // VỈA HÈ DƯỚI (Starting Camp: Y = 600 đến 720)
    if (mSidewalkBottomTexture) {
        tileTex(mSidewalkBottomTexture, 600.0f, 1280.0f, 120.0f);
    } else {
        SDL_FRect bottomSidewalk = { 0.0f, 600.0f, 1280.0f, 120.0f };
        SDL_SetRenderDrawColor(mRenderer, 34, 150, 72, 255);
        SDL_RenderFillRect(mRenderer, &bottomSidewalk);
        SDL_SetRenderDrawColor(mRenderer, 20, 95, 45, 255);
        SDL_FRect botBorder = { 0.0f, 600.0f, 1280.0f, 4.0f };
        SDL_RenderFillRect(mRenderer, &botBorder);
        SDL_SetRenderDrawColor(mRenderer, 255, 90, 110, 255);
        int flowerX[] = { 150, 300, 520, 660, 820, 1000, 1180 };
        for (int fx : flowerX) {
            SDL_FRect f = { (float)fx, 620.0f, 5.0f, 5.0f };
            SDL_RenderFillRect(mRenderer, &f);
        }
    }

    // HOLOGRAPHIC NEON CYAN DIVIDERS (SAO Theme Borders)
    SDL_SetRenderDrawColor(mRenderer, 80, 200, 255, 140);
    float dividerY[] = { 120.0f, 200.0f, 280.0f, 360.0f, 440.0f, 520.0f, 600.0f };
    for (float dy : dividerY) {
        SDL_FRect border = { 0.0f, dy - 1.0f, 1280.0f, 2.0f };
        SDL_RenderFillRect(mRenderer, &border);
    }

    for (auto t : mGleameyes) {
        t->draw(mRenderer, mFont, 0.0f);
    }
    for (auto c : mCheathcliffs) {
        c->draw(mRenderer, mFont, 0.0f);
    }
    for (auto d : mCillfangs) {
        d->draw(mRenderer, mFont, 0.0f);
    }
    for (auto b : mCicedragons) {
        b->draw(mRenderer, mFont, 0.0f);
    }
    for (auto bw : mBluewings) {
        bw->draw(mRenderer, mFont, 0.0f);
    }
    for (auto sa : mSkyarmors) {
        sa->draw(mRenderer, mFont, 0.0f);
    }
    for (auto& tl : mTrafficLights) {
        tl.draw(mRenderer, mFont, 0.0f);
    }

    mPlayer.draw(mRenderer, mFont, 0.0f);

    if (mFlashTimer > 0.0f) {
        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        Uint8 alpha = (Uint8)((mFlashTimer / 0.5f) * 180.0f);
        SDL_SetRenderDrawColor(mRenderer, 255, 0, 0, alpha);
        SDL_FRect flashRect = { 0.0f, 0.0f, 1280.0f, 720.0f };
        SDL_RenderFillRect(mRenderer, &flashRect);
    }

    SDL_Color hudColor = {255, 255, 255, 255};
    SDL_Color cyanGlow = {80, 200, 255, 255};
    std::string hudStageText = "MODE: TUTORIAL";
    
    SDL_Color shadow = {0, 0, 0, 180};
    mFont.drawText(mRenderer, hudStageText, 22, 26, 2, shadow);
    mFont.drawText(mRenderer, hudStageText, 20, 24, 2, cyanGlow);
    
    mFont.drawText(mRenderer, "PRESS ESC TO RETURN TO MENU", 882, 26, 2, shadow);
    mFont.drawText(mRenderer, "PRESS ESC TO RETURN TO MENU", 880, 24, 2, hudColor);
}

void CGAME::startGame() {
    mSelectedCharOption = 0;
    mSelectedStageOption = 0;
    mState = GameState::CHAR_SELECT;
}

void CGAME::resetGame() {
    std::lock_guard<std::mutex> lock(mGameMutex);
    if (mIsInfinityMode) {
        resetInfinite();
    } else {
        resetTutorial();
    }
}

int CGAME::randomRange(int minValue, int maxValue) const {
    if (maxValue <= minValue) return minValue;
    return minValue + (rand() % (maxValue - minValue + 1));
}

void CGAME::resetTutorial() {
    mIsInfinityMode = false;
    mStage = 1;
    mInfiniteLevel = 1;
    mCameraY = 0.0f;
    mLanes.clear();

    mPlayer.resetPosition();
    clearObstacles();

    mTrafficLights.clear();
    CTRAFFICLIGHT t1(120, 3.0f, 5.0f);
    t1.initTextures(mRenderer);
    CTRAFFICLIGHT t2(440, 3.0f, 5.0f);
    t2.initTextures(mRenderer);
    mTrafficLights.push_back(t1);
    mTrafficLights.push_back(t2);

    auto spawnLane = [&](LaneType laneType, int laneY, int laneCount, int direction) {
        int spacing = 1280 / laneCount;
        for (int i = 0; i < laneCount; ++i) {
            int startX = i * spacing + randomRange(0, spacing - 40);
            if (laneType == LaneType::VEHICLE) {
                if ((randomRange(0, 1) == 0)) {
                    CBLUEWING* c = new CBLUEWING(startX, laneY, randomRange(4, 6), direction);
                    c->setTexture(mCbluewingTexture);
                    mBluewings.push_back(c);
                } else {
                    CSKYARMOR* t = new CSKYARMOR(startX, laneY, randomRange(2, 4), direction);
                    t->setTexture(mCskyarmorTexture);
                    mSkyarmors.push_back(t);
                }
            } else if (laneType == LaneType::MONSTER) {
                int r = randomRange(0, 3);
                if (r == 0) {
                    CILLFANG* d = new CILLFANG(startX, laneY, randomRange(2, 4), direction);
                    d->setTextures(mCillfangTexture1, mCillfangTexture2);
                    mCillfangs.push_back(d);
                } else if (r == 1) {
                    CICEDRAGON* b = new CICEDRAGON(startX, laneY, randomRange(4, 6), direction);
                    b->setTextures(mCicedragonTexture1, mCicedragonTexture2);
                    mCicedragons.push_back(b);
                } else if (r == 2) {
                    CHEATHCLIFF* c = new CHEATHCLIFF(startX, laneY, randomRange(3, 5), direction);
                    c->setTextures(mCheathcliffTexture1, mCheathcliffTexture2);
                    mCheathcliffs.push_back(c);
                } else {
                    CGLEAMEYES* t = new CGLEAMEYES(startX, laneY, randomRange(2, 4), direction);
                    t->setTextures(mCGleameyesTexture1, mCGleameyesTexture2);
                    mGleameyes.push_back(t);
                }
            }
        }
    };

    spawnLane(LaneType::MONSTER, 120, 3, -1);
    spawnLane(LaneType::MONSTER, 200, 3, 1);
    spawnLane(LaneType::VEHICLE, 360, 3, 1);
    spawnLane(LaneType::VEHICLE, 440, 3, -1);
    spawnLane(LaneType::MONSTER, 520, 3, 1);
}

void CGAME::resetInfinite() {
    mIsInfinityMode = true;
    mStage = 1;
    mInfiniteLevel = 1;
    mCameraY = 0.0f;
    mLanePatternIndex = 0;
    mScore = 0;

    mPlayer.resetPosition();
    mMaxReachedY = mPlayer.getY();

    clearObstacles();
    mTrafficLights.clear();
    initInfiniteLanes();
}

void CGAME::initInfiniteLanes() {
    mLanes.clear();

    Lane bottomFillLane;
    bottomFillLane.type = LaneType::REST;
    bottomFillLane.worldY = 680;
    mLanes.push_back(bottomFillLane);

    Lane startLane;
    startLane.type = LaneType::REST;
    startLane.worldY = 600;
    mLanes.insert(mLanes.begin(), startLane);

    while (mLanes.front().worldY > -mLaneHeight * 3) {
        addLaneAbove();
    }
}

void CGAME::addLaneAbove() {
    static const LaneType pattern[] = {
        LaneType::VEHICLE,
        LaneType::MONSTER,
        LaneType::REST,
        LaneType::MONSTER,
        LaneType::VEHICLE,
        LaneType::MONSTER
    };
    const int patternCount = (int)(sizeof(pattern) / sizeof(pattern[0]));

    Lane lane;
    lane.type = pattern[mLanePatternIndex % patternCount];
    lane.worldY = mLanes.front().worldY - mLaneHeight;
    mLanePatternIndex++;

    mLanes.insert(mLanes.begin(), lane);
    spawnObstaclesForLane(lane);
}

void CGAME::pruneLanes() {
    while (!mLanes.empty()) {
        float screenY = (float)mLanes.back().worldY - mCameraY;
        if (screenY <= 720.0f + mLaneHeight * 2) break;
        mLanes.pop_back();
    }

    mTrafficLights.erase(
        std::remove_if(mTrafficLights.begin(), mTrafficLights.end(), [&](const CTRAFFICLIGHT& light) {
            float screenY = (float)light.getLaneY() - mCameraY;
            return screenY > 720.0f + mLaneHeight * 2;
        }),
        mTrafficLights.end()
    );
}

void CGAME::spawnObstaclesForLane(const Lane& lane) {
    if (lane.type == LaneType::REST) return;

    int speedBoost = (mInfiniteLevel - 1) / 2;
    int count = randomRange(2, 4);
    int spacing = 1280 / count;

    auto spawnLane = [&](LaneType laneType, int direction) {
        for (int i = 0; i < count; ++i) {
            int startX = i * spacing + randomRange(0, spacing - 40);
            if (laneType == LaneType::VEHICLE) {
                if (randomRange(0, 1) == 0) {
                    int speed = randomRange(2 + speedBoost, 3 + speedBoost);
                    CBLUEWING* c = new CBLUEWING(startX, lane.worldY, speed, direction);
                    c->setTexture(mCbluewingTexture);
                    mBluewings.push_back(c);
                } else {
                    int speed = randomRange(1 + speedBoost, 2 + speedBoost);
                    CSKYARMOR* t = new CSKYARMOR(startX, lane.worldY, speed, direction);
                    t->setTexture(mCskyarmorTexture);
                    mSkyarmors.push_back(t);
                }
            } else if (laneType == LaneType::MONSTER) {
                int r = randomRange(0, 3);
                if (r == 0) {
                    int speed = randomRange(1 + speedBoost, 2 + speedBoost);
                    CILLFANG* d = new CILLFANG(startX, lane.worldY, speed, direction);
                    d->setTextures(mCillfangTexture1, mCillfangTexture2);
                    mCillfangs.push_back(d);
                } else if (r == 1) {
                    int speed = randomRange(2 + speedBoost, 3 + speedBoost);
                    CICEDRAGON* b = new CICEDRAGON(startX, lane.worldY, speed, direction);
                    b->setTextures(mCicedragonTexture1, mCicedragonTexture2);
                    mCicedragons.push_back(b);
                } else if (r == 2) {
                    int speed = randomRange(1 + speedBoost, 2 + speedBoost);
                    CHEATHCLIFF* c = new CHEATHCLIFF(startX, lane.worldY, speed, direction);
                    c->setTextures(mCheathcliffTexture1, mCheathcliffTexture2);
                    mCheathcliffs.push_back(c);
                } else {
                    int speed = randomRange(1 + speedBoost, 2 + speedBoost);
                    CGLEAMEYES* t = new CGLEAMEYES(startX, lane.worldY, speed, direction);
                    t->setTextures(mCGleameyesTexture1, mCGleameyesTexture2);
                    mGleameyes.push_back(t);
                }
            }
        }
    };

    if (lane.type == LaneType::VEHICLE) {
        CTRAFFICLIGHT light(lane.worldY, 3.0f, 5.0f);
        light.initTextures(mRenderer);
        mTrafficLights.push_back(light);
    }

    spawnLane(lane.type, (lane.type == LaneType::VEHICLE) ? -1 : 1);
}

void CGAME::updateInfinite(float deltaTime) {
    mPlayer.update(deltaTime);

    moveObstacleList(mGleameyes, 0, 1280);
    moveObstacleList(mCheathcliffs, 0, 1280);
    moveObstacleList(mCillfangs, 0, 1280);
    moveObstacleList(mCicedragons, 0, 1280);

    for (auto bw : mBluewings) {
        bool isRed = false;
        for (const auto& light : mTrafficLights) {
            if (light.getLaneY() == bw->getY() && light.isRed()) { isRed = true; break; }
        }
        if (!isRed) bw->Move(0, 1280);
    }
    for (auto sa : mSkyarmors) {
        bool isRed = false;
        for (const auto& light : mTrafficLights) {
            if (light.getLaneY() == sa->getY() && light.isRed()) { isRed = true; break; }
        }
        if (!isRed) sa->Move(0, 1280);
    }

    float screenY = (float)mPlayer.getY() - mCameraY;
    if (screenY < 200.0f) {
        mCameraY = (float)mPlayer.getY() - 200.0f;
    }

    screenY = (float)mPlayer.getY() - mCameraY;
    if (screenY > 600.0f) {
        int newY = (int)(mCameraY + 600.0f);
        int delta = newY - mPlayer.getY();
        if (delta > 0) {
            mPlayer.Down(newY);
        }
    }

    int newLevel = 1 + (int)((-mCameraY) / (mLaneHeight * 6));
    if (newLevel > mInfiniteLevel) {
        mInfiniteLevel = newLevel;
    }

    if (mPlayer.getY() < mMaxReachedY) {
        int diff = mMaxReachedY - mPlayer.getY();
        int passedLanes = diff / mLaneHeight;
        if (passedLanes > 0) {
            mScore += passedLanes;
            mMaxReachedY -= passedLanes * mLaneHeight;
        }
    }

    while (!mLanes.empty() && mLanes.front().worldY > (int)mCameraY - mLaneHeight * 2) {
        addLaneAbove();
    }
    pruneLanes();

    pruneObstacleList(mGleameyes, mCameraY, -200.0f, 820.0f);
    pruneObstacleList(mCheathcliffs, mCameraY, -200.0f, 820.0f);
    pruneObstacleList(mCillfangs, mCameraY, -200.0f, 820.0f);
    pruneObstacleList(mCicedragons, mCameraY, -200.0f, 820.0f);
    pruneObstacleList(mBluewings, mCameraY, -200.0f, 820.0f);
    pruneObstacleList(mSkyarmors, mCameraY, -200.0f, 820.0f);

    if (hitPlayerAgainstList(mPlayer, mGleameyes) ||
        hitPlayerAgainstList(mPlayer, mCheathcliffs) ||
        hitPlayerAgainstList(mPlayer, mCillfangs) ||
        hitPlayerAgainstList(mPlayer, mCicedragons) ||
        hitPlayerAgainstList(mPlayer, mBluewings) ||
        hitPlayerAgainstList(mPlayer, mSkyarmors)) {
        mPlayer.setDead(true);
        mState = GameState::GAMEOVER;
        mFlashTimer = 0.5f;
        if (!mSfxMuted && mMixer && mSfxHit) MIX_PlayAudio(mMixer, mSfxHit);
        return;
    }
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

void CGAME::clearObstacles() {
    clearObstacleList(mGleameyes);
    clearObstacleList(mCheathcliffs);
    clearObstacleList(mCillfangs);
    clearObstacleList(mCicedragons);
    clearObstacleList(mBluewings);
    clearObstacleList(mSkyarmors);
}

void CGAME::exitGame() {
    mIsRunning = false;
    mIsThreadRunning = false;

    if (mPhysicsThread.joinable()) {
        mPhysicsThread.join();
    }

    clearObstacles();

    if (mBgmTrack) { MIX_DestroyTrack(mBgmTrack); mBgmTrack = nullptr; }
    if (mBgmMenu) { MIX_DestroyAudio(mBgmMenu); mBgmMenu = nullptr; }
    if (mSfxHit) { MIX_DestroyAudio(mSfxHit); mSfxHit = nullptr; }
    if (mSfxJump) { MIX_DestroyAudio(mSfxJump); mSfxJump = nullptr; }
    if (mMixer) { MIX_DestroyMixer(mMixer); mMixer = nullptr; }

    if (mCGleameyesTexture1) { SDL_DestroyTexture(mCGleameyesTexture1); mCGleameyesTexture1 = nullptr; }
    if (mCGleameyesTexture2) { SDL_DestroyTexture(mCGleameyesTexture2); mCGleameyesTexture2 = nullptr; }
    if (mCheathcliffTexture1) { SDL_DestroyTexture(mCheathcliffTexture1); mCheathcliffTexture1 = nullptr; }
    if (mCheathcliffTexture2) { SDL_DestroyTexture(mCheathcliffTexture2); mCheathcliffTexture2 = nullptr; }
    if (mCillfangTexture1) { SDL_DestroyTexture(mCillfangTexture1); mCillfangTexture1 = nullptr; }
    if (mCillfangTexture2) { SDL_DestroyTexture(mCillfangTexture2); mCillfangTexture2 = nullptr; }
    if (mCicedragonTexture1) { SDL_DestroyTexture(mCicedragonTexture1); mCicedragonTexture1 = nullptr; }
    if (mCicedragonTexture2) { SDL_DestroyTexture(mCicedragonTexture2); mCicedragonTexture2 = nullptr; }

    if (mBgMenuTexture) { SDL_DestroyTexture(mBgMenuTexture); mBgMenuTexture = nullptr; }
    if (mBgPlayingTexture) { SDL_DestroyTexture(mBgPlayingTexture); mBgPlayingTexture = nullptr; }
    if (mSidewalkTopTexture) { SDL_DestroyTexture(mSidewalkTopTexture); mSidewalkTopTexture = nullptr; }
    if (mSidewalkBottomTexture) { SDL_DestroyTexture(mSidewalkBottomTexture); mSidewalkBottomTexture = nullptr; }
    if (mLaneRestTexture) { SDL_DestroyTexture(mLaneRestTexture); mLaneRestTexture = nullptr; }
    if (mLaneForestTexture) { SDL_DestroyTexture(mLaneForestTexture); mLaneForestTexture = nullptr; }
    if (mLaneRoadTexture) { SDL_DestroyTexture(mLaneRoadTexture); mLaneRoadTexture = nullptr; }
    if (mCbluewingTexture) { SDL_DestroyTexture(mCbluewingTexture); mCbluewingTexture = nullptr; }
    if (mCskyarmorTexture) { SDL_DestroyTexture(mCskyarmorTexture); mCskyarmorTexture = nullptr; }
    
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
