#ifndef PEOPLE_H
#define PEOPLE_H

#include <SDL3/SDL.h>
#include "Font.h"

class CPEOPLE {
private:
    int mX, mY;      // Tọa độ của người chơi trên màn hình
    int mWidth;      // Độ rộng nhân vật (để tính va chạm)
    int mHeight;     // Độ cao nhân vật
    bool mState;     // Trạng thái: true (Sống), false (Chết)
    int mSpeed;      // Tốc độ di chuyển mỗi bước (pixel)

public:
    CPEOPLE();
    ~CPEOPLE();

    // Reset lại tọa độ người chơi về điểm xuất phát mặc định
    void resetPosition();

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

    // Getters phục vụ kiểm tra va chạm
    int getX() const { return mX; }
    int getY() const { return mY; }
    int getWidth() const { return mWidth; }
    int getHeight() const { return mHeight; }
};

#endif // PEOPLE_H
