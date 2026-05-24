#include "Animal.h"
#include <iostream>
#include <SDL3_mixer/SDL_mixer.h> // Sẽ dùng để phát SFX ở Phase 2

// ====================================================================
// TRIỂN KHAI LỚP CANIMAL (Base Class)
// ====================================================================
CANIMAL::CANIMAL(int x, int y, int speed, int direction)
    : mX(x), mY(y), mSpeed(speed), mDirection(direction), mWidth(48), mHeight(48), mTexture1(nullptr), mTexture2(nullptr) {
}

CANIMAL::~CANIMAL() {}


// ====================================================================
// TRIỂN KHAI LỚP CDINAUSOR (Khủng long bạo chúa T-Rex)
// ====================================================================
CDINAUSOR::CDINAUSOR(int x, int y, int speed, int direction)
    : CANIMAL(x, y, speed, direction) {
    mWidth = 110;  // Quái rừng rộng 110px
    mHeight = 80;  // Cao 80px / lane 80px → phủ kín làn đường
}

CDINAUSOR::~CDINAUSOR() {}

void CDINAUSOR::Move(int limitX1, int limitX2) {
    mX += mDirection * mSpeed;

    // Cơ chế Băng rìa màn hình (Wrap-around)
    if (mDirection == 1 && mX > limitX2) {
        mX = limitX1 - mWidth;
    }
    else if (mDirection == -1 && mX < limitX1 - mWidth) {
        mX = limitX2;
    }
}

void CDINAUSOR::Tell() {
    // Phase 1: In tạm log ra màn hình
    // Phase 2: Sẽ phát file âm thanh gầm rú: Mix_PlayChannel(-1, roarSFX, 0);
    std::cout << "CDINAUSOR: ROOAAARRRR!" << std::endl;
}

