#include "CGLEAMEYES.h"
#include <cmath>

CGLEAMEYES::CGLEAMEYES(int x, int y, int speed, int direction)
    : CANIMAL(x, y, speed, direction), mArmorGlow(0) {
    mWidth = 112;
    mHeight = 80;
}

CGLEAMEYES::~CGLEAMEYES() {}

void CGLEAMEYES::Move(int limitX1, int limitX2) {
    mX += mDirection * mSpeed;

    Uint64 ticks = SDL_GetTicks();
    mArmorGlow = (int)((sinf((float)ticks / 150.0f) + 1.0f) * 127.0f);

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
        int frameIndex = (std::abs(mX) / 110) % 2;
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
    SDL_SetRenderDrawColor(renderer, 20, 25, 45, 255);
    SDL_FRect body = { (float)mX + 16, baseY + 6, 80.0f, 32.0f };
    SDL_RenderFillRect(renderer, &body);

    SDL_SetRenderDrawColor(renderer, 80, 200, 255, (Uint8)mArmorGlow);
    SDL_FRect glow = { (float)mX + (mDirection == 1 ? 108.0f : -4.0f), baseY + 14, 8.0f, 8.0f };
    SDL_RenderFillRect(renderer, &glow);

    SDL_Color textCol = {255, 255, 255, 255};
    font.drawText(renderer, "EYES", mX + 22, (int)(baseY + 18), 1, textCol);
}

void CGLEAMEYES::Tell(MIX_Mixer* mixer, MIX_Audio* sound) {
    CANIMAL::Tell(mixer, sound);
}
