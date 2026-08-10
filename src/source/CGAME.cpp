#include "CGAME.h"
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
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
      mBgMenuTexture(nullptr), mSidewalkTopTexture(nullptr), mSidewalkBottomTexture(nullptr),
      mLaneRestTexture(nullptr), mLaneForestTexture(nullptr), mLaneRoadTexture(nullptr),
      mCbluewingTexture(nullptr), mCskyarmorTexture(nullptr),
      mTrafficLightRedTexture(nullptr), mTrafficLightGreenTexture(nullptr),
      mStage(1), mIsInfinityMode(false), mPendingStageAdvance(false),
      mCameraY(0.0f), mLaneHeight(80), mInfiniteLevel(1), mLanePatternIndex(0),
      mSelectedMenuOption(0), mSelectedCharOption(0), mSelectedStageOption(0), mSelectedSettingsOption(0),
      mSelectedPauseOption(0), mSelectedSaveIndex(0), mSelectedLoadIndex(0),
      mSettingsPreviousState(GameState::MENU),
      mLoadPreviousState(GameState::MENU),
      mPendingDeleteFileName(""),
      mPendingSaveSlotIndex(-1), mPendingLoadSlotIndex(-1),
      mScore(0), mMaxReachedY(0),
      mShowMenuWarning(false), mWarningTimer(0.0f), mMenuAnimTimer(0.0f),
      mMixer(nullptr), mBgmTrack(nullptr), mSfxTracks{nullptr, nullptr, nullptr, nullptr}, mSfxTrackIndex(0),
      mBgmMenu(nullptr), mSfxHit(nullptr), mSfxJump(nullptr), mAudioMuted(false), mSfxMuted(false),
      mBgmVolume(100), mSfxVolume(100),
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
        mWindow = nullptr;
        MIX_Quit();
        SDL_Quit();
        return false;
    }

    // 5. Khởi tạo Bitmap Font vẽ chữ pixel
    if (!mFont.init(mRenderer)) {
        std::cerr << "Failed to initialize font!" << std::endl;
        SDL_DestroyRenderer(mRenderer);
        mRenderer = nullptr;
        SDL_DestroyWindow(mWindow);
        mWindow = nullptr;
        MIX_Quit();
        SDL_Quit();
        return false;
    }

    // 6. Load âm thanh từ thư mục assets/audio/ (.mp3, .wav, .ogg)
    if (mMixer) {
        mBgmTrack = MIX_CreateTrack(mMixer);
        for (int i = 0; i < 4; ++i) {
            mSfxTracks[i] = MIX_CreateTrack(mMixer);
        }
        mBgmMenu = loadAudioFlexible(mMixer, "assets/audio/bgm_menu");
        mSfxHit = loadAudioFlexible(mMixer, "assets/audio/sfx_hit");
        mSfxJump = loadAudioFlexible(mMixer, "assets/audio/sfx_jump");

        if (mBgmMenu && mBgmTrack) {
            playBGM(mBgmTrack, mBgmMenu);
            updateVolumeSettings();
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

    // 2. CHEATHCLIFF
    mCheathcliffTexture1 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cheathcliff1.png");
    mCheathcliffTexture2 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cheathcliff2.png");

    // 3. CILLFANG
    mCillfangTexture1 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cillfang1.png");
    mCillfangTexture2 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cillfang2.png");

    // 4. CICEDRAGON
    mCicedragonTexture1 = IMG_LoadTexture(mRenderer, "assets/images/monsters/cicedragon1.png");
    mBgMenuTexture = IMG_LoadTexture(mRenderer, "assets/images/ui/bg_menu.png");
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
        else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
            float mx = (float)event.button.x;
            float my = (float)event.button.y;

            if (!mPendingDeleteFileName.empty()) {
                // Click YES [Y] button
                if (mx >= 370.0f && mx <= 610.0f && my >= 400.0f && my <= 460.0f) {
                    try { std::filesystem::remove("saves/" + mPendingDeleteFileName); } catch (...) {}
                    mPendingDeleteFileName = "";
                    scanSaveSlots();
                }
                // Click CANCEL [N] button
                else if (mx >= 670.0f && mx <= 910.0f && my >= 400.0f && my <= 460.0f) {
                    mPendingDeleteFileName = "";
                }
                continue;
            }

            if (mPendingSaveSlotIndex >= 0) {
                // Click YES [Y] button
                if (mx >= 370.0f && mx <= 610.0f && my >= 400.0f && my <= 460.0f) {
                    if (saveGame(mPendingSaveSlotIndex)) {
                        mState = GameState::PLAYING;
                    }
                    mPendingSaveSlotIndex = -1;
                }
                // Click CANCEL [N] button
                else if (mx >= 670.0f && mx <= 910.0f && my >= 400.0f && my <= 460.0f) {
                    mPendingSaveSlotIndex = -1;
                }
                continue;
            }

            if (mPendingLoadSlotIndex >= 0) {
                // Click YES [Y] button
                if (mx >= 370.0f && mx <= 610.0f && my >= 400.0f && my <= 460.0f) {
                    if (loadGame(mPendingLoadSlotIndex)) {
                        mState = GameState::PLAYING;
                    }
                    mPendingLoadSlotIndex = -1;
                }
                // Click CANCEL [N] button
                else if (mx >= 670.0f && mx <= 910.0f && my >= 400.0f && my <= 460.0f) {
                    mPendingLoadSlotIndex = -1;
                }
                continue;
            }

            float panelX = 240.0f, panelW = 800.0f;
            if (mState == GameState::SAVE_DIALOG) {
                for (int i = 0; i < 5; ++i) {
                    float yPos = 245.0f + (float)i * 65.0f;
                    float btnX = panelX + panelW - 75.0f;
                    float btnY = yPos - 6.0f;

                    // Click Red [X] Delete Button
                    if (mSaveSlots[i].exists && mx >= btnX && mx <= btnX + 45.0f && my >= btnY && my <= btnY + 35.0f) {
                        mPendingDeleteFileName = mSaveSlots[i].filename;
                        break;
                    }
                    // Click Row to Select & Prompt Save Confirmation
                    else if (mx >= panelX + 20 && mx <= panelX + panelW - 20 && my >= yPos - 10.0f && my <= yPos + 45.0f) {
                        mSelectedSaveIndex = i;
                        mPendingSaveSlotIndex = i;
                        break;
                    }
                }
            }
            else if (mState == GameState::LOAD_DIALOG) {
                for (int i = 0; i < 5; ++i) {
                    float yPos = 245.0f + (float)i * 65.0f;
                    float btnX = panelX + panelW - 75.0f;
                    float btnY = yPos - 6.0f;

                    // Click Red [X] Delete Button
                    if (mSaveSlots[i].exists && mx >= btnX && mx <= btnX + 45.0f && my >= btnY && my <= btnY + 35.0f) {
                        mPendingDeleteFileName = mSaveSlots[i].filename;
                        break;
                    }
                    // Click Row to Select & Prompt Load Confirmation
                    else if (mx >= panelX + 20 && mx <= panelX + panelW - 20 && my >= yPos - 10.0f && my <= yPos + 45.0f) {
                        mSelectedLoadIndex = i;
                        if (mSaveSlots[i].exists) {
                            mPendingLoadSlotIndex = i;
                        }
                        break;
                    }
                }
            }
            else if (mState == GameState::MENU) {
                float menuPanelX = 240.0f, menuPanelW = 800.0f;
                for (int i = 0; i < 4; ++i) {
                    float yPos = 390.0f + (float)i * 55.0f;
                    if (mx >= menuPanelX + 10 && mx <= menuPanelX + menuPanelW - 10 && my >= yPos - 5.0f && my <= yPos + 40.0f) {
                        mSelectedMenuOption = i;
                        if (i == 0) {
                            startGame();
                        } else if (i == 1) {
                            scanSaveSlots();
                            mSelectedLoadIndex = 0;
                            mLoadPreviousState = GameState::MENU;
                            mState = GameState::LOAD_DIALOG;
                        } else if (i == 2) {
                            mSettingsPreviousState = GameState::MENU;
                            mSelectedSettingsOption = 0;
                            mState = GameState::SETTINGS;
                        } else if (i == 3) {
                            mIsRunning = false;
                        }
                        break;
                    }
                }
            }
        }
        else if (event.type == SDL_EVENT_KEY_DOWN) {
            SDL_Keycode key = event.key.key;

            if (!mPendingDeleteFileName.empty()) {
                if (key == SDLK_Y) {
                    try { std::filesystem::remove("saves/" + mPendingDeleteFileName); } catch (...) {}
                    mPendingDeleteFileName = "";
                    scanSaveSlots();
                }
                else if (key == SDLK_N || key == SDLK_ESCAPE) {
                    mPendingDeleteFileName = "";
                }
                continue;
            }

            if (mPendingSaveSlotIndex >= 0) {
                if (key == SDLK_Y || key == SDLK_RETURN || key == SDLK_SPACE) {
                    if (saveGame(mPendingSaveSlotIndex)) {
                        mState = GameState::PLAYING;
                    }
                    mPendingSaveSlotIndex = -1;
                }
                else if (key == SDLK_N || key == SDLK_ESCAPE) {
                    mPendingSaveSlotIndex = -1;
                }
                continue;
            }

            if (mPendingLoadSlotIndex >= 0) {
                if (key == SDLK_Y || key == SDLK_RETURN || key == SDLK_SPACE) {
                    if (loadGame(mPendingLoadSlotIndex)) {
                        mState = GameState::PLAYING;
                    }
                    mPendingLoadSlotIndex = -1;
                }
                else if (key == SDLK_N || key == SDLK_ESCAPE) {
                    mPendingLoadSlotIndex = -1;
                }
                continue;
            }

            if (mState == GameState::MENU) {
                // Điều khiển Menu bằng phím di chuyển
                if (key == SDLK_W || key == SDLK_UP) {
                    mSelectedMenuOption = (mSelectedMenuOption - 1 + 4) % 4;
                }
                else if (key == SDLK_S || key == SDLK_DOWN) {
                    mSelectedMenuOption = (mSelectedMenuOption + 1) % 4;
                }
                else if (key == SDLK_RETURN || key == SDLK_SPACE) {
                    if (mSelectedMenuOption == 0) {
                        startGame();
                    } else if (mSelectedMenuOption == 1) {
                        scanSaveSlots();
                        mSelectedLoadIndex = 0;
                        mLoadPreviousState = GameState::MENU;
                        mState = GameState::LOAD_DIALOG;
                    } else if (mSelectedMenuOption == 2) {
                        mSettingsPreviousState = GameState::MENU;
                        mSelectedSettingsOption = 0;
                        mState = GameState::SETTINGS;
                    } else if (mSelectedMenuOption == 3) {
                        mIsRunning = false;
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
                    mSelectedSettingsOption = (mSelectedSettingsOption - 1 + 3) % 3;
                }
                else if (key == SDLK_S || key == SDLK_DOWN) {
                    mSelectedSettingsOption = (mSelectedSettingsOption + 1) % 3;
                }
                else if (key == SDLK_A || key == SDLK_LEFT) {
                    if (mSelectedSettingsOption == 0) {
                        mBgmVolume = std::max(0, mBgmVolume - 25);
                        mAudioMuted = (mBgmVolume == 0);
                        updateVolumeSettings();
                    } else if (mSelectedSettingsOption == 1) {
                        mSfxVolume = std::max(0, mSfxVolume - 25);
                        mSfxMuted = (mSfxVolume == 0);
                        updateVolumeSettings();
                    }
                }
                else if (key == SDLK_D || key == SDLK_RIGHT || key == SDLK_RETURN || key == SDLK_SPACE) {
                    if (mSelectedSettingsOption == 0) {
                        mBgmVolume = (mBgmVolume >= 100) ? 0 : mBgmVolume + 25;
                        mAudioMuted = (mBgmVolume == 0);
                        updateVolumeSettings();
                    } else if (mSelectedSettingsOption == 1) {
                        mSfxVolume = (mSfxVolume >= 100) ? 0 : mSfxVolume + 25;
                        mSfxMuted = (mSfxVolume == 0);
                        updateVolumeSettings();
                    } else if (mSelectedSettingsOption == 2) {
                        mState = mSettingsPreviousState;
                    }
                }
                else if (key == SDLK_ESCAPE) {
                    mState = mSettingsPreviousState;
                }
            }
            else if (mState == GameState::PAUSED) {
                if (key == SDLK_W || key == SDLK_UP) {
                    mSelectedPauseOption = (mSelectedPauseOption - 1 + 5) % 5;
                }
                else if (key == SDLK_S || key == SDLK_DOWN) {
                    mSelectedPauseOption = (mSelectedPauseOption + 1) % 5;
                }
                else if (key == SDLK_RETURN || key == SDLK_SPACE) {
                    if (mSelectedPauseOption == 0) {
                        mState = GameState::PLAYING;
                    } else if (mSelectedPauseOption == 1) {
                        scanSaveSlots();
                        mSelectedSaveIndex = 0;
                        mState = GameState::SAVE_DIALOG;
                    } else if (mSelectedPauseOption == 2) {
                        scanSaveSlots();
                        mSelectedLoadIndex = 0;
                        mLoadPreviousState = GameState::PAUSED;
                        mState = GameState::LOAD_DIALOG;
                    } else if (mSelectedPauseOption == 3) {
                        mSettingsPreviousState = GameState::PAUSED;
                        mSelectedSettingsOption = 0;
                        mState = GameState::SETTINGS;
                    } else if (mSelectedPauseOption == 4) {
                        mState = GameState::MENU;
                    }
                }
                else if (key == SDLK_P || key == SDLK_ESCAPE) {
                    mState = GameState::PLAYING;
                }
            }
            else if (mState == GameState::SAVE_DIALOG) {
                if (key == SDLK_W || key == SDLK_UP) {
                    mSelectedSaveIndex = (mSelectedSaveIndex - 1 + 5) % 5;
                }
                else if (key == SDLK_S || key == SDLK_DOWN) {
                    mSelectedSaveIndex = (mSelectedSaveIndex + 1) % 5;
                }
                else if (key == SDLK_DELETE) {
                    if (mSaveSlots[mSelectedSaveIndex].exists) {
                        mPendingDeleteFileName = mSaveSlots[mSelectedSaveIndex].filename;
                    }
                }
                else if (key == SDLK_RETURN || key == SDLK_SPACE) {
                    mPendingSaveSlotIndex = mSelectedSaveIndex;
                }
                else if (key == SDLK_ESCAPE) {
                    mState = GameState::PAUSED;
                }
            }
            else if (mState == GameState::LOAD_DIALOG) {
                if (key == SDLK_W || key == SDLK_UP) {
                    mSelectedLoadIndex = (mSelectedLoadIndex - 1 + 5) % 5;
                }
                else if (key == SDLK_S || key == SDLK_DOWN) {
                    mSelectedLoadIndex = (mSelectedLoadIndex + 1) % 5;
                }
                else if (key == SDLK_DELETE) {
                    if (mSaveSlots[mSelectedLoadIndex].exists) {
                        mPendingDeleteFileName = mSaveSlots[mSelectedLoadIndex].filename;
                    }
                }
                else if (key == SDLK_RETURN || key == SDLK_SPACE) {
                    if (mSaveSlots[mSelectedLoadIndex].exists) {
                        mPendingLoadSlotIndex = mSelectedLoadIndex;
                    }
                }
                else if (key == SDLK_ESCAPE) {
                    mState = mLoadPreviousState;
                }
            }
            else if (mState == GameState::PLAYING) {
                std::lock_guard<std::mutex> lock(mGameMutex);

                if (!event.key.repeat) {
                    if (key == SDLK_P) {
                        mSelectedPauseOption = 0;
                        mState = GameState::PAUSED;
                    }
                    else if (key == SDLK_L) {
                        scanSaveSlots();
                        mSelectedSaveIndex = 0;
                        mState = GameState::SAVE_DIALOG;
                    }
                    else if (key == SDLK_T) {
                        scanSaveSlots();
                        mSelectedLoadIndex = 0;
                        mLoadPreviousState = GameState::PAUSED;
                        mState = GameState::LOAD_DIALOG;
                    }
                    else if (key == SDLK_ESCAPE) {
                        mSelectedPauseOption = 0;
                        mState = GameState::PAUSED;
                    }
                    else {
                        int topLimit = 40;
                        int bottomLimit = 600;
                        if (mIsInfinityMode) {
                            topLimit = (int)mCameraY + 40;
                            bottomLimit = (int)mCameraY + 600;
                        }

                        bool moved = false;
                        if (key == SDLK_W || key == SDLK_UP) {
                            mPlayer.Up(topLimit);
                            moved = true;
                        }
                        else if (key == SDLK_S || key == SDLK_DOWN) {
                            mPlayer.Down(bottomLimit);
                            moved = true;
                        }
                        else if (key == SDLK_A || key == SDLK_LEFT) {
                            mPlayer.Left(0);
                            moved = true;
                        }
                        else if (key == SDLK_D || key == SDLK_RIGHT) {
                            mPlayer.Right(1280 - mPlayer.getWidth());
                            moved = true;
                        }

                        if (moved) {
                            playSFX(mSfxJump);
                        }
                    }
                }
            }
            else if (mState == GameState::GAMEOVER) {
                // Nhấn Y để chơi tiếp, ESC hoặc N quay lại Menu
                if (key == SDLK_Y) {
                    if (mPlayer.isDead()) {
                        // Chết → Retry stage hiện tại (giữ mStage)
                        resetGame();
                        mState = GameState::PLAYING;
                    } else {
                        // Victory → Play Again từ Stage 1
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

        {
            std::lock_guard<std::mutex> lock(mGameMutex);
            if (mState == GameState::PLAYING) {
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

                    if (mPlayer.isFinish() && !mPendingStageAdvance.load()) {
                        if (mStage < MAX_EASY_STAGE) {
                            // Chuyển sang stage tiếp theo
                            mStage++;
                            mPendingStageAdvance.store(true);
                            // Giữ PLAYING, main thread sẽ reset stage
                        } else {
                            // Hoàn thành stage cuối → Victory
                            mState = GameState::GAMEOVER;
                        }
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
                        playSFX(mSfxHit);
                    }
                }
            }
        }

        SDL_Delay(10); // Tick rate 100Hz
    }
}

void CGAME::update(float deltaTime) {
    // Cập nhật bộ đếm animation menu (chạy liên tục cho hiệu ứng sin wave)
    mMenuAnimTimer += deltaTime;

    // Tự động phát lại Nhạc nền BGM liên tục khi bài nhạc phát tới giây cuối cùng
    if (mMixer && mBgmTrack && mBgmMenu && !mAudioMuted && mBgmVolume > 0) {
        if (!MIX_TrackPlaying(mBgmTrack)) {
            playBGM(mBgmTrack, mBgmMenu);
            updateVolumeSettings();
        }
    }

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

    // Xử lý chuyển stage Easy Mode trên main thread (tránh deadlock với physics thread)
    if (mPendingStageAdvance.load()) {
        mPendingStageAdvance.store(false);
        std::lock_guard<std::mutex> lock(mGameMutex);
        mPlayer.resetPosition();
        mFlashTimer = 0.0f;
        setupEasyModeObstacles();
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
    else if (mState == GameState::PAUSED) {
        renderPlaying();
        renderPauseMenu();
    }
    else if (mState == GameState::SAVE_DIALOG) {
        renderPlaying();
        renderSaveDialog();
    }
    else if (mState == GameState::LOAD_DIALOG) {
        if (mLoadPreviousState == GameState::PAUSED) {
            renderPlaying();
        } else {
            renderMenuBackground();
        }
        renderLoadDialog();
    }

    if (!mPendingDeleteFileName.empty()) {
        renderDeleteConfirmDialog();
    }
    else if (mPendingSaveSlotIndex >= 0) {
        renderSaveConfirmDialog();
    }
    else if (mPendingLoadSlotIndex >= 0) {
        renderLoadConfirmDialog();
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
                    mFont.drawTextCentered(mRenderer, "STAGE: " + std::to_string(mStage) + " / " + std::to_string(MAX_EASY_STAGE), 320, 2, whiteColor);
                }
                mFont.drawTextCentered(mRenderer, "PRESS 'Y' TO RETRY", 390, 2, whiteColor);
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
                mFont.drawTextCentered(mRenderer, "YOU HAVE BEATEN ALL 5 STAGES!", 350, 2, whiteColor);
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

    std::string menuOptions[4] = {
        "NEW GAME",
        "LOAD GAME",
        "SETTINGS",
        "EXIT GAME"
    };

    for (int i = 0; i < 4; ++i) {
        int yPos = 390 + i * 55;

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
        "EASY MODE: 1 MAP (SAFE START)",
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

void CGAME::playSFX(MIX_Audio* sfx) {
    if (mSfxMuted || mSfxVolume == 0 || !mMixer || !sfx) return;
    float gain = (float)mSfxVolume / 100.0f;
    int idx = mSfxTrackIndex;
    mSfxTrackIndex = (mSfxTrackIndex + 1) % 4;
    MIX_Track* trk = mSfxTracks[idx];
    if (trk) {
        MIX_SetTrackAudio(trk, sfx);
        MIX_SetTrackGain(trk, gain);
        MIX_PlayTrack(trk, 0);
    } else {
        MIX_PlayAudio(mMixer, sfx);
    }
}

void CGAME::updateVolumeSettings() {
    if (mMixer) {
        MIX_SetMixerGain(mMixer, 1.0f);
        float bgmGain = (mAudioMuted || mBgmVolume == 0) ? 0.0f : ((float)mBgmVolume / 100.0f);
        if (mBgmTrack) {
            MIX_SetTrackGain(mBgmTrack, bgmGain);
        }
        float sfxGain = (mSfxMuted || mSfxVolume == 0) ? 0.0f : ((float)mSfxVolume / 100.0f);
        for (int i = 0; i < 4; ++i) {
            if (mSfxTracks[i]) {
                MIX_SetTrackGain(mSfxTracks[i], sfxGain);
            }
        }
    }
}



void CGAME::renderSettings() {
    if (mSettingsPreviousState == GameState::PAUSED) {
        renderPlaying();
        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 130);
        SDL_FRect overlay = { 0, 0, 1280.0f, 720.0f };
        SDL_RenderFillRect(mRenderer, &overlay);
    } else {
        renderMenuBackground();
    }

    SDL_Color titleShadow = {0, 0, 0, 80};
    SDL_Color titleColor = {255, 255, 255, 255};
    SDL_Color subtitleColor = {220, 245, 255, 255};
    mFont.drawTextCentered(mRenderer, "SETTINGS", 98, 4, titleShadow);
    mFont.drawTextCentered(mRenderer, "SETTINGS", 96, 4, titleColor);
    mFont.drawTextCentered(mRenderer, "AUDIO & VOLUME CONTROLS", 146, 2, subtitleColor);

    float panelX = 240.0f, panelY = 210.0f;
    float panelW = 800.0f, panelH = 430.0f;

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

    std::string labels[3] = {
        "MUSIC BGM :",
        "SOUND SFX :",
        "BACK TO PREVIOUS MENU"
    };

    SDL_Color normalColor = {30, 60, 50, 255};
    SDL_Color selectColor = {10, 95, 75, 255};

    for (int i = 0; i < 3; ++i) {
        int yPos = 265 + i * 85;

        if (mSelectedSettingsOption == i) {
            SDL_SetRenderDrawColor(mRenderer, 20, 120, 100, 60);
            SDL_FRect highlight = { panelX + 20, (float)yPos - 15, panelW - 40, 65.0f };
            SDL_RenderFillRect(mRenderer, &highlight);

            SDL_SetRenderDrawColor(mRenderer, 20, 140, 100, 255);
            SDL_FRect hlBorder = { panelX + 20, (float)yPos - 15, 4.0f, 65.0f };
            SDL_RenderFillRect(mRenderer, &hlBorder);

            float arrowOffset = sinf(mMenuAnimTimer * 5.0f) * 4.0f;
            mFont.drawText(mRenderer, ">", (int)(panelX + 40 + arrowOffset), yPos, 3, selectColor);
            mFont.drawText(mRenderer, labels[i], (int)(panelX + 80), yPos, 3, selectColor);
        } else {
            mFont.drawText(mRenderer, labels[i], (int)(panelX + 80), yPos, 3, normalColor);
        }

        // Draw Volume Progress Bar & Percentage text for Music and SFX
        if (i < 2) {
            int vol = (i == 0) ? mBgmVolume : mSfxVolume;

            // 1. Progress Bar (X = panelX + 380)
            float barX = panelX + 380.0f;
            float barY = (float)yPos + 4.0f;
            float barW = 160.0f;
            float barH = 22.0f;

            SDL_SetRenderDrawColor(mRenderer, 50, 70, 65, 200);
            SDL_FRect barBg = { barX, barY, barW, barH };
            SDL_RenderFillRect(mRenderer, &barBg);

            float fillW = barW * ((float)vol / 100.0f);
            if (fillW > 0.0f) {
                SDL_SetRenderDrawColor(mRenderer, 80, 200, 255, 255);
                SDL_FRect barFill = { barX, barY, fillW, barH };
                SDL_RenderFillRect(mRenderer, &barFill);
            }

            SDL_SetRenderDrawColor(mRenderer, 20, 140, 100, 255);
            SDL_RenderRect(mRenderer, &barBg);

            // 2. Percentage text (X = panelX + 560 -> fits nicely inside panel boundary)
            std::string pctText = "< " + (vol == 0 ? "OFF" : std::to_string(vol) + "%") + " >";
            SDL_Color textCol = (mSelectedSettingsOption == i) ? selectColor : normalColor;
            mFont.drawText(mRenderer, pctText, (int)(panelX + 560), yPos, 3, textCol);
        }
    }

    SDL_Color guideColor = {255, 255, 255, 200};
    mFont.drawTextCentered(mRenderer, "W/S TO SELECT  -  A/D OR LEFT/RIGHT TO ADJUST VOLUME  -  ESC TO RETURN", 655, 1, guideColor);
}

void CGAME::renderPauseMenu() {
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 130);
    SDL_FRect overlay = { 0, 0, 1280.0f, 720.0f };
    SDL_RenderFillRect(mRenderer, &overlay);

    SDL_Color titleShadow = {0, 0, 0, 80};
    SDL_Color titleColor = {255, 255, 255, 255};
    SDL_Color subtitleColor = {220, 245, 255, 255};
    mFont.drawTextCentered(mRenderer, "GAME PAUSED", 98, 4, titleShadow);
    mFont.drawTextCentered(mRenderer, "GAME PAUSED", 96, 4, titleColor);
    mFont.drawTextCentered(mRenderer, "SYSTEM & GAME CONTROL OPTIONS", 146, 2, subtitleColor);

    float panelX = 340.0f, panelY = 200.0f;
    float panelW = 600.0f, panelH = 430.0f;

    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 175);
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

    std::string options[5] = {
        "1. RESUME GAME",
        "2. SAVE GAME  (L)",
        "3. LOAD GAME  (T)",
        "4. SETTINGS",
        "5. MAIN MENU"
    };

    SDL_Color normalColor = {30, 60, 50, 255};
    SDL_Color selectColor = {10, 95, 75, 255};

    for (int i = 0; i < 5; ++i) {
        int yPos = 245 + i * 65;

        if (mSelectedPauseOption == i) {
            SDL_SetRenderDrawColor(mRenderer, 20, 120, 100, 60);
            SDL_FRect highlight = { panelX + 20, (float)yPos - 12, panelW - 40, 50.0f };
            SDL_RenderFillRect(mRenderer, &highlight);

            SDL_SetRenderDrawColor(mRenderer, 20, 140, 100, 255);
            SDL_FRect hlBorder = { panelX + 20, (float)yPos - 12, 4.0f, 50.0f };
            SDL_RenderFillRect(mRenderer, &hlBorder);

            float arrowOffset = sinf(mMenuAnimTimer * 5.0f) * 4.0f;
            mFont.drawText(mRenderer, ">", (int)(panelX + 40 + arrowOffset), yPos, 3, selectColor);
            mFont.drawText(mRenderer, options[i], (int)(panelX + 80), yPos, 3, selectColor);
        } else {
            mFont.drawText(mRenderer, options[i], (int)(panelX + 80), yPos, 3, normalColor);
        }
    }

    SDL_Color guideColor = {255, 255, 255, 200};
    mFont.drawTextCentered(mRenderer, "W/S TO SELECT  -  ENTER TO CHOOSE  -  P / ESC TO RESUME", 655, 1, guideColor);
}

void CGAME::renderSaveDialog() {
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 130);
    SDL_FRect overlay = { 0, 0, 1280.0f, 720.0f };
    SDL_RenderFillRect(mRenderer, &overlay);

    SDL_Color titleShadow = {0, 0, 0, 80};
    SDL_Color titleColor = {255, 255, 255, 255};
    SDL_Color subtitleColor = {220, 245, 255, 255};
    SDL_Color warnSubtitleColor = {255, 180, 180, 255};

    mFont.drawTextCentered(mRenderer, "SAVE GAME", 98, 4, titleShadow);
    mFont.drawTextCentered(mRenderer, "SAVE GAME", 96, 4, titleColor);

    mFont.drawTextCentered(mRenderer, "SELECT A SLOT TO SAVE YOUR PROGRESS", 146, 2, subtitleColor);

    float panelX = 240.0f, panelY = 210.0f;
    float panelW = 800.0f, panelH = 430.0f;

    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 175);
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

    SDL_Color normalColor = {30, 60, 50, 255};
    SDL_Color selectColor = {10, 95, 75, 255};
    SDL_Color emptyColor  = {120, 140, 135, 255};

    int startY = 245;
    for (int i = 0; i < 5; ++i) {
        int yPos = startY + i * 65;

        if (mSelectedSaveIndex == i) {
            SDL_SetRenderDrawColor(mRenderer, 20, 120, 100, 60);
            SDL_FRect highlight = { panelX + 20, (float)yPos - 10, panelW - 40, 52.0f };
            SDL_RenderFillRect(mRenderer, &highlight);

            SDL_SetRenderDrawColor(mRenderer, 20, 140, 100, 255);
            SDL_FRect hlBorder = { panelX + 20, (float)yPos - 10, 4.0f, 52.0f };
            SDL_RenderFillRect(mRenderer, &hlBorder);

            float arrowOffset = sinf(mMenuAnimTimer * 5.0f) * 4.0f;
            mFont.drawText(mRenderer, ">", (int)(panelX + 40 + arrowOffset), yPos, 2, selectColor);

            std::string slotName = "SLOT " + std::to_string(i + 1);
            mFont.drawText(mRenderer, slotName, (int)(panelX + 70), yPos, 2, selectColor);

            if (mSaveSlots[i].exists) {
                std::string timeLabel = mSaveSlots[i].timestamp;
                std::string scoreLabel = "SCORE: " + std::to_string(mSaveSlots[i].score);
                mFont.drawText(mRenderer, timeLabel, (int)(panelX + 220), yPos, 2, selectColor);
                mFont.drawText(mRenderer, scoreLabel, (int)(panelX + 480), yPos, 2, selectColor);
            } else {
                mFont.drawText(mRenderer, "[ EMPTY ]", (int)(panelX + 220), yPos, 2, emptyColor);
            }
        } else {
            SDL_Color col = mSaveSlots[i].exists ? normalColor : emptyColor;
            std::string slotName = "SLOT " + std::to_string(i + 1);
            mFont.drawText(mRenderer, slotName, (int)(panelX + 70), yPos, 2, col);

            if (mSaveSlots[i].exists) {
                std::string timeLabel = mSaveSlots[i].timestamp;
                std::string scoreLabel = "SCORE: " + std::to_string(mSaveSlots[i].score);
                mFont.drawText(mRenderer, timeLabel, (int)(panelX + 220), yPos, 2, col);
                mFont.drawText(mRenderer, scoreLabel, (int)(panelX + 480), yPos, 2, col);
            } else {
                mFont.drawText(mRenderer, "[ EMPTY ]", (int)(panelX + 220), yPos, 2, emptyColor);
            }
        }

        // Delete button [X] for existing saves
        if (mSaveSlots[i].exists) {
            float btnX = panelX + panelW - 75.0f;
            float btnY = (float)yPos - 6.0f;
            SDL_SetRenderDrawColor(mRenderer, 220, 40, 40, 220);
            SDL_FRect delBtn = { btnX, btnY, 45.0f, 35.0f };
            SDL_RenderFillRect(mRenderer, &delBtn);

            SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
            SDL_RenderRect(mRenderer, &delBtn);

            SDL_Color whiteCol = { 255, 255, 255, 255 };
            mFont.drawTextCenteredInBox(mRenderer, "X", btnX, btnY, 45.0f, 35.0f, 2, whiteCol);
        }
    }

    SDL_Color guideColor = {255, 255, 255, 200};
    mFont.drawTextCentered(mRenderer, "UP / DOWN TO SELECT  -  ENTER TO SAVE  -  DEL / [X] TO DELETE  -  ESC TO CANCEL", 655, 1, guideColor);
}

void CGAME::renderLoadDialog() {
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 130);
    SDL_FRect overlay = { 0, 0, 1280.0f, 720.0f };
    SDL_RenderFillRect(mRenderer, &overlay);

    SDL_Color titleShadow = {0, 0, 0, 80};
    SDL_Color titleColor = {255, 255, 255, 255};
    SDL_Color subtitleColor = {220, 245, 255, 255};
    mFont.drawTextCentered(mRenderer, "LOAD GAME", 98, 4, titleShadow);
    mFont.drawTextCentered(mRenderer, "LOAD GAME", 96, 4, titleColor);
    mFont.drawTextCentered(mRenderer, "SELECT A SAVED GAME SLOT TO RESUME", 146, 2, subtitleColor);

    float panelX = 240.0f, panelY = 210.0f;
    float panelW = 800.0f, panelH = 430.0f;

    SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 175);
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

    SDL_Color normalColor = {30, 60, 50, 255};
    SDL_Color selectColor = {10, 95, 75, 255};
    SDL_Color emptyColor  = {120, 140, 135, 255};

    int startY = 245;
    for (int i = 0; i < 5; ++i) {
        int yPos = startY + i * 65;

        if (mSelectedLoadIndex == i) {
            SDL_SetRenderDrawColor(mRenderer, 20, 120, 100, 60);
            SDL_FRect highlight = { panelX + 20, (float)yPos - 10, panelW - 40, 52.0f };
            SDL_RenderFillRect(mRenderer, &highlight);

            SDL_SetRenderDrawColor(mRenderer, 20, 140, 100, 255);
            SDL_FRect hlBorder = { panelX + 20, (float)yPos - 10, 4.0f, 52.0f };
            SDL_RenderFillRect(mRenderer, &hlBorder);

            float arrowOffset = sinf(mMenuAnimTimer * 5.0f) * 4.0f;
            mFont.drawText(mRenderer, ">", (int)(panelX + 40 + arrowOffset), yPos, 2, selectColor);

            std::string slotName = "SLOT " + std::to_string(i + 1);
            mFont.drawText(mRenderer, slotName, (int)(panelX + 70), yPos, 2, selectColor);

            if (mSaveSlots[i].exists) {
                std::string timeLabel = mSaveSlots[i].timestamp;
                std::string scoreLabel = "SCORE: " + std::to_string(mSaveSlots[i].score);
                mFont.drawText(mRenderer, timeLabel, (int)(panelX + 220), yPos, 2, selectColor);
                mFont.drawText(mRenderer, scoreLabel, (int)(panelX + 480), yPos, 2, selectColor);
            } else {
                mFont.drawText(mRenderer, "[ EMPTY ]", (int)(panelX + 220), yPos, 2, emptyColor);
            }
        } else {
            SDL_Color col = mSaveSlots[i].exists ? normalColor : emptyColor;
            std::string slotName = "SLOT " + std::to_string(i + 1);
            mFont.drawText(mRenderer, slotName, (int)(panelX + 70), yPos, 2, col);

            if (mSaveSlots[i].exists) {
                std::string timeLabel = mSaveSlots[i].timestamp;
                std::string scoreLabel = "SCORE: " + std::to_string(mSaveSlots[i].score);
                mFont.drawText(mRenderer, timeLabel, (int)(panelX + 220), yPos, 2, col);
                mFont.drawText(mRenderer, scoreLabel, (int)(panelX + 480), yPos, 2, col);
            } else {
                mFont.drawText(mRenderer, "[ EMPTY ]", (int)(panelX + 220), yPos, 2, emptyColor);
            }
        }

        // Delete button [X] for existing saves
        if (mSaveSlots[i].exists) {
            float btnX = panelX + panelW - 75.0f;
            float btnY = (float)yPos - 6.0f;
            SDL_SetRenderDrawColor(mRenderer, 220, 40, 40, 220);
            SDL_FRect delBtn = { btnX, btnY, 45.0f, 35.0f };
            SDL_RenderFillRect(mRenderer, &delBtn);

            SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
            SDL_RenderRect(mRenderer, &delBtn);

            SDL_Color whiteCol = { 255, 255, 255, 255 };
            mFont.drawTextCenteredInBox(mRenderer, "X", btnX, btnY, 45.0f, 35.0f, 2, whiteCol);
        }
    }

    SDL_Color guideColor = {255, 255, 255, 200};
    if (mLoadPreviousState == GameState::PAUSED) {
        mFont.drawTextCentered(mRenderer, "UP/DOWN TO SELECT  -  ENTER TO LOAD  -  DEL / [X] TO DELETE  -  ESC BACK TO PAUSE", 655, 1, guideColor);
    } else {
        mFont.drawTextCentered(mRenderer, "UP/DOWN TO SELECT  -  ENTER TO LOAD  -  DEL / [X] TO DELETE  -  ESC TO BACK", 655, 1, guideColor);
    }
}

