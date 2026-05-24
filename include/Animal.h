#ifndef ANIMAL_H
#define ANIMAL_H

#include <SDL3/SDL.h>
#include "Font.h"

// ====================================================================
// LỚP CANIMAL (Trừu tượng - Abstract Base Class)
// Quản lý các thuộc tính chung của sinh vật/động vật di chuyển (Khủng long, Chim)
// ====================================================================
class CANIMAL {
protected:
    int mX, mY;         // Vị trí trên màn hình
    int mWidth, mHeight;// Kích thước hộp biên va chạm
    int mSpeed;         // Tốc độ di chuyển (pixel/s)
    int mDirection;     // Hướng di chuyển (1: Trái -> Phải, -1: Phải -> Trái)
    SDL_Texture* mTexture1; // Texture frame 1 (PNG)
    SDL_Texture* mTexture2; // Texture frame 2 (PNG)

public:
    CANIMAL(int x, int y, int speed, int direction);
    virtual ~CANIMAL();

    // Hàm thuần ảo bắt buộc lớp con tự định nghĩa logic di chuyển
    virtual void Move(int limitX1, int limitX2) = 0;

    // Phát ra tiếng kêu đặc trưng của loài vật qua SDL3_mixer
    virtual void Tell() = 0;

    // Vẽ động vật với các hiệu ứng animation chuyển động
    virtual void draw(SDL_Renderer* renderer, CFont& font) = 0;

    // Getters phục vụ va chạm
    int getX() const { return mX; }
    int getY() const { return mY; }
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }
    SDL_Texture* getTexture() const { return mTexture1; }

    // ═════ HITBOX (Tách biệt visual và collision) ═════
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
// LỚP CDINAUSOR (Khủng long bạo chúa T-Rex)
// Di chuyển dưới làn đường đi bộ, bước chạy chân luân phiên nhịp nhàng
// ====================================================================
class CDINAUSOR : public CANIMAL {
public:
    CDINAUSOR(int x, int y, int speed, int direction);
    virtual ~CDINAUSOR();

    void Move(int limitX1, int limitX2) override;
    void Tell() override;
    void draw(SDL_Renderer* renderer, CFont& font) override;
};

// ====================================================================
// LỚP CBIRD (Chim đại bàng bay lượn)
// Di chuyển tốc độ cao, có hiệu ứng vỗ cánh bay lên xuống nhịp nhàng
// ====================================================================
class CBIRD : public CANIMAL {
public:
    CBIRD(int x, int y, int speed, int direction);
    virtual ~CBIRD();

    void Move(int limitX1, int limitX2) override;
    void Tell() override;
    void draw(SDL_Renderer* renderer, CFont& font) override;
};

#endif // ANIMAL_H
