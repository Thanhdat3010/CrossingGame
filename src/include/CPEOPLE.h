#ifndef CPEOPLE_H
#define CPEOPLE_H

#include <SDL3/SDL.h>
#include "CFont.h"

class CVEHICLE;
class CANIMAL;

class CPEOPLE {
public:
    enum class CharacterType { KIRITO, ASUNA };

private:
    int mX, mY;
    int mWidth;
    int mHeight;
    bool mState;
    int mSpeed;

    float mDrawX, mDrawY;
    float mAnimTimer;

    CharacterType mCharType;
    SDL_Texture* mKiritoTexture;
    SDL_Texture* mAsunaTexture;

public:
    CPEOPLE();
    ~CPEOPLE();

    void update(float deltaTime);
    void resetPosition();
    void setPosition(int x, int y);
    void setCharacter(CharacterType type);
    void loadTextures(SDL_Renderer* renderer);
    void freeTextures();

    bool isImpact(CVEHICLE* v);
    bool isImpact(CANIMAL* a);

    void Up(int limitY);
    void Down(int limitY);
    void Left(int limitX);
    void Right(int limitX);

    void draw(SDL_Renderer* renderer, CFont& font, float cameraY);

    bool isDead() const { return !mState; }
    bool isFinish() const;
    void setDead(bool dead) { mState = !dead; }

    int getX() const { return mX; }
    int getY() const { return mY; }
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }

    SDL_FRect getHitbox() const {
        const float PAD_X = mWidth  * 0.20f;
        const float PAD_Y = mHeight * 0.20f;
        return { (float)mX + PAD_X, (float)mY + PAD_Y,
                 (float)mWidth - 2 * PAD_X, (float)mHeight - 2 * PAD_Y };
    }

    SDL_Texture* getKiritoTexture() const { return mKiritoTexture; }
    SDL_Texture* getAsunaTexture() const { return mAsunaTexture; }
};

#endif // CPEOPLE_H