void CGAME::renderDeleteConfirmDialog() {
    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 180);
    SDL_FRect overlay = { 0, 0, 1280.0f, 720.0f };
    SDL_RenderFillRect(mRenderer, &overlay);

    float panelX = 340.0f, panelY = 210.0f;
    float panelW = 600.0f, panelH = 280.0f;

    SDL_SetRenderDrawColor(mRenderer, 255, 240, 240, 245);
    SDL_FRect panelBg = { panelX, panelY, panelW, panelH };
    SDL_RenderFillRect(mRenderer, &panelBg);

    SDL_SetRenderDrawColor(mRenderer, 214, 40, 40, 255);
    SDL_FRect borderTop    = { panelX, panelY, panelW, 4.0f };
    SDL_FRect borderBottom = { panelX, panelY + panelH - 4, panelW, 4.0f };
    SDL_FRect borderLeft   = { panelX, panelY, 4.0f, panelH };
    SDL_FRect borderRight  = { panelX + panelW - 4, panelY, 4.0f, panelH };
    SDL_RenderFillRect(mRenderer, &borderTop);
    SDL_RenderFillRect(mRenderer, &borderBottom);
    SDL_RenderFillRect(mRenderer, &borderLeft);
    SDL_RenderFillRect(mRenderer, &borderRight);

    SDL_Color titleColor = { 180, 20, 20, 255 };
    SDL_Color msgColor = { 40, 40, 40, 255 };
    SDL_Color fileColor = { 20, 100, 150, 255 };
    SDL_Color warnColor = { 200, 30, 30, 255 };

    mFont.drawTextCenteredInBox(mRenderer, "DELETE SAVE FILE", panelX, panelY + 15, panelW, 30, 2, titleColor);
    mFont.drawTextCenteredInBox(mRenderer, "ARE YOU SURE YOU WANT TO DELETE:", panelX, panelY + 55, panelW, 25, 2, msgColor);
    
    std::string fileStr = "[ " + mPendingDeleteFileName + " ]";
    mFont.drawTextCenteredInBox(mRenderer, fileStr, panelX, panelY + 95, panelW, 25, 2, fileColor);
    mFont.drawTextCenteredInBox(mRenderer, "THIS ACTION CANNOT BE UNDONE!", panelX, panelY + 145, panelW, 20, 1, warnColor);

    // Nút YES [Y]
    float btnYesX = 370.0f, btnY = panelY + 195.0f, btnW = 240.0f, btnH = 50.0f;
    SDL_SetRenderDrawColor(mRenderer, 200, 40, 40, 255);
    SDL_FRect btnYes = { btnYesX, btnY, btnW, btnH };
    SDL_RenderFillRect(mRenderer, &btnYes);
    SDL_Color btnTextCol = { 255, 255, 255, 255 };
    mFont.drawTextCenteredInBox(mRenderer, "[ Y ] YES", btnYesX, btnY, btnW, btnH, 2, btnTextCol);

    // Nút CANCEL [N]
    float btnNoX = 670.0f;
    SDL_SetRenderDrawColor(mRenderer, 80, 100, 90, 255);
    SDL_FRect btnNo = { btnNoX, btnY, btnW, btnH };
    SDL_RenderFillRect(mRenderer, &btnNo);
    mFont.drawTextCenteredInBox(mRenderer, "[ N ] CANCEL", btnNoX, btnY, btnW, btnH, 2, btnTextCol);
}

