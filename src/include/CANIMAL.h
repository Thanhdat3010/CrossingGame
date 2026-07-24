#ifndef CANIMAL_H
#define CANIMAL_H

#include <SDL3/SDL.h>
#include "CFont.h"

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
    virtual void Tell(struct MIX_Mixer* mixer = nullptr, struct MIX_Audio* sound = nullptr);

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

#endif // CANIMAL_H