void CDINAUSOR::draw(SDL_Renderer* renderer, CFont& font) {
    if (mTexture1) {
        // Tính toán frame hiện tại dựa vào vị trí di chuyển mX để khớp nhịp chân chạm đất với tốc độ chạy thật
        int frameIndex = (std::abs(mX) / 120) % 2; // Cứ đi 120px đổi 1 frame luân phiên (chậm lại gấp ba để có bước chạy tự nhiên, thư thả)
        SDL_Texture* activeTex = (frameIndex == 0) ? mTexture1 : mTexture2;
        if (!activeTex) activeTex = mTexture1;

        // Tăng kích thước vẽ trực quan (visual scale) lên 1.3 lần để bù đắp viền trong suốt của AI, giữ collision công bằng
        float drawW = (float)mWidth * 1.3f;
        float drawH = (float)mHeight * 1.3f;
        float drawX = (float)mX - (drawW - (float)mWidth) / 2.0f;
        float drawY = (float)mY - (drawH - (float)mHeight) / 2.0f;

        SDL_FRect dstRect = { drawX, drawY, drawW, drawH };
        SDL_FlipMode flip = (mDirection == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        
        SDL_RenderTextureRotated(renderer, activeTex, NULL, &dstRect, 0.0f, NULL, flip);
        return;
    }
    // Vẽ Khủng long T-Rex Pixel phong cách dễ thương (emerald green)
    // Thân mình chính (Chữ nhật đứng bo)
    SDL_SetRenderDrawColor(renderer, 46, 117, 89, 255); // Xanh lá đậm T-Rex
    SDL_FRect body = { (float)mX + 16, (float)mY + 12, 32.0f, 24.0f };
    SDL_RenderFillRect(renderer, &body);

    // Đầu khủng long T-Rex to ngầu
    SDL_FRect head;
    if (mDirection == 1) {
        head = { (float)mX + 32, (float)mY + 2, 24.0f, 16.0f };
    } else {
        head = { (float)mX + 8, (float)mY + 2, 24.0f, 16.0f };
    }
    SDL_RenderFillRect(renderer, &head);

    // Mắt T-Rex (Màu vàng rực sành điệu)
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    SDL_FRect eye;
    if (mDirection == 1) {
        eye = { (float)mX + 46, (float)mY + 6, 4.0f, 4.0f };
    } else {
        eye = { (float)mX + 14, (float)mY + 6, 4.0f, 4.0f };
    }
    SDL_RenderFillRect(renderer, &eye);

    // Đuôi khủng long
    SDL_SetRenderDrawColor(renderer, 46, 117, 89, 255);
    SDL_FRect tail;
    if (mDirection == 1) {
        tail = { (float)mX, (float)mY + 20, 16.0f, 12.0f };
    } else {
        tail = { (float)mX + 48, (float)mY + 20, 16.0f, 12.0f };
    }
    SDL_RenderFillRect(renderer, &tail);

    // Hiệu ứng bước chạy chân (Chân đập liên tục nhịp nhàng theo thời gian)
    Uint64 ticks = SDL_GetTicks();
    bool legState = (ticks / 450) % 2 == 0; // Đổi chân chạy mỗi 450ms (chậm lại gấp ba cho tự nhiên)

    SDL_SetRenderDrawColor(renderer, 24, 78, 55, 255); // Chân màu xanh sẫm hơn
    SDL_FRect leg1, leg2;
    if (legState) {
        // Chân 1 co, Chân 2 thẳng
        leg1 = { (float)mX + 20, (float)mY + 36, 6.0f, 8.0f };
        leg2 = { (float)mX + 38, (float)mY + 36, 6.0f, 12.0f };
    } else {
        // Chân 1 thẳng, Chân 2 co
        leg1 = { (float)mX + 20, (float)mY + 36, 6.0f, 12.0f };
        leg2 = { (float)mX + 38, (float)mY + 36, 6.0f, 8.0f };
    }
    SDL_RenderFillRect(renderer, &leg1);
    SDL_RenderFillRect(renderer, &leg2);

    // Vẽ chữ "DINO" nhỏ trên mình trang trí
    SDL_Color textCol = {255, 255, 255, 255};
    font.drawText(renderer, "DINO", mX + (mDirection == 1 ? 18 : 22), mY + 18, 1, textCol);
}


// ====================================================================
// TRIỂN KHAI LỚP CBIRD (Chim đại bàng bay lượn)
// ====================================================================
CBIRD::CBIRD(int x, int y, int speed, int direction)
    : CANIMAL(x, y, speed, direction) {
    mWidth = 100;  // Quái bay rộng 100px
    mHeight = 80;  // Cao 80px / lane 80px → phủ kín làn đường
}

CBIRD::~CBIRD() {}

void CBIRD::Move(int limitX1, int limitX2) {
    mX += mDirection * mSpeed;

    // Bay lượn lên xuống nhẹ hình sin cho tự nhiên sinh động
    Uint64 ticks = SDL_GetTicks();
    mY += (int)(sinf((float)ticks / 200.0f) * 1.5f); // Nhấp nhô 1.5 pixel tạo nhịp bay

    if (mDirection == 1 && mX > limitX2) {
        mX = limitX1 - mWidth;
    }
    else if (mDirection == -1 && mX < limitX1 - mWidth) {
        mX = limitX2;
    }
}

void CBIRD::Tell() {
    // In tạm log ra màn hình
    // Mix_PlayChannel(-1, chirpSFX, 0);
    std::cout << "CBIRD: CHIRP CHIRP!" << std::endl;
}

void CBIRD::draw(SDL_Renderer* renderer, CFont& font) {
    if (mTexture1) {
        // Tính toán frame bay dựa vào vị trí mX
        int frameIndex = (std::abs(mX) / 140) % 2; // Cứ đi 140px đổi 1 frame vỗ cánh luân phiên (chậm lại gấp bốn cho cánh vỗ uyển chuyển)
        SDL_Texture* activeTex = (frameIndex == 0) ? mTexture1 : mTexture2;
        if (!activeTex) activeTex = mTexture1;

        // Tăng kích thước vẽ trực quan (visual scale) lên 1.3 lần để bù đắp viền trong suốt của AI, giữ va chạm chính xác
        float drawW = (float)mWidth * 1.3f;
        float drawH = (float)mHeight * 1.3f;
        float drawX = (float)mX - (drawW - (float)mWidth) / 2.0f;
        float drawY = (float)mY - (drawH - (float)mHeight) / 2.0f;

        SDL_FRect dstRect = { drawX, drawY, drawW, drawH };
        SDL_FlipMode flip = (mDirection == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        
        SDL_RenderTextureRotated(renderer, activeTex, NULL, &dstRect, 0.0f, NULL, flip);
        return;
    }
    // Vẽ chim phượng hoàng lửa pixel đỏ hồng siêu dễ thương
    // Thân chim
    SDL_SetRenderDrawColor(renderer, 240, 84, 84, 255); // Màu đỏ hồng Ruby
    SDL_FRect body = { (float)mX + 12, (float)mY + 10, 24.0f, 16.0f };
    SDL_RenderFillRect(renderer, &body);

    // Đầu chim + Mỏ
    SDL_FRect head;
    SDL_FRect beak;
    if (mDirection == 1) {
        head = { (float)mX + 32, (float)mY + 6, 12.0f, 12.0f };
        beak = { (float)mX + 44, (float)mY + 10, 6.0f, 4.0f };
    } else {
        head = { (float)mX + 4, (float)mY + 6, 12.0f, 12.0f };
        beak = { (float)mX - 2, (float)mY + 10, 6.0f, 4.0f };
    }
    SDL_RenderFillRect(renderer, &head);
    SDL_SetRenderDrawColor(renderer, 255, 159, 67, 255); // Mỏ màu cam rực
    SDL_RenderFillRect(renderer, &beak);

    // Đuôi chim đuôi én pixel
    SDL_SetRenderDrawColor(renderer, 240, 84, 84, 255);
    SDL_FRect tail;
    if (mDirection == 1) {
        tail = { (float)mX, (float)mY + 14, 12.0f, 8.0f };
    } else {
        tail = { (float)mX + 36, (float)mY + 14, 12.0f, 8.0f };
    }
    SDL_RenderFillRect(renderer, &tail);

    // Animation Vỗ Cánh (Lên / Xuống) mỗi 300ms
    Uint64 ticks = SDL_GetTicks();
    bool wingUp = (ticks / 300) % 2 == 0;

    SDL_SetRenderDrawColor(renderer, 48, 227, 202, 255); // Cánh chim màu Cyan ngọc bích nổi bật
    SDL_FRect wing;
    if (wingUp) {
        // Cánh vỗ lên trên
        wing = { (float)mX + 18, (float)mY, 10.0f, 12.0f };
    } else {
        // Cánh vỗ xuống dưới
        wing = { (float)mX + 18, (float)mY + 22, 10.0f, 12.0f };
    }
    SDL_RenderFillRect(renderer, &wing);

    // Vẽ chữ "BIRD" nhỏ xinh xắn
    SDL_Color textCol = {255, 255, 255, 255};
    font.drawText(renderer, "BIRD", mX + (mDirection == 1 ? 13 : 21), mY + 14, 1, textCol);
}