void CGAME::renderSaveConfirmDialog() {
    if (mPendingSaveSlotIndex < 0 || mPendingSaveSlotIndex >= 5) return;

    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 180);
    SDL_FRect overlay = { 0, 0, 1280.0f, 720.0f };
    SDL_RenderFillRect(mRenderer, &overlay);

    float panelX = 340.0f, panelY = 210.0f;
    float panelW = 600.0f, panelH = 280.0f;

    SDL_SetRenderDrawColor(mRenderer, 240, 255, 250, 245);
    SDL_FRect panelBg = { panelX, panelY, panelW, panelH };
    SDL_RenderFillRect(mRenderer, &panelBg);

    SDL_SetRenderDrawColor(mRenderer, 20, 140, 100, 255);
    SDL_FRect borderTop    = { panelX, panelY, panelW, 4.0f };
    SDL_FRect borderBottom = { panelX, panelY + panelH - 4, panelW, 4.0f };
    SDL_FRect borderLeft   = { panelX, panelY, 4.0f, panelH };
    SDL_FRect borderRight  = { panelX + panelW - 4, panelY, 4.0f, panelH };
    SDL_RenderFillRect(mRenderer, &borderTop);
    SDL_RenderFillRect(mRenderer, &borderBottom);
    SDL_RenderFillRect(mRenderer, &borderLeft);
    SDL_RenderFillRect(mRenderer, &borderRight);

    SDL_Color titleColor = { 10, 110, 80, 255 };
    SDL_Color msgColor = { 40, 40, 40, 255 };
    SDL_Color slotColor = { 20, 100, 150, 255 };
    SDL_Color warnColor = { 200, 50, 50, 255 };

    mFont.drawTextCenteredInBox(mRenderer, "SAVE GAME CONFIRMATION", panelX, panelY + 15, panelW, 30, 2, titleColor);
    
    std::string slotStr = "SLOT " + std::to_string(mPendingSaveSlotIndex + 1);
    if (mSaveSlots[mPendingSaveSlotIndex].exists) {
        mFont.drawTextCenteredInBox(mRenderer, "OVERWRITE GAME DATA IN:", panelX, panelY + 55, panelW, 25, 2, msgColor);
        std::string infoStr = "[ " + slotStr + " : " + mSaveSlots[mPendingSaveSlotIndex].timestamp + " ]";
        mFont.drawTextCenteredInBox(mRenderer, infoStr, panelX, panelY + 95, panelW, 25, 2, slotColor);
        mFont.drawTextCenteredInBox(mRenderer, "PREVIOUS DATA IN THIS SLOT WILL BE REPLACED!", panelX, panelY + 145, panelW, 20, 1, warnColor);
    } else {
        mFont.drawTextCenteredInBox(mRenderer, "SAVE CURRENT PROGRESS TO:", panelX, panelY + 55, panelW, 25, 2, msgColor);
        std::string infoStr = "[ " + slotStr + " : NEW SAVE ]";
        mFont.drawTextCenteredInBox(mRenderer, infoStr, panelX, panelY + 95, panelW, 25, 2, slotColor);
        mFont.drawTextCenteredInBox(mRenderer, "GAME STATE WILL BE SAVED TO THIS SLOT", panelX, panelY + 145, panelW, 20, 1, titleColor);
    }

    // Nút YES [Y]
    float btnYesX = 370.0f, btnY = panelY + 195.0f, btnW = 240.0f, btnH = 50.0f;
    SDL_SetRenderDrawColor(mRenderer, 20, 140, 100, 255);
    SDL_FRect btnYes = { btnYesX, btnY, btnW, btnH };
    SDL_RenderFillRect(mRenderer, &btnYes);
    SDL_Color btnTextCol = { 255, 255, 255, 255 };
    mFont.drawTextCenteredInBox(mRenderer, "[ Y ] YES", btnYesX, btnY, btnW, btnH, 2, btnTextCol);

    // Nút CANCEL [N]
    float btnNoX = 670.0f;
    SDL_SetRenderDrawColor(mRenderer, 80, 100, 90, 255);
    SDL_FRect btnNo = { btnNoX, btnY, btnW, btnH };
    SDL_RenderFillRect(mRenderer, &btnNo);
    mFont.drawTextCenteredInBox(mRenderer, "[ N ] CANCEL", btnNoX, btnY, btnW, btnH, 2, btnTextCol);
}

