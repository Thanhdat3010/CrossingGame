#ifndef FONT_H
#define FONT_H

#include <SDL3/SDL.h>
#include <string>

class CFont {
private:
    SDL_Texture* mFontTexture;
    int mCharWidth;
    int mCharHeight;

    // Tạo Texture font chữ pixel từ mảng byte nhị phân được định nghĩa sẵn
    void createFontTexture(SDL_Renderer* renderer);

public:
    CFont();
    ~CFont();

    // Khởi tạo Font và tạo Texture
    bool init(SDL_Renderer* renderer);

    // Giải phóng tài nguyên Font
    void free();

    // Hàm vẽ chuỗi ký tự lên màn hình
    void drawText(SDL_Renderer* renderer, const std::string& text, int x, int y, int scale, SDL_Color color);

    // Hàm tự động căn giữa chuỗi ký tự theo chiều ngang màn hình 1280px
    void drawTextCentered(SDL_Renderer* renderer, const std::string& text, int y, int scale, SDL_Color color);
};

#endif // FONT_H
