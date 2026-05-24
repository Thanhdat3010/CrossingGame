#include "Vehicle.h"

// ====================================================================
// TRIỂN KHAI LỚP CVEHICLE (Base Class)
// ====================================================================
CVEHICLE::CVEHICLE(int x, int y, int speed, int direction)
    : mX(x), mY(y), mSpeed(speed), mDirection(direction), mWidth(64), mHeight(48), mTexture(nullptr) {
    // Giá trị width/height mặc định, các lớp con sẽ ghi đè lên để khớp kích thước vẽ
}

CVEHICLE::~CVEHICLE() {}


// ====================================================================
// TRIỂN KHAI LỚP CTRUCK (Xe tải / Quái vật khổng lồ)
// ====================================================================
CTRUCK::CTRUCK(int x, int y, int speed, int direction)
    : CVEHICLE(x, y, speed, direction) {
    mWidth = 140;  // Quái khổng lồ rộng 140px
    mHeight = 70;  // Cao 70px / lane 80px → gần phủ kín đường
}

CTRUCK::~CTRUCK() {}

void CTRUCK::Move(int limitX1, int limitX2) {
    // Di chuyển tịnh tiến theo hướng mDirection * mSpeed * deltaTime
    // Do deltaTime được xử lý ở Game loop, ở đây chúng ta nhận mSpeed làm lượng dịch chuyển từng frame
    mX += mDirection * mSpeed;

    // Cơ chế Wrap-around (Băng qua rìa màn hình thì xuất hiện lại ở rìa đối diện)
    if (mDirection == 1 && mX > limitX2) {
        mX = limitX1 - mWidth; // Đi quá biên phải -> đưa về biên trái
    }
    else if (mDirection == -1 && mX < limitX1 - mWidth) {
        mX = limitX2;          // Đi quá biên trái -> đưa về biên phải
    }
}