void CGAME::renderLoadConfirmDialog() {
    if (mPendingLoadSlotIndex < 0 || mPendingLoadSlotIndex >= 5) return;

    SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 180);
    SDL_FRect overlay = { 0, 0, 1280.0f, 720.0f };
    SDL_RenderFillRect(mRenderer, &overlay);

    float panelX = 340.0f, panelY = 210.0f;
    float panelW = 600.0f, panelH = 280.0f;

    SDL_SetRenderDrawColor(mRenderer, 240, 250, 255, 245);
    SDL_FRect panelBg = { panelX, panelY, panelW, panelH };
    SDL_RenderFillRect(mRenderer, &panelBg);

    SDL_SetRenderDrawColor(mRenderer, 20, 100, 180, 255);
    SDL_FRect borderTop    = { panelX, panelY, panelW, 4.0f };
    SDL_FRect borderBottom = { panelX, panelY + panelH - 4, panelW, 4.0f };
    SDL_FRect borderLeft   = { panelX, panelY, 4.0f, panelH };
    SDL_FRect borderRight  = { panelX + panelW - 4, panelY, 4.0f, panelH };
    SDL_RenderFillRect(mRenderer, &borderTop);
    SDL_RenderFillRect(mRenderer, &borderBottom);
    SDL_RenderFillRect(mRenderer, &borderLeft);
    SDL_RenderFillRect(mRenderer, &borderRight);

    SDL_Color titleColor = { 20, 90, 160, 255 };
    SDL_Color msgColor = { 40, 40, 40, 255 };
    SDL_Color slotColor = { 10, 110, 80, 255 };
    SDL_Color warnColor = { 200, 50, 50, 255 };

    mFont.drawTextCenteredInBox(mRenderer, "LOAD GAME CONFIRMATION", panelX, panelY + 15, panelW, 30, 2, titleColor);
    mFont.drawTextCenteredInBox(mRenderer, "LOAD SAVED PROGRESS FROM:", panelX, panelY + 55, panelW, 25, 2, msgColor);
    
    std::string slotStr = "SLOT " + std::to_string(mPendingLoadSlotIndex + 1);
    std::string infoStr = "[ " + slotStr + " : " + mSaveSlots[mPendingLoadSlotIndex].timestamp + " ]";
    mFont.drawTextCenteredInBox(mRenderer, infoStr, panelX, panelY + 95, panelW, 25, 2, slotColor);
    mFont.drawTextCenteredInBox(mRenderer, "UNSAVED CURRENT PROGRESS WILL BE LOST!", panelX, panelY + 145, panelW, 20, 1, warnColor);

    // Nút YES [Y]
    float btnYesX = 370.0f, btnY = panelY + 195.0f, btnW = 240.0f, btnH = 50.0f;
    SDL_SetRenderDrawColor(mRenderer, 20, 100, 180, 255);
    SDL_FRect btnYes = { btnYesX, btnY, btnW, btnH };
    SDL_RenderFillRect(mRenderer, &btnYes);
    SDL_Color btnTextCol = { 255, 255, 255, 255 };
    mFont.drawTextCenteredInBox(mRenderer, "[ Y ] YES", btnYesX, btnY, btnW, btnH, 2, btnTextCol);

    // Nút CANCEL [N]
    float btnNoX = 670.0f;
    SDL_SetRenderDrawColor(mRenderer, 80, 100, 90, 255);
    SDL_FRect btnNo = { btnNoX, btnY, btnW, btnH };
    SDL_RenderFillRect(mRenderer, &btnNo);
    mFont.drawTextCenteredInBox(mRenderer, "[ N ] CANCEL", btnNoX, btnY, btnW, btnH, 2, btnTextCol);
}

