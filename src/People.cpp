#include "People.h"

CPEOPLE::CPEOPLE() {
    mWidth = 32;
    mHeight = 32;
    mSpeed = 16; // Tốc độ di chuyển mỗi lần nhấn phím
    mState = true; // Ban đầu còn sống
    resetPosition();
}

CPEOPLE::~CPEOPLE() {}

void CPEOPLE::resetPosition() {
    // Xuất phát ở chính giữa bên dưới màn hình (1280 x 720)
    // Trừ đi nửa độ rộng nhân vật (32/2 = 16) để căn đúng tâm
    mX = 640 - (mWidth / 2);
    mY = 650; // Vỉa hè xuất phát phía dưới
    mState = true;
}

void CPEOPLE::Up(int limitY) {
    if (isDead()) return;
    mY -= mSpeed;
    if (mY < limitY) {
        mY = limitY; // Giới hạn biên trên
    }
}

void CPEOPLE::Down(int limitY) {
    if (isDead()) return;
    mY += mSpeed;
    if (mY > limitY) {
        mY = limitY; // Giới hạn biên dưới
    }
}

void CPEOPLE::Left(int limitX) {
    if (isDead()) return;
    mX -= mSpeed;
    if (mX < limitX) {
        mX = limitX; // Giới hạn biên trái
    }
}

void CPEOPLE::Right(int limitX) {
    if (isDead()) return;
    mX += mSpeed;
    if (mX > limitX) {
        mX = limitX; // Giới hạn biên phải
    }
}

void CPEOPLE::draw(SDL_Renderer* renderer, CFont& font) {
    if (isDead()) {
        // Vẽ nhân vật đã chết màu Xám hoặc biểu tượng chữ X
        SDL_Color deadColor = {180, 180, 180, 255};
        font.drawText(renderer, "X", mX, mY, 4, deadColor);
    } else {
        // Vẽ ký tự 'Y' pixel màu xanh navy hoặc tím cá tính cho người chơi
        SDL_Color playerColor = {67, 97, 238, 255}; // Màu xanh nước biển sâu thời thượng
        font.drawText(renderer, "Y", mX, mY, 4, playerColor);
    }
}

bool CPEOPLE::isFinish() const {
    // Vỉa hè về đích nằm ở biên trên (y <= 80px)
    return mY <= 80;
}
