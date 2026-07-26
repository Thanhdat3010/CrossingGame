#include "CICEDRAGON.h"
#include <cmath>

CICEDRAGON::CICEDRAGON(int x, int y, int speed, int direction)
    : CANIMAL(x, y, speed, direction), mWingPulse(0.0f) {
    mWidth = 100;
    mHeight = 80;
}

CICEDRAGON::~CICEDRAGON() {}

void CICEDRAGON::Move(int limitX1, int limitX2) {
    mX += mDirection * mSpeed;

    if (mDirection == 1 && mX > limitX2) {
        mX = limitX1 - mWidth;
    }
    else if (mDirection == -1 && mX < limitX1 - mWidth) {
        mX = limitX2;
    }
}

void CICEDRAGON::draw(SDL_Renderer* renderer, CFont& font, float cameraY) {
    float hoverY = sinf((float)SDL_GetTicks() / 200.0f) * 3.0f;
    float baseY = (float)mY - cameraY + hoverY;
    if (mTexture1) {
        int frameIndex = (std::abs(mX) / 140) % 2;
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
    SDL_SetRenderDrawColor(renderer, 240, 84, 84, 255);
    SDL_FRect body = { (float)mX + 12, baseY + 10, 24.0f, 16.0f };
    SDL_RenderFillRect(renderer, &body);

    SDL_FRect head;
    SDL_FRect beak;
    if (mDirection == 1) {
        head = { (float)mX + 32, baseY + 6, 12.0f, 12.0f };
        beak = { (float)mX + 44, baseY + 10, 6.0f, 4.0f };
    } else {
        head = { (float)mX + 4, baseY + 6, 12.0f, 12.0f };
        beak = { (float)mX - 2, baseY + 10, 6.0f, 4.0f };
    }
    SDL_RenderFillRect(renderer, &head);
    SDL_SetRenderDrawColor(renderer, 255, 159, 67, 255);
    SDL_RenderFillRect(renderer, &beak);

    SDL_SetRenderDrawColor(renderer, 240, 84, 84, 255);
    SDL_FRect tail;
    if (mDirection == 1) {
        tail = { (float)mX, baseY + 14, 12.0f, 8.0f };
    } else {
        tail = { (float)mX + 36, baseY + 14, 12.0f, 8.0f };
    }
    SDL_RenderFillRect(renderer, &tail);

    Uint64 ticks = SDL_GetTicks();
    bool wingUp = (ticks / 300) % 2 == 0;

    SDL_SetRenderDrawColor(renderer, 48, 227, 202, 255);
    SDL_FRect wing;
    if (wingUp) {
        wing = { (float)mX + 18, baseY, 10.0f, 12.0f };
    } else {
        wing = { (float)mX + 18, baseY + 22, 10.0f, 12.0f };
    }
    SDL_RenderFillRect(renderer, &wing);

    SDL_Color textCol = {255, 255, 255, 255};
    font.drawText(renderer, "ICE", mX + 22, (int)(baseY + 18), 1, textCol);
}

void CICEDRAGON::Tell(MIX_Mixer* mixer, MIX_Audio* sound) {
    CANIMAL::Tell(mixer, sound);
}