void CGAME::scanSaveSlots() {
    try {
        std::filesystem::create_directories("saves");
    } catch (...) {}

    for (int i = 0; i < 5; ++i) {
        std::string fname = "slot" + std::to_string(i + 1) + ".txt";
        std::string fullPath = "saves/" + fname;

        mSaveSlots[i].filename = fname;
        try {
            if (!std::filesystem::exists(fullPath)) {
                mSaveSlots[i].exists = false;
                mSaveSlots[i].timestamp = "";
                mSaveSlots[i].score = 0;
                mSaveSlots[i].mode = "";
                mSaveSlots[i].stage = 1;
                continue;
            }

            mSaveSlots[i].exists = true;
            mSaveSlots[i].score = 0;
            mSaveSlots[i].mode = "STAGE";
            mSaveSlots[i].stage = 1;
            mSaveSlots[i].timestamp = "---";

            std::ifstream inFile(fullPath);
            if (inFile.is_open()) {
                std::string line;
                std::string currentSection = "";
                while (std::getline(inFile, line)) {
                    if (line.empty() || line[0] == '#') continue;
                    if (line[0] == '[') {
                        currentSection = line;
                        continue;
                    }
                    if (currentSection == "[HEADER]") {
                        std::stringstream ss(line);
                        std::string key, val;
                        if (std::getline(ss, key, '=') && std::getline(ss, val)) {
                            if (key == "mode") {
                                int mVal = 0;
                                try { mVal = std::stoi(val); } catch (...) { mVal = 0; }
                                mSaveSlots[i].mode = (mVal == 1) ? "INFINITE" : "STAGE";
                            } else if (key == "stage") {
                                try { mSaveSlots[i].stage = std::stoi(val); } catch (...) { mSaveSlots[i].stage = 1; }
                            } else if (key == "score") {
                                try { mSaveSlots[i].score = std::stoi(val); } catch (...) { mSaveSlots[i].score = 0; }
                            } else if (key == "date") {
                                if (val.length() >= 16 && val[4] == '-') {
                                    val = val.substr(5);
                                }
                                mSaveSlots[i].timestamp = val;
                            }
                        }
                    }
                }
                inFile.close();
                if (mSaveSlots[i].mode == "STAGE") {
                    mSaveSlots[i].mode = "STAGE " + std::to_string(mSaveSlots[i].stage);
                }
            }
        } catch (...) {
            mSaveSlots[i].exists = false;
        }
    }
}

