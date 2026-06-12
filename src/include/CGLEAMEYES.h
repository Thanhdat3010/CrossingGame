#ifndef CGLEAMEYES_H
#define CGLEAMEYES_H

#include "CANIMAL.h"

class CGLEAMEYES : public CANIMAL {
private:
    int mArmorGlow;

public:
    CGLEAMEYES(int x, int y, int speed, int direction);
    virtual ~CGLEAMEYES();

    void Move(int limitX1, int limitX2) override;
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY) override;
};

#endif // CGLEAMEYES_H
