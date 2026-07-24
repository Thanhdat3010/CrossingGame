#ifndef CICEDRAGON_H
#define CICEDRAGON_H

#include "CANIMAL.h"

class CICEDRAGON : public CANIMAL {
private:
    float mWingPulse;

public:
    CICEDRAGON(int x, int y, int speed, int direction);
    virtual ~CICEDRAGON();

    void Move(int limitX1, int limitX2) override;
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY) override;
    void Tell(struct MIX_Mixer* mixer = nullptr, struct MIX_Audio* sound = nullptr) override;
};

#endif // CICEDRAGON_H
