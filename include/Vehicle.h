#ifndef VEHICLE_H
#define VEHICLE_H

#include <SDL3/SDL.h>

class CVEHICLE {
protected:
    int mX;
    int mY;
    int mWidth;
    int mHeight;

public:
    CVEHICLE(int x = 0, int y = 0, int width = 0, int height = 0)
        : mX(x), mY(y), mWidth(width), mHeight(height) {}
    virtual ~CVEHICLE() = default;

    int getX() const { return mX; }
    int getY() const { return mY; }
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }

    SDL_FRect getHitbox() const {
        const float padX = mWidth * 0.15f;
        const float padY = mHeight * 0.15f;
        return { (float)mX + padX, (float)mY + padY,
                 (float)mWidth - 2 * padX, (float)mHeight - 2 * padY };
    }
};

#endif // VEHICLE_H
