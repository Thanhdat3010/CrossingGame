#ifndef CBLUEWING_H
#define CBLUEWING_H

#include "CVEHICLE.h"

class CBLUEWING : public CVEHICLE {
public:
    CBLUEWING(int x, int y, int speed, int direction);
    void Move(int limitX1, int limitX2) override;
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY) override;
};

#endif // CBLUEWING_H
