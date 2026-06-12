#include "CANIMAL.h"

CANIMAL::CANIMAL(int x, int y, int speed, int direction)
    : mX(x), mY(y), mWidth(48), mHeight(48), mSpeed(speed), mDirection(direction), mTexture1(nullptr), mTexture2(nullptr) {
}

CANIMAL::~CANIMAL() {}
