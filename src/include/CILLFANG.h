#ifndef CILLFANG_H
#define CILLFANG_H

#include "CANIMAL.h"

class CILLFANG : public CANIMAL {
private:
    int mStridePhase;

public:
    CILLFANG(int x, int y, int speed, int direction);
    virtual ~CILLFANG();

    void Move(int limitX1, int limitX2) override;
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY) override;
};

#endif // CILLFANG_H
