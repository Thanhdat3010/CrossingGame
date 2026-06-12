#include "Vehicle.h"
#include <cmath>

CVEHICLE::CVEHICLE(int x, int y, int speed, int direction)
    : mX(x), mY(y), mWidth(48), mHeight(48), mSpeed(speed), mDirection(direction), mTexture(nullptr) {
}

// ====================================================================
// CBLUEWING
// ====================================================================
CBLUEWING::CBLUEWING(int x, int y, int speed, int direction)
    : CVEHICLE(x, y, speed, direction) {
    mWidth = 100;
    mHeight = 80;
}

void CBLUEWING::Move(int limitX1, int limitX2) {
    mX += mDirection * mSpeed;
    if (mDirection == 1 && mX > limitX2) {
        mX = limitX1 - mWidth;
    } else if (mDirection == -1 && mX < limitX1 - mWidth) {
        mX = limitX2;
    }
}

void CBLUEWING::draw(SDL_Renderer* renderer, CFont& font, float cameraY) {
    float baseY = (float)mY - cameraY;
    if (mTexture) {
        float drawW = (float)mWidth * 1.3f;
        float drawH = (float)mHeight * 1.3f;
        float drawX = (float)mX - (drawW - (float)mWidth) / 2.0f;
        float drawY = baseY - (drawH - (float)mHeight) / 2.0f;

        SDL_FRect dstRect = { drawX, drawY, drawW, drawH };
        SDL_FlipMode flip = (mDirection == 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderTextureRotated(renderer, mTexture, NULL, &dstRect, 0.0f, NULL, flip);
        return;
    }
    // Fallback: Blue Cyan box for CBLUEWING
    SDL_SetRenderDrawColor(renderer, 0, 150, 255, 255);
    SDL_FRect body = { (float)mX, baseY + 10, (float)mWidth, (float)mHeight - 20 };
    SDL_RenderFillRect(renderer, &body);

    SDL_Color textCol = { 255, 255, 255, 255 };
    font.drawText(renderer, "WING", mX + 15, (int)(baseY + 28), 1, textCol);
}

// ====================================================================
// CSKYARMOR
// ====================================================================
CSKYARMOR::CSKYARMOR(int x, int y, int speed, int direction)
    : CVEHICLE(x, y, speed, direction) {
    mWidth = 130;
    mHeight = 80;
}

void CSKYARMOR::Move(int limitX1, int limitX2) {
    mX += mDirection * mSpeed;
    if (mDirection == 1 && mX > limitX2) {
        mX = limitX1 - mWidth;
    } else if (mDirection == -1 && mX < limitX1 - mWidth) {
        mX = limitX2;
    }
}

void CSKYARMOR::draw(SDL_Renderer* renderer, CFont& font, float cameraY) {
    float baseY = (float)mY - cameraY;
    if (mTexture) {
        float drawW = (float)mWidth * 1.3f;
        float drawH = (float)mHeight * 1.3f;
        float drawX = (float)mX - (drawW - (float)mWidth) / 2.0f;
        float drawY = baseY - (drawH - (float)mHeight) / 2.0f;

        SDL_FRect dstRect = { drawX, drawY, drawW, drawH };
        SDL_FlipMode flip = (mDirection == 1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderTextureRotated(renderer, mTexture, NULL, &dstRect, 0.0f, NULL, flip);
        return;
    }
    // Fallback: Grey Armor box for CSKYARMOR
    SDL_SetRenderDrawColor(renderer, 120, 130, 140, 255);
    SDL_FRect body = { (float)mX, baseY + 5, (float)mWidth, (float)mHeight - 10 };
    SDL_RenderFillRect(renderer, &body);

    SDL_Color textCol = { 255, 255, 255, 255 };
    font.drawText(renderer, "SKY", mX + 15, (int)(baseY + 28), 1, textCol);
}
