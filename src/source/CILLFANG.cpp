#include "CILLFANG.h"
#include <cmath>

CILLFANG::CILLFANG(int x, int y, int speed, int direction)
    : CANIMAL(x, y, speed, direction), mStridePhase(0) {
    mWidth = 110;
    mHeight = 80;
}

CILLFANG::~CILLFANG() {}

void CILLFANG::Move(int limitX1, int limitX2) {
    mX += mDirection * mSpeed;
    mStridePhase = (mStridePhase + 1) % 4;

    if (mDirection == 1 && mX > limitX2) {
        mX = limitX1 - mWidth;
    }
    else if (mDirection == -1 && mX < limitX1 - mWidth) {
        mX = limitX2;
    }
}

void CILLFANG::draw(SDL_Renderer* renderer, CFont& font, float cameraY) {
    float baseY = (float)mY - cameraY;
    if (mTexture1) {
        int frameIndex = (std::abs(mX) / 120) % 2;
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
    SDL_SetRenderDrawColor(renderer, 46, 117, 89, 255);
    SDL_FRect body = { (float)mX + 16, baseY + 12, 32.0f, 24.0f };
    SDL_RenderFillRect(renderer, &body);

    SDL_FRect head;
    if (mDirection == 1) {
        head = { (float)mX + 32, baseY + 2, 24.0f, 16.0f };
    } else {
        head = { (float)mX + 8, baseY + 2, 24.0f, 16.0f };
    }
    SDL_RenderFillRect(renderer, &head);

    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_FRect eye;
    if (mDirection == 1) {
        eye = { (float)mX + 46, baseY + 6, 4.0f, 4.0f };
    } else {
        eye = { (float)mX + 14, baseY + 6, 4.0f, 4.0f };
    }
    SDL_RenderFillRect(renderer, &eye);

    SDL_SetRenderDrawColor(renderer, 46, 117, 89, 255);
    SDL_FRect tail;
    if (mDirection == 1) {
        tail = { (float)mX, baseY + 20, 16.0f, 12.0f };
    } else {
        tail = { (float)mX + 48, baseY + 20, 16.0f, 12.0f };
    }
    SDL_RenderFillRect(renderer, &tail);

    Uint64 ticks = SDL_GetTicks();
    bool legState = (ticks / 450) % 2 == 0;

    SDL_SetRenderDrawColor(renderer, 24, 78, 55, 255);
    SDL_FRect leg1, leg2;
    if (legState) {
        leg1 = { (float)mX + 20, baseY + 36, 6.0f, 8.0f };
        leg2 = { (float)mX + 38, baseY + 36, 6.0f, 12.0f };
    } else {
        leg1 = { (float)mX + 20, baseY + 36, 6.0f, 12.0f };
        leg2 = { (float)mX + 38, baseY + 36, 6.0f, 8.0f };
    }
    SDL_RenderFillRect(renderer, &leg1);
    SDL_RenderFillRect(renderer, &leg2);

    SDL_Color textCol = {255, 255, 255, 255};
    font.drawText(renderer, "FANG", mX + (mDirection == 1 ? 18 : 22), (int)(baseY + 18), 1, textCol);
}
