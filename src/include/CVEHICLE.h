#ifndef CVEHICLE_H
#define CVEHICLE_H

#include <SDL3/SDL.h>
#include "CFont.h"

class CVEHICLE {
protected:
    int mX;
    int mY;
    int mWidth;
    int mHeight;
    int mSpeed;
    int mDirection;
    SDL_Texture* mTexture;

public:
    CVEHICLE(int x, int y, int speed, int direction);
    virtual ~CVEHICLE() = default;

    virtual void Move(int limitX1, int limitX2) = 0;
    virtual void draw(SDL_Renderer* renderer, CFont& font, float cameraY) = 0;

    int getX() const { return mX; }
    int getY() const { return mY; }
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }
    SDL_Texture* getTexture() const { return mTexture; }

    SDL_FRect getHitbox() const {
        const float padX = mWidth * 0.15f;
        const float padY = mHeight * 0.15f;
        return { (float)mX + padX, (float)mY + padY,
                 (float)mWidth - 2 * padX, (float)mHeight - 2 * padY };
    }

    void setX(int x) { mX = x; }
    void setY(int y) { mY = y; }
    void setSpeed(int speed) { mSpeed = speed; }
    void setTexture(SDL_Texture* tex) {
        mTexture = tex;
        if (mTexture) {
            float texW = 0, texH = 0;
            SDL_GetTextureSize(mTexture, &texW, &texH);
            if (texW > 0 && texH > 0) {
                mWidth = (int)((float)mHeight * (texW / texH));
            }
        }
    }
};

#endif // CVEHICLE_H