bool CGAME::saveGame(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= 5) return false;
    std::string actualName = "slot" + std::to_string(slotIndex + 1) + ".txt";

    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    struct tm timeinfo;
#if defined(_MSC_VER)
    localtime_s(&timeinfo, &in_time_t);
#else
    localtime_r(&in_time_t, &timeinfo);
#endif
    std::stringstream timeSs;
    timeSs << std::put_time(&timeinfo, "%m-%d %H:%M");
    std::string dateStr = timeSs.str();

    try {
        std::filesystem::create_directories("saves");
        std::ofstream outFile("saves/" + actualName);
        if (!outFile.is_open()) return false;

        std::lock_guard<std::mutex> lock(mGameMutex);

        outFile << "[HEADER]\n";
        outFile << "version=1\n";
        outFile << "date=" << dateStr << "\n";
        outFile << "mode=" << (mIsInfinityMode ? 1 : 0) << "\n";
        outFile << "stage=" << mStage << "\n";
        outFile << "score=" << mScore << "\n";
        outFile << "max_reached_y=" << mMaxReachedY << "\n";
        outFile << "camera_y=" << mCameraY << "\n";
        outFile << "infinite_level=" << mInfiniteLevel << "\n";
        outFile << "lane_pattern_index=" << mLanePatternIndex << "\n\n";

        outFile << "[PLAYER]\n";
        outFile << "char_type=" << mSelectedCharOption << "\n";
        outFile << "x=" << mPlayer.getX() << "\n";
        outFile << "y=" << mPlayer.getY() << "\n";
        outFile << "state=" << (mPlayer.isDead() ? 0 : 1) << "\n\n";

        outFile << "[TRAFFIC_LIGHTS]\n";
        outFile << "count=" << mTrafficLights.size() << "\n";
        for (size_t i = 0; i < mTrafficLights.size(); ++i) {
            outFile << mTrafficLights[i].getLaneY() << " "
                    << (mTrafficLights[i].isRed() ? 1 : 0) << " "
                    << mTrafficLights[i].getTimer() << " "
                    << mTrafficLights[i].getRedDuration() << " "
                    << mTrafficLights[i].getGreenDuration() << "\n";
        }
        outFile << "\n";

        outFile << "[LANES]\n";
        outFile << "count=" << mLanes.size() << "\n";
        for (size_t i = 0; i < mLanes.size(); ++i) {
            std::string typeStr = (mLanes[i].type == LaneType::REST) ? "REST" :
                                  (mLanes[i].type == LaneType::VEHICLE ? "VEHICLE" : "MONSTER");
            outFile << typeStr << " " << mLanes[i].worldY << "\n";
        }
        outFile << "\n";

        outFile << "[BLUEWINGS]\n";
        outFile << "count=" << mBluewings.size() << "\n";
        for (auto bw : mBluewings) {
            outFile << bw->getX() << " " << bw->getY() << " " << bw->getSpeed() << " " << bw->getDirection() << "\n";
        }
        outFile << "\n";

        outFile << "[SKYARMORS]\n";
        outFile << "count=" << mSkyarmors.size() << "\n";
        for (auto sa : mSkyarmors) {
            outFile << sa->getX() << " " << sa->getY() << " " << sa->getSpeed() << " " << sa->getDirection() << "\n";
        }
        outFile << "\n";

        outFile << "[GLEAMEYES]\n";
        outFile << "count=" << mGleameyes.size() << "\n";
        for (auto ge : mGleameyes) {
            outFile << ge->getX() << " " << ge->getY() << " " << ge->getSpeed() << " " << ge->getDirection() << "\n";
        }
        outFile << "\n";

        outFile << "[CHEATHCLIFFS]\n";
        outFile << "count=" << mCheathcliffs.size() << "\n";
        for (auto hc : mCheathcliffs) {
            outFile << hc->getX() << " " << hc->getY() << " " << hc->getSpeed() << " " << hc->getDirection() << "\n";
        }
        outFile << "\n";

        outFile << "[CILLFANGS]\n";
        outFile << "count=" << mCillfangs.size() << "\n";
        for (auto cf : mCillfangs) {
            outFile << cf->getX() << " " << cf->getY() << " " << cf->getSpeed() << " " << cf->getDirection() << "\n";
        }
        outFile << "\n";

        outFile << "[ICEDRAGONS]\n";
        outFile << "count=" << mCicedragons.size() << "\n";
        for (auto id : mCicedragons) {
            outFile << id->getX() << " " << id->getY() << " " << id->getSpeed() << " " << id->getDirection() << "\n";
        }
        outFile << "\n";

        outFile.flush();
        bool success = outFile.good();
        outFile.close();
        if (success) {
            scanSaveSlots();
        }
        return success;
    } catch (...) {
        return false;
    }
}

