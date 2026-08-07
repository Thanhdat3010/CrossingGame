#ifndef CTRAFFICLIGHT_H
#define CTRAFFICLIGHT_H

#include <SDL3/SDL.h>
#include "CFont.h"

class CTRAFFICLIGHT {
private:
    int mLaneY;             // Vị trí Y của làn xe
    bool mIsRed;            // true: Đèn Đỏ (xe dừng), false: Đèn Xanh (xe chạy)
    float mTimer;           // Bộ đếm thời gian hiện tại (giây)
    float mRedDuration;     // Thời gian giữ đèn đỏ (mặc định 3.0s)
    float mGreenDuration;   // Thời gian giữ đèn xanh (mặc định 5.0s)

public:
    CTRAFFICLIGHT(int laneY = 0, float redDur = 3.0f, float greenDur = 5.0f);
    ~CTRAFFICLIGHT() = default;

    void initTextures(SDL_Renderer* renderer);
    void update(float deltaTime);
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY, SDL_Texture* redTex = nullptr, SDL_Texture* greenTex = nullptr);

    bool isRed() const { return mIsRed; }
    int getLaneY() const { return mLaneY; }
    float getTimer() const { return mTimer; }
    float getRedDuration() const { return mRedDuration; }
    float getGreenDuration() const { return mGreenDuration; }

    void setState(bool isRed, float timer) {
        mIsRed = isRed;
        mTimer = timer;
    }
};

#endif // CTRAFFICLIGHT_H
