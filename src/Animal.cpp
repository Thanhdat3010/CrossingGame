#include "Animal.h"

CANIMAL::CANIMAL(int x, int y, int speed, int direction)
    : mX(x), mY(y), mWidth(48), mHeight(48), mSpeed(speed), mDirection(direction), mTexture1(nullptr), mTexture2(nullptr) {
}

CANIMAL::~CANIMAL() {}

// ====================================================================
// TRIỂN KHAI LỚP CILLFANG
// ====================================================================
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
    SDL_SetRenderDrawColor(renderer, 46, 117, 89, 255); // Xanh lá đậm T-Rex
    SDL_FRect body = { (float)mX + 16, baseY + 12, 32.0f, 24.0f };
    SDL_RenderFillRect(renderer, &body);

    // Đầu khủng long T-Rex to ngầu
    SDL_FRect head;
    if (mDirection == 1) {
        head = { (float)mX + 32, baseY + 2, 24.0f, 16.0f };
    } else {
        head = { (float)mX + 8, baseY + 2, 24.0f, 16.0f };
    }
    SDL_RenderFillRect(renderer, &head);

    // Mắt T-Rex (Màu vàng rực sành điệu)
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_FRect eye;
    if (mDirection == 1) {
        eye = { (float)mX + 46, baseY + 6, 4.0f, 4.0f };
    } else {
        eye = { (float)mX + 14, baseY + 6, 4.0f, 4.0f };
    }
    SDL_RenderFillRect(renderer, &eye);

    // Đuôi khủng long
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

    SDL_SetRenderDrawColor(renderer, 24, 78, 55, 255); // Chân màu xanh sẫm hơn
    SDL_FRect leg1, leg2;
    if (legState) {
        // Chân 1 co, Chân 2 thẳng
        leg1 = { (float)mX + 20, baseY + 36, 6.0f, 8.0f };
        leg2 = { (float)mX + 38, baseY + 36, 6.0f, 12.0f };
    } else {
        // Chân 1 thẳng, Chân 2 co
        leg1 = { (float)mX + 20, baseY + 36, 6.0f, 12.0f };
        leg2 = { (float)mX + 38, baseY + 36, 6.0f, 8.0f };
    }
    SDL_RenderFillRect(renderer, &leg1);
    SDL_RenderFillRect(renderer, &leg2);

    SDL_Color textCol = {255, 255, 255, 255};
    font.drawText(renderer, "FANG", mX + (mDirection == 1 ? 18 : 22), (int)(baseY + 18), 1, textCol);
}


// ====================================================================
// TRIỂN KHAI LỚP CICEDRAGON
// ====================================================================
CICEDRAGON::CICEDRAGON(int x, int y, int speed, int direction)
    : CANIMAL(x, y, speed, direction), mWingPulse(0.0f) {
    mWidth = 100;
    mHeight = 80;
}

CICEDRAGON::~CICEDRAGON() {}

void CICEDRAGON::Move(int limitX1, int limitX2) {
    mX += mDirection * mSpeed;

    Uint64 ticks = SDL_GetTicks();
    mWingPulse = sinf((float)ticks / 200.0f);
    mY += (int)(mWingPulse * 1.5f);

    if (mDirection == 1 && mX > limitX2) {
        mX = limitX1 - mWidth;
    }
    else if (mDirection == -1 && mX < limitX1 - mWidth) {
        mX = limitX2;
    }
}

void CICEDRAGON::draw(SDL_Renderer* renderer, CFont& font, float cameraY) {
    float baseY = (float)mY - cameraY;
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
    SDL_SetRenderDrawColor(renderer, 240, 84, 84, 255); // Màu đỏ hồng Ruby
    SDL_FRect body = { (float)mX + 12, baseY + 10, 24.0f, 16.0f };
    SDL_RenderFillRect(renderer, &body);

    // Đầu chim + Mỏ
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
    SDL_SetRenderDrawColor(renderer, 255, 159, 67, 255); // Mỏ màu cam rực
    SDL_RenderFillRect(renderer, &beak);

    // Đuôi chim đuôi én pixel
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

    SDL_SetRenderDrawColor(renderer, 48, 227, 202, 255); // Cánh chim màu Cyan ngọc bích nổi bật
    SDL_FRect wing;
    if (wingUp) {
        // Cánh vỗ lên trên
        wing = { (float)mX + 18, baseY, 10.0f, 12.0f };
    } else {
        // Cánh vỗ xuống dưới
        wing = { (float)mX + 18, baseY + 22, 10.0f, 12.0f };
    }
    SDL_RenderFillRect(renderer, &wing);

    SDL_Color textCol = {255, 255, 255, 255};
    font.drawText(renderer, "ICE", mX + (mDirection == 1 ? 13 : 21), (int)(baseY + 14), 1, textCol);
}

// ====================================================================
// TRIỂN KHAI LỚP CHEATHCLIFF
// ====================================================================
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

// ====================================================================
// TRIỂN KHAI LỚP CGLEAMEYES
// ====================================================================
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