bool CGAME::loadGame(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= 5) return false;
    std::string filename = "slot" + std::to_string(slotIndex + 1) + ".txt";
    std::string fullPath = "saves/" + filename;
    try {
        if (!std::filesystem::exists(fullPath)) return false;
    } catch (...) {
        return false;
    }

    // Temporary snapshot variables to parse file before touching active game state
    bool parsedIsInfinityMode = false;
    int parsedStage = 1;
    int parsedScore = 0;
    int parsedMaxReachedY = 0;
    float parsedCameraY = 0.0f;
    int parsedInfiniteLevel = 1;
    int parsedLanePatternIndex = 0;

    int parsedCharType = 0;
    int parsedPlayerX = 600, parsedPlayerY = 600, parsedPlayerState = 1;

    std::vector<CTRAFFICLIGHT> tempTrafficLights;
    std::vector<Lane> tempLanes;
    std::vector<CBLUEWING*> tempBluewings;
    std::vector<CSKYARMOR*> tempSkyarmors;
    std::vector<CGLEAMEYES*> tempGleameyes;
    std::vector<CHEATHCLIFF*> tempCheathcliffs;
    std::vector<CILLFANG*> tempCillfangs;
    std::vector<CICEDRAGON*> tempCicedragons;

    try {
        std::ifstream inFile(fullPath);
        if (!inFile.is_open()) return false;

        std::string line;
        std::string currentSection = "";

        while (std::getline(inFile, line)) {
            if (line.empty() || line[0] == '#') continue;
            if (line[0] == '[') {
                currentSection = line;
                continue;
            }

            std::stringstream ss(line);
            if (currentSection == "[HEADER]") {
                std::string key, val;
                if (std::getline(ss, key, '=') && std::getline(ss, val)) {
                    if (key == "mode") parsedIsInfinityMode = (std::stoi(val) == 1);
                    else if (key == "stage") parsedStage = std::stoi(val);
                    else if (key == "score") parsedScore = std::stoi(val);
                    else if (key == "max_reached_y") parsedMaxReachedY = std::stoi(val);
                    else if (key == "camera_y") parsedCameraY = std::stof(val);
                    else if (key == "infinite_level") parsedInfiniteLevel = std::stoi(val);
                    else if (key == "lane_pattern_index") parsedLanePatternIndex = std::stoi(val);
                }
            }
            else if (currentSection == "[PLAYER]") {
                std::string key, val;
                if (std::getline(ss, key, '=') && std::getline(ss, val)) {
                    if (key == "char_type") parsedCharType = std::stoi(val);
                    else if (key == "x") parsedPlayerX = std::stoi(val);
                    else if (key == "y") parsedPlayerY = std::stoi(val);
                    else if (key == "state") parsedPlayerState = std::stoi(val);
                }
            }
            else if (currentSection == "[TRAFFIC_LIGHTS]") {
                if (line.rfind("count=", 0) == 0) continue;
                int laneY = 0, isRedInt = 0;
                float timer = 0.0f, redDur = 3.0f, greenDur = 5.0f;
                if (ss >> laneY >> isRedInt >> timer >> redDur >> greenDur) {
                    CTRAFFICLIGHT light(laneY, redDur, greenDur);
                    light.setState(isRedInt == 1, timer);
                    tempTrafficLights.push_back(light);
                }
            }
            else if (currentSection == "[LANES]") {
                if (line.rfind("count=", 0) == 0) continue;
                std::string typeStr;
                int worldY = 0;
                if (ss >> typeStr >> worldY) {
                    LaneType lt = (typeStr == "REST") ? LaneType::REST :
                                 (typeStr == "VEHICLE" ? LaneType::VEHICLE : LaneType::MONSTER);
                    tempLanes.push_back({ lt, worldY });
                }
            }
            else if (currentSection == "[BLUEWINGS]") {
                if (line.rfind("count=", 0) == 0) continue;
                int x = 0, y = 0, speed = 0, dir = 0;
                if (ss >> x >> y >> speed >> dir) {
                    CBLUEWING* bw = new CBLUEWING(x, y, speed, dir);
                    bw->setTexture(mCbluewingTexture);
                    tempBluewings.push_back(bw);
                }
            }
            else if (currentSection == "[SKYARMORS]") {
                if (line.rfind("count=", 0) == 0) continue;
                int x = 0, y = 0, speed = 0, dir = 0;
                if (ss >> x >> y >> speed >> dir) {
                    CSKYARMOR* sa = new CSKYARMOR(x, y, speed, dir);
                    sa->setTexture(mCskyarmorTexture);
                    tempSkyarmors.push_back(sa);
                }
            }
            else if (currentSection == "[GLEAMEYES]") {
                if (line.rfind("count=", 0) == 0) continue;
                int x = 0, y = 0, speed = 0, dir = 0;
                if (ss >> x >> y >> speed >> dir) {
                    CGLEAMEYES* ge = new CGLEAMEYES(x, y, speed, dir);
                    ge->setTextures(mCGleameyesTexture1, mCGleameyesTexture2);
                    tempGleameyes.push_back(ge);
                }
            }
            else if (currentSection == "[CHEATHCLIFFS]") {
                if (line.rfind("count=", 0) == 0) continue;
                int x = 0, y = 0, speed = 0, dir = 0;
                if (ss >> x >> y >> speed >> dir) {
                    CHEATHCLIFF* hc = new CHEATHCLIFF(x, y, speed, dir);
                    hc->setTextures(mCheathcliffTexture1, mCheathcliffTexture2);
                    tempCheathcliffs.push_back(hc);
                }
            }
            else if (currentSection == "[CILLFANGS]") {
                if (line.rfind("count=", 0) == 0) continue;
                int x = 0, y = 0, speed = 0, dir = 0;
                if (ss >> x >> y >> speed >> dir) {
                    CILLFANG* cf = new CILLFANG(x, y, speed, dir);
                    cf->setTextures(mCillfangTexture1, mCillfangTexture2);
                    tempCillfangs.push_back(cf);
                }
            }
            else if (currentSection == "[ICEDRAGONS]") {
                if (line.rfind("count=", 0) == 0) continue;
                int x = 0, y = 0, speed = 0, dir = 0;
                if (ss >> x >> y >> speed >> dir) {
                    CICEDRAGON* id = new CICEDRAGON(x, y, speed, dir);
                    id->setTextures(mCicedragonTexture1, mCicedragonTexture2);
                    tempCicedragons.push_back(id);
                }
            }
        }
        inFile.close();

        // Swap snapshot into actual game state under thread lock safely
        {
            std::lock_guard<std::mutex> lock(mGameMutex);

            clearObstacles();
            mTrafficLights.clear();
            mLanes.clear();

            mIsInfinityMode = parsedIsInfinityMode;
            mStage = parsedStage;
            mScore = parsedScore;
            mMaxReachedY = parsedMaxReachedY;
            mCameraY = parsedCameraY;
            mInfiniteLevel = parsedInfiniteLevel;
            mLanePatternIndex = parsedLanePatternIndex;

            mTrafficLights = std::move(tempTrafficLights);
            mLanes = std::move(tempLanes);
            mBluewings = std::move(tempBluewings);
            mSkyarmors = std::move(tempSkyarmors);
            mGleameyes = std::move(tempGleameyes);
            mCheathcliffs = std::move(tempCheathcliffs);
            mCillfangs = std::move(tempCillfangs);
            mCicedragons = std::move(tempCicedragons);

            mSelectedCharOption = parsedCharType;
            mPlayer.setCharacter(parsedCharType == 0 ? CPEOPLE::CharacterType::KIRITO : CPEOPLE::CharacterType::ASUNA);
            mPlayer.setPosition(parsedPlayerX, parsedPlayerY);
            mPlayer.setDead(parsedPlayerState == 0);

            mState = GameState::PLAYING;
        }
        return true;
    } catch (...) {
        // Clean up temporary dynamically allocated obstacle pointers if parsing failed
        clearObstacleList(tempBluewings);
        clearObstacleList(tempSkyarmors);
        clearObstacleList(tempGleameyes);
        clearObstacleList(tempCheathcliffs);
        clearObstacleList(tempCillfangs);
        clearObstacleList(tempCicedragons);
        return false;
    }
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
            tl.draw(mRenderer, mFont, mCameraY, mTrafficLightRedTexture, mTrafficLightGreenTexture);
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

        mFont.drawText(mRenderer, "PRESS ESC TO RETURN TO MENU", 776, 26, 2, shadow);
        mFont.drawText(mRenderer, "PRESS ESC TO RETURN TO MENU", 774, 24, 2, hudColor);
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
        tl.draw(mRenderer, mFont, 0.0f, mTrafficLightRedTexture, mTrafficLightGreenTexture);
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
    std::string hudStageText = "STAGE: " + std::to_string(mStage) + " / " + std::to_string(MAX_EASY_STAGE);
    
    SDL_Color shadow = {0, 0, 0, 180};
    mFont.drawText(mRenderer, hudStageText, 22, 26, 2, shadow);
    mFont.drawText(mRenderer, hudStageText, 20, 24, 2, cyanGlow);
    
    mFont.drawText(mRenderer, "PRESS ESC TO RETURN TO MENU", 776, 26, 2, shadow);
    mFont.drawText(mRenderer, "PRESS ESC TO RETURN TO MENU", 774, 24, 2, hudColor);
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
        resetEasyMode();
    }
}

