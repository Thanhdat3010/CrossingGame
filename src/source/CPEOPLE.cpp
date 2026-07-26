#include "CPEOPLE.h"
#include "CVEHICLE.h"
#include "CANIMAL.h"
#include <SDL3_image/SDL_image.h>
#include <cmath>

CPEOPLE::CPEOPLE() {
    mCharType = CharacterType::KIRITO;
    mKiritoTexture = nullptr;
    mAsunaTexture = nullptr;
    mState = true;
    mDrawX = 0;
    mDrawY = 0;
    mAnimTimer = 0.0f;
    setCharacter(CharacterType::KIRITO);
    resetPosition();
}

CPEOPLE::~CPEOPLE() {
    if (mKiritoTexture) {
        SDL_DestroyTexture(mKiritoTexture);
        mKiritoTexture = nullptr;
    }
    if (mAsunaTexture) {
        SDL_DestroyTexture(mAsunaTexture);
        mAsunaTexture = nullptr;
    }
}

void CPEOPLE::update(float deltaTime) {
    if (isDead()) return;

    if (deltaTime > 0.1f) deltaTime = 0.1f;

    const float LERP_RATE = 15.0f;
    mDrawX += ((float)mX - mDrawX) * LERP_RATE * deltaTime;
    mDrawY += ((float)mY - mDrawY) * LERP_RATE * deltaTime;

    if (std::abs((float)mX - mDrawX) < 0.2f) mDrawX = (float)mX;
    if (std::abs((float)mY - mDrawY) < 0.2f) mDrawY = (float)mY;

    mAnimTimer += deltaTime;
}

void CPEOPLE::resetPosition() {
    mX = 640 - (mWidth / 2);
    mY = 600;
    mState = true;
    mDrawX = (float)mX;
    mDrawY = (float)mY;
    mAnimTimer = 0.0f;
}

void CPEOPLE::setPosition(int x, int y) {
    mX = x;
    mY = y;
    mDrawX = (float)x;
    mDrawY = (float)y;
    mAnimTimer = 0.0f;
}

void CPEOPLE::Up(int limitY) {
    if (isDead()) return;
    mY -= mSpeed;
    if (mY < limitY) {
        mY = limitY;
    }
}

void CPEOPLE::Down(int limitY) {
    if (isDead()) return;
    mY += mSpeed;
    if (mY > limitY) {
        mY = limitY;
    }
}

void CPEOPLE::Left(int limitX) {
    if (isDead()) return;
    mX -= mSpeed;
    if (mX < limitX) {
        mX = limitX;
    }
}

void CPEOPLE::Right(int limitX) {
    if (isDead()) return;
    mX += mSpeed;
    if (mX > limitX) {
        mX = limitX;
    }
}

void CPEOPLE::setCharacter(CharacterType type) {
    mCharType = type;
    if (mCharType == CharacterType::KIRITO) {
        mSpeed = 80;
        mWidth = 80;
        mHeight = 80;
    } else {
        mSpeed = 80;
        mWidth = 80;
        mHeight = 80;
    }
}

void CPEOPLE::loadTextures(SDL_Renderer* renderer) {
    if (!renderer) return;

    mKiritoTexture = IMG_LoadTexture(renderer, "assets/images/characters/kirito.png");
    if (!mKiritoTexture) {
        SDL_Log("Chua co assets/images/characters/kirito.png, game se su dung che do fallback ve chu K pixel.");
    }

    mAsunaTexture = IMG_LoadTexture(renderer, "assets/images/characters/asuna.png");
    if (!mAsunaTexture) {
        SDL_Log("Chua co assets/images/characters/asuna.png, game se su dung che do fallback ve chu A pixel.");
    }
}

void CPEOPLE::draw(SDL_Renderer* renderer, CFont& font, float cameraY) {
    if (isDead()) {
        SDL_Color deadColor = {214, 40, 40, 255};
        font.drawText(renderer, "X", mX + 2, (int)((float)mY - cameraY), 4, deadColor);
    } else {
        float scaleX = 1.0f;
        float scaleY = 1.0f;
        float offsetY = 0.0f;

        bool isMoving = (mDrawX != mX || mDrawY != mY);
        if (isMoving) {
            float dist = std::sqrt(((float)mX - mDrawX) * ((float)mX - mDrawX) + ((float)mY - mDrawY) * ((float)mY - mDrawY));
            float totalDist = 80.0f;
            float factor = dist / totalDist;
            if (factor > 1.0f) factor = 1.0f;

            float hopFactor = sinf(factor * 3.14159f);
            offsetY = -hopFactor * 16.0f;
            scaleX = 1.0f - hopFactor * 0.12f;
            scaleY = 1.0f + hopFactor * 0.18f;
        } else {
            float breath = sinf(mAnimTimer * 5.0f);
            scaleX = 1.0f + breath * 0.02f;
            scaleY = 1.0f - breath * 0.02f;
        }

        float drawW = mWidth * scaleX;
        float drawH = mHeight * scaleY;
        float drawX = mDrawX + ((float)mWidth - drawW) / 2.0f;
        float drawY = mDrawY + ((float)mHeight - drawH) + offsetY - cameraY;

        if (mCharType == CharacterType::KIRITO) {
            if (mKiritoTexture) {
                SDL_FRect dstRect = { drawX, drawY, drawW, drawH };
                SDL_RenderTexture(renderer, mKiritoTexture, NULL, &dstRect);
            } else {
                SDL_Color kiritoColor = {80, 200, 255, 255};
                font.drawText(renderer, "K", (int)drawX, (int)drawY, 4, kiritoColor);
            }
        } else {
            if (mAsunaTexture) {
                SDL_FRect dstRect = { drawX, drawY, drawW, drawH };
                SDL_RenderTexture(renderer, mAsunaTexture, NULL, &dstRect);
            } else {
                SDL_Color asunaColor = {255, 130, 170, 255};
                font.drawText(renderer, "A", (int)drawX, (int)drawY, 4, asunaColor);
            }
        }
    }
}

bool CPEOPLE::isFinish() const {
    return mY <= 40;
}

bool CPEOPLE::isImpact(CVEHICLE* v) {
    if (!v || isDead()) return false;

    SDL_FRect playerBox = getHitbox();
    SDL_FRect obstacleBox = v->getHitbox();
    
    return (playerBox.x < obstacleBox.x + obstacleBox.w &&
            playerBox.x + playerBox.w > obstacleBox.x &&
            playerBox.y < obstacleBox.y + obstacleBox.h &&
            playerBox.y + playerBox.h > obstacleBox.y);
}

bool CPEOPLE::isImpact(CANIMAL* a) {
    if (!a || isDead()) return false;

    SDL_FRect playerBox = getHitbox();
    SDL_FRect animalBox = a->getHitbox();
    
    return (playerBox.x < animalBox.x + animalBox.w &&
            playerBox.x + playerBox.w > animalBox.x &&
            playerBox.y < animalBox.y + animalBox.h &&
            playerBox.y + animalBox.h > animalBox.y);
}
