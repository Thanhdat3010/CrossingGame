#ifndef VEHICLE_H
#define VEHICLE_H

#include <SDL3/SDL.h>
#include "Font.h"

// ====================================================================
// LỚP CVEHICLE (Trừu tượng - Abstract Base Class)
// Quản lý các thuộc tính chung của phương tiện giao thông (xe cộ/quái vật)
// ====================================================================
class CVEHICLE {
protected:
    int mX, mY;         // Vị trí (x, y) trên màn hình
    int mWidth, mHeight;// Kích thước hộp biên va chạm
    int mSpeed;         // Tốc độ di chuyển cơ sở (pixel/s)
    int mDirection;     // Hướng di chuyển (1: Trái sang Phải, -1: Phải sang Trái)
    SDL_Texture* mTexture1; // Texture frame 1 (PNG)
    SDL_Texture* mTexture2; // Texture frame 2 (PNG)

public:
    CVEHICLE(int x, int y, int speed, int direction);
    virtual ~CVEHICLE();

    // Hàm thuần ảo bắt buộc các lớp con (CTRUCK, CCAR) phải tự định nghĩa logic di chuyển
    virtual void Move(int limitX1, int limitX2) = 0;

    // Hàm thuần ảo vẽ phương tiện lên renderer
    virtual void draw(SDL_Renderer* renderer, CFont& font, float cameraY) = 0;

    // Getters phục vụ logic va chạm và vẽ
    int getX() const { return mX; }
    int getY() const { return mY; }
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }
    int getSpeed() const { return mSpeed; }
    int getDirection() const { return mDirection; }
    SDL_Texture* getTexture() const { return mTexture1; }

    // ═════ HITBOX (Tách biệt visual và collision) ═════
    // Obstacle hitbox co 15% mỗi bên → chỉ phần body cứng mới gây va chạm
    SDL_FRect getHitbox() const {
        const float PAD_X = mWidth  * 0.15f;
        const float PAD_Y = mHeight * 0.15f;
        return { (float)mX + PAD_X, (float)mY + PAD_Y,
                 (float)mWidth - 2 * PAD_X, (float)mHeight - 2 * PAD_Y };
    }

    // Setters
    void setX(int x) { mX = x; }
    void setY(int y) { mY = y; }
    void setSpeed(int speed) { mSpeed = speed; }
    void setTextures(SDL_Texture* tex1, SDL_Texture* tex2) { 
        mTexture1 = tex1; 
        mTexture2 = tex2; 
        if (mTexture1) {
            float texW = 0, texH = 0;
            SDL_GetTextureSize(mTexture1, &texW, &texH);
            if (texW > 0 && texH > 0) {
                mWidth = (int)((float)mHeight * (texW / texH));
            }
        }
    }
};


// ====================================================================
// LỚP CTRUCK (Xe tải / Quái vật khổng lồ)
// Kích thước to hơn, di chuyển đầm và chậm hơn
// ====================================================================
class CTRUCK : public CVEHICLE {
public:
    CTRUCK(int x, int y, int speed, int direction);
    virtual ~CTRUCK();

    void Move(int limitX1, int limitX2) override;
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY) override;
};

// ====================================================================
// LỚP CCAR (Xe con / Quái vật tốc độ)
// Kích thước nhỏ gọn, tốc độ di chuyển nhanh và lướt hơn
// ====================================================================
class CCAR : public CVEHICLE {
public:
    CCAR(int x, int y, int speed, int direction);
    virtual ~CCAR();

    void Move(int limitX1, int limitX2) override;
    void draw(SDL_Renderer* renderer, CFont& font, float cameraY) override;
};

#endif // VEHICLE_H
