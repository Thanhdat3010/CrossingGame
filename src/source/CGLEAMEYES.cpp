#include "CGLEAMEYES.h"
#include <cmath>

CGLEAMEYES::CGLEAMEYES(int x, int y, int speed, int direction)
    : CANIMAL(x, y, speed, direction), mArmorGlow(0) {
    mWidth = 140;
    mHeight = 80;
}

CGLEAMEYES::~CGLEAMEYES() {}

void CGLEAMEYES::Move(int limitX1, int limitX2) {
    mX += mDirection * mSpeed;
    mArmorGlow = (mArmorGlow + 1) % 8;

    if (mDirection == 1 && mX > limitX2) {
        mX = limitX1 - mWidth;
    }
    else if (mDirection == -1 && mX < limitX1 - mWidth) {
        mX = limitX2;
    }
}

void CGLEAMEYES::draw(SDL_Renderer* renderer, CFont& font, float cameraY) {
    float baseY = (float)mY - cameraY;
    if (mTexture1) {
        int frameIndex = (std::abs(mX) / 90) % 2;
        SDL_Texture* activeTex = (frameIndex == 0) ? mTexture1 : mTexture2;
        if (!activeTex) activeTex = mTexture1;

        float drawW = (float)mWidth * 1.5f;
        float drawH = (float)mHeight * 1.5f;
        float drawX = (float)mX - (drawW - (float)mWidth) / 2.0f;
        float drawY = baseY - (drawH - (float)mHeight) / 2.0f;

        SDL_FRect dstRect = { drawX, drawY, drawW, drawH };
        SDL_FlipMode flip = (mDirection == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderTextureRotated(renderer, activeTex, NULL, &dstRect, 0.0f, NULL, flip);
        return;
    }

    SDL_SetRenderDrawColor(renderer, 43, 45, 66, 255);
    SDL_FRect cargoRect;
    SDL_SetRenderDrawColor(renderer, 230, 57, 70, 255);
    SDL_FRect cabinRect;
    SDL_SetRenderDrawColor(renderer, 241, 250, 238, 255);
    SDL_FRect windowRect;

    if (mDirection == 1) {
        cargoRect = { (float)mX, baseY + 4, 60.0f, 32.0f };
        cabinRect = { (float)mX + 60, baseY + 12, 36.0f, 24.0f };
        windowRect = { (float)mX + 75, baseY + 16, 12.0f, 10.0f };
    } else {
        cabinRect = { (float)mX, baseY + 12, 36.0f, 24.0f };
        cargoRect = { (float)mX + 36, baseY + 4, 60.0f, 32.0f };
        windowRect = { (float)mX + 9, baseY + 16, 12.0f, 10.0f };
    }

    SDL_RenderFillRect(renderer, &cargoRect);
    SDL_RenderFillRect(renderer, &cabinRect);
    SDL_RenderFillRect(renderer, &windowRect);

    SDL_SetRenderDrawColor(renderer, 29, 53, 87, 255);
    SDL_FRect wheel1, wheel2, wheel3;
    if (mDirection == 1) {
        wheel1 = { (float)mX + 12, baseY + 34, 14.0f, 10.0f };
        wheel2 = { (float)mX + 38, baseY + 34, 14.0f, 10.0f };
        wheel3 = { (float)mX + 72, baseY + 34, 14.0f, 10.0f };
    } else {
        wheel1 = { (float)mX + 10, baseY + 34, 14.0f, 10.0f };
        wheel2 = { (float)mX + 44, baseY + 34, 14.0f, 10.0f };
        wheel3 = { (float)mX + 70, baseY + 34, 14.0f, 10.0f };
    }
    SDL_RenderFillRect(renderer, &wheel1);
    SDL_RenderFillRect(renderer, &wheel2);
    SDL_RenderFillRect(renderer, &wheel3);

    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 180);
    SDL_FRect glow = { (float)mX + (mDirection == 1 ? 108.0f : -4.0f), baseY + 14, 8.0f, 8.0f };
    SDL_RenderFillRect(renderer, &glow);

    SDL_Color textColor = {255, 255, 255, 255};
    font.drawText(renderer, "EYES", mX + (mDirection == 1 ? 12 : 48), (int)(baseY + 14), 1, textColor);
}