int CGAME::randomRange(int minValue, int maxValue) const {
    if (maxValue <= minValue) return minValue;
    return minValue + (rand() % (maxValue - minValue + 1));
}

void CGAME::resetEasyMode() {
    mIsInfinityMode = false;
    mScore = 0;
    mInfiniteLevel = 1;
    mCameraY = 0.0f;
    mLanes.clear();
    mFlashTimer = 0.0f;
    mPendingStageAdvance.store(false);

    mPlayer.resetPosition();
    setupEasyModeObstacles();
}

void CGAME::setupEasyModeObstacles() {
    clearObstacles();
    mTrafficLights.clear();

    int count = mStage + 1;  // Stage 1→2, Stage 2→3, ..., Stage 5→6

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

    spawnLane(LaneType::VEHICLE, 120, count, -1);
    spawnLane(LaneType::MONSTER, 200, count, 1);
    spawnLane(LaneType::MONSTER, 360, count, 1);
    spawnLane(LaneType::VEHICLE, 440, count, -1);
    spawnLane(LaneType::MONSTER, 520, count, 1);
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
    if (screenY < 360.0f) {
        mCameraY = (float)mPlayer.getY() - 360.0f;
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

    pruneObstacleList(mGleameyes, mCameraY, -400.0f, 880.0f);
    pruneObstacleList(mCheathcliffs, mCameraY, -400.0f, 880.0f);
    pruneObstacleList(mCillfangs, mCameraY, -400.0f, 880.0f);
    pruneObstacleList(mCicedragons, mCameraY, -400.0f, 880.0f);
    pruneObstacleList(mBluewings, mCameraY, -400.0f, 880.0f);
    pruneObstacleList(mSkyarmors, mCameraY, -400.0f, 880.0f);

    if (hitPlayerAgainstList(mPlayer, mGleameyes) ||
        hitPlayerAgainstList(mPlayer, mCheathcliffs) ||
        hitPlayerAgainstList(mPlayer, mCillfangs) ||
        hitPlayerAgainstList(mPlayer, mCicedragons) ||
        hitPlayerAgainstList(mPlayer, mBluewings) ||
        hitPlayerAgainstList(mPlayer, mSkyarmors)) {
        mPlayer.setDead(true);
        mState = GameState::GAMEOVER;
        mFlashTimer = 0.5f;
        playSFX(mSfxHit);
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
    if (mSidewalkTopTexture) { SDL_DestroyTexture(mSidewalkTopTexture); mSidewalkTopTexture = nullptr; }
    if (mSidewalkBottomTexture) { SDL_DestroyTexture(mSidewalkBottomTexture); mSidewalkBottomTexture = nullptr; }
    if (mLaneRestTexture) { SDL_DestroyTexture(mLaneRestTexture); mLaneRestTexture = nullptr; }
    if (mLaneForestTexture) { SDL_DestroyTexture(mLaneForestTexture); mLaneForestTexture = nullptr; }
    if (mLaneRoadTexture) { SDL_DestroyTexture(mLaneRoadTexture); mLaneRoadTexture = nullptr; }
    if (mCbluewingTexture) { SDL_DestroyTexture(mCbluewingTexture); mCbluewingTexture = nullptr; }
    if (mCskyarmorTexture) { SDL_DestroyTexture(mCskyarmorTexture); mCskyarmorTexture = nullptr; }
    
    if (mTrafficLightRedTexture) { SDL_DestroyTexture(mTrafficLightRedTexture); mTrafficLightRedTexture = nullptr; }
    if (mTrafficLightGreenTexture) { SDL_DestroyTexture(mTrafficLightGreenTexture); mTrafficLightGreenTexture = nullptr; }
    
    if (mSwordTexture) {
        SDL_DestroyTexture(mSwordTexture);
        mSwordTexture = nullptr;
    }
    
    mPlayer.freeTextures();
    mFont.free();

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
