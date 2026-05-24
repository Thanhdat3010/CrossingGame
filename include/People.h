#ifndef PEOPLE_H
#define PEOPLE_H

#include <SDL3/SDL.h>
#include "Font.h"

class CVEHICLE;
class CANIMAL;

class CPEOPLE {
public:
    // Enum định nghĩa các loại nhân vật
    enum class CharacterType { KIRITO, ASUNA };

private:
    int mX, mY;      // Tọa độ của người chơi trên màn hình
    int mWidth;      // Độ rộng nhân vật (để tính va chạm)
    int mHeight;     // Độ cao nhân vật
    bool mState;     // Trạng thái: true (Sống), false (Chết)
    int mSpeed;      // Tốc độ di chuyển mỗi bước (pixel)

    CharacterType mCharType;         // Nhân vật đang chọn
    SDL_Texture* mKiritoTexture;     // Texture ảnh của Kirito (PNG)
    SDL_Texture* mAsunaTexture;      // Texture ảnh của Asuna (PNG)

public:
    CPEOPLE();
    ~CPEOPLE();

    // Reset lại tọa độ người chơi về điểm xuất phát mặc định
    void resetPosition();

    // Chọn nhân vật và tải các chỉ số (Speed, Hitbox) tương ứng
    void setCharacter(CharacterType type);

    // Tải trước ảnh PNG của các nhân vật (Kirito, Asuna)
    void loadTextures(SDL_Renderer* renderer);

    // Kiểm tra va chạm hộp biên (AABB) với xe/thú
    bool isImpact(CVEHICLE* v);
    bool isImpact(CANIMAL* a);

    // Các hàm di chuyển (W/A/S/D) - nhận vào biên giới hạn màn hình
    void Up(int limitY);
    void Down(int limitY);
    void Left(int limitX);
    void Right(int limitX);

    // Vẽ nhân vật lên màn hình (sử dụng ký tự 'Y' pixel-art chuẩn đề bài)
    void draw(SDL_Renderer* renderer, CFont& font);

    // Kiểm tra trạng thái
    bool isDead() const { return !mState; }
    bool isFinish() const; // Kiểm tra đã băng qua đường thành công chưa
    void setDead(bool dead) { mState = !dead; }

    // Getters phục vụ kiểm tra va chạm và vẽ
    int getX() const { return mX; }
    int getY() const { return mY; }
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }

    // ═════ HITBOX CHUẨN NGÀNH (Lazy Foo' / Frogger Best Practice) ═════
    // Hitbox nhỏ hơn sprite, chỉ bao phủ phần "body" thực tế của nhân vật.
    // Player sprite 80×80 → hitbox ~48×48, căn giữa trong sprite.
    // Tách biệt visual (draw) và collision (hitbox) = chuẩn mọi game 2D.
    SDL_FRect getHitbox() const {
        const float PAD_X = mWidth  * 0.20f;  // Co 20% mỗi bên ngang
        const float PAD_Y = mHeight * 0.20f;  // Co 20% mỗi bên dọc
        return { (float)mX + PAD_X, (float)mY + PAD_Y,
                 (float)mWidth - 2 * PAD_X, (float)mHeight - 2 * PAD_Y };
    }

    SDL_Texture* getKiritoTexture() const { return mKiritoTexture; }
    SDL_Texture* getAsunaTexture() const { return mAsunaTexture; }
};


#endif // PEOPLE_H
