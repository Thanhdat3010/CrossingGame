#include "CHEATHCLIFF.h"
#include <cmath>

CHEATHCLIFF::CHEATHCLIFF(int x, int y, int speed, int direction)
    : CANIMAL(x, y, speed, direction), mDashPhase(0) {
    mWidth = 120;
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

        float drawW = (float)mWidth * 1.4f;
        float drawH = (float)mHeight * 1.4f;
        float drawX = (float)mX - (drawW - (float)mWidth) / 2.0f;
        float drawY = baseY - (drawH - (float)mHeight) / 2.0f;

        SDL_FRect dstRect = { drawX, drawY, drawW, drawH };
        SDL_FlipMode flip = (mDirection == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderTextureRotated(renderer, activeTex, NULL, &dstRect, 0.0f, NULL, flip);
        return;
    }

    SDL_SetRenderDrawColor(renderer, 247, 127, 0, 255);
    SDL_FRect bodyRect = { (float)mX, baseY + 12, 72.0f, 16.0f };
    SDL_RenderFillRect(renderer, &bodyRect);

    SDL_SetRenderDrawColor(renderer, 252, 191, 73, 255);
    SDL_FRect roofRect;
    if (mDirection == 1) {
        roofRect = { (float)mX + 16, baseY + 2, 36.0f, 10.0f };
    } else {
        roofRect = { (float)mX + 20, baseY + 2, 36.0f, 10.0f };
    }
    SDL_RenderFillRect(renderer, &roofRect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_FRect windowRect;
    if (mDirection == 1) {
        windowRect = { (float)mX + 32, baseY + 4, 16.0f, 6.0f };
    } else {
        windowRect = { (float)mX + 24, baseY + 4, 16.0f, 6.0f };
    }
    SDL_RenderFillRect(renderer, &windowRect);

    SDL_SetRenderDrawColor(renderer, 0, 48, 73, 255);
    SDL_FRect w1 = { (float)mX + 10, baseY + 24, 12.0f, 8.0f };
    SDL_FRect w2 = { (float)mX + 50, baseY + 24, 12.0f, 8.0f };
    SDL_RenderFillRect(renderer, &w1);
    SDL_RenderFillRect(renderer, &w2);

    SDL_SetRenderDrawColor(renderer, 255, 235, 41, 200);
    SDL_FRect headlight;
    if (mDirection == 1) {
        headlight = { (float)mX + 70, baseY + 16, 4.0f, 6.0f };
    } else {
        headlight = { (float)mX - 2, baseY + 16, 4.0f, 6.0f };
    }
    SDL_RenderFillRect(renderer, &headlight);

    SDL_Color textCol = {0, 0, 0, 255};
    font.drawText(renderer, "CLIFF", mX + 26, (int)(baseY + 16), 1, textCol);
}
