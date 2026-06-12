#ifndef CFONT_H
#define CFONT_H

#include <SDL3/SDL.h>
#include <string>

class CFont {
private:
    SDL_Texture* mFontTexture;
    int mCharWidth;
    int mCharHeight;

    void createFontTexture(SDL_Renderer* renderer);

public:
    CFont();
    ~CFont();

    bool init(SDL_Renderer* renderer);
    void free();
    void drawText(SDL_Renderer* renderer, const std::string& text, int x, int y, int scale, SDL_Color color);
    void drawTextCentered(SDL_Renderer* renderer, const std::string& text, int y, int scale, SDL_Color color);
};

#endif // CFONT_H
