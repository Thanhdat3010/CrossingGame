#ifndef CGAME_H
#define CGAME_H

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include "CFont.h"
#include "CPEOPLE.h"
#include "CVEHICLE.h"
#include "CBLUEWING.h"
#include "CSKYARMOR.h"
#include "CANIMAL.h"
#include "CILLFANG.h"
#include "CICEDRAGON.h"
#include "CHEATHCLIFF.h"
#include "CGLEAMEYES.h"
#include "CTRAFFICLIGHT.h"

enum class GameState {
    MENU,
    CHAR_SELECT,
    STAGE_SELECT,
    SETTINGS,
    PLAYING,
    PAUSED,
    SAVE_DIALOG,
    LOAD_DIALOG,
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

    CFont mFont;
    SDL_Texture* mSwordTexture;

    SDL_Texture* mCGleameyesTexture1;
    SDL_Texture* mCGleameyesTexture2;
    SDL_Texture* mCheathcliffTexture1;
    SDL_Texture* mCheathcliffTexture2;
    SDL_Texture* mCillfangTexture1;
    SDL_Texture* mCillfangTexture2;
    SDL_Texture* mCicedragonTexture1;
    SDL_Texture* mCicedragonTexture2;
    SDL_Texture* mBgMenuTexture;
    SDL_Texture* mBgPlayingTexture;
    SDL_Texture* mSidewalkTopTexture;
    SDL_Texture* mSidewalkBottomTexture;
    SDL_Texture* mLaneRestTexture;
    SDL_Texture* mLaneForestTexture;
    SDL_Texture* mLaneRoadTexture;
    SDL_Texture* mCbluewingTexture;
    SDL_Texture* mCskyarmorTexture;

    CPEOPLE mPlayer;

    std::vector<CGLEAMEYES*> mGleameyes;
    std::vector<CHEATHCLIFF*> mCheathcliffs;
    std::vector<CILLFANG*> mCillfangs;
    std::vector<CICEDRAGON*> mCicedragons;
    std::vector<CBLUEWING*> mBluewings;
    std::vector<CSKYARMOR*> mSkyarmors;
    std::vector<CTRAFFICLIGHT> mTrafficLights;

    int mStage;
    bool mIsInfinityMode;

    float mCameraY;
    int mLaneHeight;
    int mInfiniteLevel;
    int mLanePatternIndex;
    std::vector<Lane> mLanes;

    int mSelectedMenuOption;
    int mSelectedCharOption;
    int mSelectedStageOption;
    int mSelectedSettingsOption;
    int mSelectedPauseOption;
    int mSelectedSaveIndex;
    int mSelectedLoadIndex;

    GameState mSettingsPreviousState;
    GameState mLoadPreviousState;

    std::vector<std::string> mSaveFilesList;
    std::string mInputSaveName;
    bool mIsTypingNewSaveName;
    std::string mPendingDeleteFileName;
    GameState mDeleteReturnState;

    int mScore;
    int mMaxReachedY;

    bool mShowMenuWarning;
    float mWarningTimer;
    float mMenuAnimTimer;

    MIX_Mixer* mMixer;
    MIX_Track* mBgmTrack;
    MIX_Audio* mBgmMenu;
    MIX_Audio* mSfxHit;
    MIX_Audio* mSfxJump;
    bool mAudioMuted;
    bool mSfxMuted;

    float mFlashTimer;

    std::thread mPhysicsThread;
    mutable std::mutex mGameMutex;
    std::atomic<bool> mIsThreadRunning;
    void physicsWorkerFunc();

    void handleInput();
    void update(float deltaTime);
    void render();

    void renderMenuBackground();
    void renderMenu();
    void renderCharSelect();
    void renderStageSelect();
    void renderSettings();
    void renderPauseMenu();
    void renderSaveDialog();
    void renderLoadDialog();
    void renderDeleteConfirmDialog();
    void renderPlaying();
    void toggleMusic();
    void toggleSfx();

    bool saveGame(const std::string& filename);
    bool loadGame(const std::string& filename);
    std::vector<std::string> scanSaveFiles();

    void clearObstacles();

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

    bool init(const char* title, int width, int height);
    void run();

    void startGame();
    void resetGame();
    void exitGame();
    void pauseGame();
    void resumeGame();

    SDL_Renderer* getRenderer() const { return mRenderer; }
    GameState getState() const { return mState; }
};

#endif // CGAME_H
