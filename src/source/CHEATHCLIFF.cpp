#include "CHEATHCLIFF.h"
#include <cmath>

CHEATHCLIFF::CHEATHCLIFF(int x, int y, int speed, int direction)
    : CANIMAL(x, y, speed, direction), mDashPhase(0) {
    mWidth = 105;
    mHeight = 80;
}

CHEATHCLIFF::~CHEATHCLIFF() {}

void CHEATHCLIFF::Move(int limitX1, int limitX2) {
    mX += mDirection * mSpeed;
    mDashPhase = (mDashPhase + 1) % 6;

    if (mDirection == 1 && mX > limitX2) {
        mX = limitX1 - mWidth;
    }
    else if (mDirection == -1 && mX < limitX1 - mWidth) {
        mX = limitX2;
    }
}

void CHEATHCLIFF::draw(SDL_Renderer* renderer, CFont& font, float cameraY) {
    float baseY = (float)mY - cameraY;
    if (mTexture1) {
        int frameIndex = (std::abs(mX) / 100) % 2;
        SDL_Texture* activeTex = (frameIndex == 0) ? mTexture1 : mTexture2;
        if (!activeTex) activeTex = mTexture1;

        float drawW = (float)mWidth * 1.3f;
        float drawH = (float)mHeight * 1.3f;
        float drawX = (float)mX - (drawW - (float)mWidth) / 2.0f;
        float drawY = baseY - (drawH - (float)mHeight) / 2.0f;

        SDL_FRect dstRect = { drawX, drawY, drawW, drawH };
        SDL_FlipMode flip = (mDirection == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        
        SDL_RenderTextureRotated(renderer, activeTex, NULL, &dstRect, 0.0f, NULL, flip);
        return;
    }
    SDL_SetRenderDrawColor(renderer, 230, 230, 240, 255);
    SDL_FRect body = { (float)mX + 16, baseY + 8, 48.0f, 28.0f };
    SDL_RenderFillRect(renderer, &body);

    SDL_SetRenderDrawColor(renderer, 180, 20, 20, 255);
    SDL_FRect shield;
    if (mDirection == 1) {
        shield = { (float)mX + 56, baseY + 6, 12.0f, 32.0f };
    } else {
        shield = { (float)mX + 12, baseY + 6, 12.0f, 32.0f };
    }
    SDL_RenderFillRect(renderer, &shield);

    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_FRect headlight;
    if (mDirection == 1) {
        headlight = { (float)mX + 62, baseY + 12, 6.0f, 6.0f };
    } else {
        headlight = { (float)mX + 12, baseY + 12, 6.0f, 6.0f };
    }
    SDL_RenderFillRect(renderer, &headlight);

    SDL_Color textCol = {255, 255, 255, 255};
    font.drawText(renderer, "BOSS", mX + 22, (int)(baseY + 18), 1, textCol);
}

void CHEATHCLIFF::Tell(MIX_Mixer* mixer, MIX_Audio* sound) {
    CANIMAL::Tell(mixer, sound);
}
