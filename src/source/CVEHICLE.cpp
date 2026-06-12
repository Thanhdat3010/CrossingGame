#include "CVEHICLE.h"

CVEHICLE::CVEHICLE(int x, int y, int speed, int direction)
    : mX(x), mY(y), mWidth(48), mHeight(48), mSpeed(speed), mDirection(direction), mTexture(nullptr) {
}
