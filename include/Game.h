#ifndef GAME_H
#define GAME_H

#include <SDL3/SDL.h>
#include "Font.h"
#include "People.h"
#include "Vehicle.h"
#include "Animal.h"
#include <vector>

enum class GameState {
    MENU,
    CHAR_SELECT,
    STAGE_SELECT,
    PLAYING,
    PAUSED,
    GAMEOVER
};

enum class LaneType {
    VEHICLE,
    MONSTER,
    REST
};

struct Lane {
    LaneType type;
    int worldY;
};

class CGAME {
private:
    SDL_Window* mWindow;
    SDL_Renderer* mRenderer;
    bool mIsRunning;
    GameState mState;

    // Thành phần vẽ chữ pixel (Bitmap Font)
    CFont mFont;

    // Texture của ảnh hai thanh kiếm
    SDL_Texture* mSwordTexture;

    // Các texture bản đồ & quái vật (SAO theme PNGs)
    SDL_Texture* mCGleameyesTexture1;
    SDL_Texture* mCGleameyesTexture2;
    SDL_Texture* mCheathcliffTexture1;
    SDL_Texture* mCheathcliffTexture2;
    SDL_Texture* mCillfangTexture1;
    SDL_Texture* mCillfangTexture2;
    SDL_Texture* mCicedragonTexture1;
    SDL_Texture* mCicedragonTexture2;
    SDL_Texture* mBgPlayingTexture;
    SDL_Texture* mSidewalkTopTexture;
    SDL_Texture* mSidewalkBottomTexture;
    SDL_Texture* mLaneRestTexture;
    SDL_Texture* mLaneForestTexture;
    SDL_Texture* mLaneRoadTexture;


    // Người chơi
    CPEOPLE mPlayer;

    // Danh sách các chướng ngại vật (xe/thú) trên các làn đường
    std::vector<CGLEAMEYES*> mGleameyes;
    std::vector<CHEATHCLIFF*> mCheathcliffs;
    std::vector<CILLFANG*> mCillfangs;
    std::vector<CICEDRAGON*> mCicedragons;

    // Cấp độ màn chơi hiện tại
    int mStage;
    bool mIsInfinityMode; // Cờ hiệu chế độ Vô Tận (Infinity Mode)

    // Camera cho chế độ Infinite
    float mCameraY;
    int mLaneHeight;
    int mInfiniteLevel;
    int mLanePatternIndex;
    std::vector<Lane> mLanes;

    // Trạng thái quản lý Menu
    int mSelectedMenuOption; // 0: New Game, 1: Load Game, 2: Settings
    int mSelectedCharOption; // 0: Kirito, 1: Asuna
    int mSelectedStageOption;// 0: Stage 1, 1: Stage 2, 2: Stage 3, 3: Infinity Mode

    bool mShowMenuWarning;   // Hiển thị thông báo khi chọn Load/Settings
    float mWarningTimer;     // Đếm ngược thời gian tắt cảnh báo
    float mMenuAnimTimer;    // Bộ đếm thời gian animation cho menu

    // Các hàm phụ trợ cho Game Loop
    void handleInput();
    void update(float deltaTime);
    void render();

    // Hàm vẽ riêng cho từng trạng thái
    void renderMenu();
    void renderCharSelect();
    void renderStageSelect();
    void renderPlaying();

    // Hàm dọn dẹp các chướng ngại vật
    void clearObstacles();

    // Tutorial / Infinite helpers
    void resetTutorial();
    void resetInfinite();
    void initInfiniteLanes();
    void addLaneAbove();
    void pruneLanes();
    void spawnObstaclesForLane(const Lane& lane);
    void updateInfinite(float deltaTime);
    int randomRange(int minValue, int maxValue) const;

public:
    CGAME();
    ~CGAME();

    // Khởi tạo cửa sổ và bộ vẽ
    bool init(const char* title, int width, int height);

    // Chạy vòng lặp game chính
    void run();

    // Các phương thức điều khiển trạng thái (yêu cầu từ đề bài)
    void startGame();      // Bắt đầu màn chơi mới
    void resetGame();      // Thiết lập lại game như ban đầu
    void exitGame();       // Thoát game và giải phóng tài nguyên
    void pauseGame();      // Tạm dừng
    void resumeGame();     // Tiếp tục chơi

    // Getters
    SDL_Renderer* getRenderer() const { return mRenderer; }
    GameState getState() const { return mState; }
};

#endif // GAME_H
