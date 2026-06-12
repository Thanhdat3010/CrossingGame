#ifndef CSKYARMOR_H
#define CSKYARMOR_H

#include "CVEHICLE.h"

class CSKYARMOR : public CVEHICLE {
public:
    CSKYARMOR(int x, int y, int speed, int direction);
    void Move(int limitX1, int limitX2) override;
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY) override;
};

#endif // CSKYARMOR_H
