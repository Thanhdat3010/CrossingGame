#ifndef CHEATHCLIFF_H
#define CHEATHCLIFF_H

#include "CANIMAL.h"

class CHEATHCLIFF : public CANIMAL {
private:
    int mDashPhase;

public:
    CHEATHCLIFF(int x, int y, int speed, int direction);
    virtual ~CHEATHCLIFF();

    void Move(int limitX1, int limitX2) override;
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY) override;
};

#endif // CHEATHCLIFF_H