void CTRUCK::draw(SDL_Renderer* renderer, CFont& font) {
    if (mTexture) {
        SDL_FRect dstRect = { (float)mX, (float)mY, (float)mWidth, (float)mHeight };
        SDL_FlipMode flip = (mDirection == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderTextureRotated(renderer, mTexture, NULL, &dstRect, 0.0, NULL, flip);
        return;
    }
    // Vẽ xe tải retro pixel bằng các hình chữ nhật lồng nhau
    // Thân sau xe (Thùng hàng - Teal Blue thời thượng)
    SDL_SetRenderDrawColor(renderer, 43, 45, 66, 255);
    SDL_FRect cargoRect;
    
    // Thân trước xe (Cabin - Đỏ tươi nổi bật)
    SDL_SetRenderDrawColor(renderer, 230, 57, 70, 255);
    SDL_FRect cabinRect;
    
    // Cửa kính cabin
    SDL_SetRenderDrawColor(renderer, 241, 250, 238, 255);
    SDL_FRect windowRect;

    if (mDirection == 1) {
        // Hướng sang phải: Thùng hàng bên trái, Cabin bên phải
        cargoRect = { (float)mX, (float)mY + 4, 60.0f, 32.0f };
        cabinRect = { (float)mX + 60, (float)mY + 12, 36.0f, 24.0f };
        windowRect = { (float)mX + 75, (float)mY + 16, 12.0f, 10.0f };
    } else {
        // Hướng sang trái: Cabin bên trái, Thùng hàng bên phải
        cabinRect = { (float)mX, (float)mY + 12, 36.0f, 24.0f };
        cargoRect = { (float)mX + 36, (float)mY + 4, 60.0f, 32.0f };
        windowRect = { (float)mX + 9, (float)mY + 16, 12.0f, 10.0f };
    }
    
    SDL_RenderFillRect(renderer, &cargoRect);
    SDL_RenderFillRect(renderer, &cabinRect);
    SDL_RenderFillRect(renderer, &windowRect);

    // Vẽ bánh xe (Bánh xe đen tròn / hình vuông pixel retro)
    SDL_SetRenderDrawColor(renderer, 29, 53, 87, 255); // Bánh xe xanh đen đậm
    SDL_FRect wheel1, wheel2, wheel3;
    if (mDirection == 1) {
        wheel1 = { (float)mX + 12, (float)mY + 34, 14.0f, 10.0f };
        wheel2 = { (float)mX + 38, (float)mY + 34, 14.0f, 10.0f };
        wheel3 = { (float)mX + 72, (float)mY + 34, 14.0f, 10.0f };
    } else {
        wheel1 = { (float)mX + 10, (float)mY + 34, 14.0f, 10.0f };
        wheel2 = { (float)mX + 44, (float)mY + 34, 14.0f, 10.0f };
        wheel3 = { (float)mX + 70, (float)mY + 34, 14.0f, 10.0f };
    }
    SDL_RenderFillRect(renderer, &wheel1);
    SDL_RenderFillRect(renderer, &wheel2);
    SDL_RenderFillRect(renderer, &wheel3);

    // Vẽ chữ "TRUCK" nhỏ trên thùng hàng trang trí thêm
    SDL_Color textColor = {255, 255, 255, 255};
    font.drawText(renderer, "TRUCK", mX + (mDirection == 1 ? 12 : 48), mY + 14, 1, textColor);
}


// ====================================================================
// TRIỂN KHAI LỚP CCAR (Xe con / Xe thể thao / Quái vật tốc độ)
// ====================================================================
CCAR::CCAR(int x, int y, int speed, int direction)
    : CVEHICLE(x, y, speed, direction) {
    mWidth = 120;  // Quái tốc độ rộng 120px
    mHeight = 65;  // Cao 65px / lane 80px → gần phủ kín đường
}

CCAR::~CCAR() {}

void CCAR::Move(int limitX1, int limitX2) {
    mX += mDirection * mSpeed;

    if (mDirection == 1 && mX > limitX2) {
        mX = limitX1 - mWidth;
    }
    else if (mDirection == -1 && mX < limitX1 - mWidth) {
        mX = limitX2;
    }
}

void CCAR::draw(SDL_Renderer* renderer, CFont& font) {
    if (mTexture) {
        SDL_FRect dstRect = { (float)mX, (float)mY, (float)mWidth, (float)mHeight };
        SDL_FlipMode flip = (mDirection == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderTextureRotated(renderer, mTexture, NULL, &dstRect, 0.0, NULL, flip);
        return;
    }
    // Vẽ xe thể thao thon gọn, khí động học
    // Thân xe dưới (Cam san hô rực rỡ / Orange Neon)
    SDL_SetRenderDrawColor(renderer, 247, 127, 0, 255);
    SDL_FRect bodyRect = { (float)mX, (float)mY + 12, 72.0f, 16.0f };
    SDL_RenderFillRect(renderer, &bodyRect);

    // Cabin mui xe (Vàng tươi mượt mà)
    SDL_SetRenderDrawColor(renderer, 252, 191, 73, 255);
    SDL_FRect roofRect;
    if (mDirection == 1) {
        roofRect = { (float)mX + 16, (float)mY + 2, 36.0f, 10.0f };
    } else {
        roofRect = { (float)mX + 20, (float)mY + 2, 36.0f, 10.0f };
    }
    SDL_RenderFillRect(renderer, &roofRect);

    // Cửa kính xe
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_FRect windowRect;
    if (mDirection == 1) {
        windowRect = { (float)mX + 32, (float)mY + 4, 16.0f, 6.0f };
    } else {
        windowRect = { (float)mX + 24, (float)mY + 4, 16.0f, 6.0f };
    }
    SDL_RenderFillRect(renderer, &windowRect);

    // 2 Bánh xe thăng bằng
    SDL_SetRenderDrawColor(renderer, 0, 48, 73, 255);
    SDL_FRect w1 = { (float)mX + 10, (float)mY + 24, 12.0f, 8.0f };
    SDL_FRect w2 = { (float)mX + 50, (float)mY + 24, 12.0f, 8.0f };
    SDL_RenderFillRect(renderer, &w1);
    SDL_RenderFillRect(renderer, &w2);

    // Đèn pha trước xe (Chữ nhật màu vàng phát sáng nhẹ)
    SDL_SetRenderDrawColor(renderer, 255, 235, 41, 200);
    SDL_FRect headlight;
    if (mDirection == 1) {
        headlight = { (float)mX + 70, (float)mY + 16, 4.0f, 6.0f };
    } else {
        headlight = { (float)mX - 2, (float)mY + 16, 4.0f, 6.0f };
    }
    SDL_RenderFillRect(renderer, &headlight);

    // Vẽ chữ "CAR" siêu nhỏ trên cửa xe
    SDL_Color textCol = {0, 0, 0, 255};
    font.drawText(renderer, "CAR", mX + 26, mY + 16, 1, textCol);
}
