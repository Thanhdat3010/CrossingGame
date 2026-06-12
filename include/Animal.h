#ifndef ANIMAL_H
#define ANIMAL_H

#include <SDL3/SDL.h>
#include "Font.h"

class CANIMAL {
protected:
    int mX;
    int mY;
    int mWidth;
    int mHeight;
    int mSpeed;
    int mDirection;
    SDL_Texture* mTexture1;
    SDL_Texture* mTexture2;

public:
    CANIMAL(int x, int y, int speed, int direction);
    virtual ~CANIMAL();

    virtual void Move(int limitX1, int limitX2) = 0;
    virtual void draw(SDL_Renderer* renderer, CFont& font, float cameraY) = 0;

    int getX() const { return mX; }
    int getY() const { return mY; }
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }
    SDL_Texture* getTexture() const { return mTexture1; }

    SDL_FRect getHitbox() const {
        const float PAD_X = mWidth * 0.15f;
        const float PAD_Y = mHeight * 0.15f;
        return { (float)mX + PAD_X, (float)mY + PAD_Y,
                 (float)mWidth - 2 * PAD_X, (float)mHeight - 2 * PAD_Y };
    }

    void setX(int x) { mX = x; }
    void setY(int y) { mY = y; }
    void setSpeed(int speed) { mSpeed = speed; }
    void setTextures(SDL_Texture* tex1, SDL_Texture* tex2) {
        mTexture1 = tex1;
        mTexture2 = tex2;
        if (mTexture1) {
            float texW = 0, texH = 0;
            SDL_GetTextureSize(mTexture1, &texW, &texH);
            if (texW > 0 && texH > 0) {
                mWidth = (int)((float)mHeight * (texW / texH));
            }
        }
    }
};

class CILLFANG : public CANIMAL {
private:
    int mStridePhase;

public:
    CILLFANG(int x, int y, int speed, int direction);
    virtual ~CILLFANG();

    void Move(int limitX1, int limitX2) override;
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY) override;
};

class CICEDRAGON : public CANIMAL {
private:
    float mWingPulse;

public:
    CICEDRAGON(int x, int y, int speed, int direction);
    virtual ~CICEDRAGON();

    void Move(int limitX1, int limitX2) override;
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY) override;
};

class CHEATHCLIFF : public CANIMAL {
private:
    int mDashPhase;

public:
    CHEATHCLIFF(int x, int y, int speed, int direction);
    virtual ~CHEATHCLIFF();

    void Move(int limitX1, int limitX2) override;
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY) override;
};

class CGLEAMEYES : public CANIMAL {
private:
    int mArmorGlow;

public:
    CGLEAMEYES(int x, int y, int speed, int direction);
    virtual ~CGLEAMEYES();

    void Move(int limitX1, int limitX2) override;
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY) override;
};

#endif // ANIMAL_H
