#include "CTRAFFICLIGHT.h"
#include <SDL3_image/SDL_image.h>
#include <cmath>

CTRAFFICLIGHT::CTRAFFICLIGHT(int laneY, float redDur, float greenDur)
    : mLaneY(laneY), mIsRed(false), mTimer(0.0f),
      mRedDuration(redDur), mGreenDuration(greenDur) {
    if (laneY != 0) {
        float totalCycle = redDur + greenDur;
        float initialOffset = fmodf((float)std::abs(laneY * 17 + 13), totalCycle);
        if (initialOffset < redDur) {
            mIsRed = true;
            mTimer = initialOffset;
        } else {
            mIsRed = false;
            mTimer = initialOffset - redDur;
        }
    }
}

void CTRAFFICLIGHT::initTextures(SDL_Renderer* renderer) {
    // Shared textures managed by CGAME to avoid texture leaks
    (void)renderer;
}

void CTRAFFICLIGHT::update(float deltaTime) {
    mTimer += deltaTime;
    float currentLimit = mIsRed ? mRedDuration : mGreenDuration;
    if (mTimer >= currentLimit) {
        mTimer -= currentLimit;
        mIsRed = !mIsRed;
    }
}

void CTRAFFICLIGHT::draw(SDL_Renderer* renderer, CFont& font, float cameraY, SDL_Texture* redTex, SDL_Texture* greenTex) {
    float screenY = (float)mLaneY - cameraY;
    // Bỏ qua nếu làn nằm ngoài màn hình
    if (screenY < -80.0f || screenY > 720.0f) return;

    // Vị trí vẽ hai đèn giao thông ở 2 đầu vỉa hè (bên trái X=15, bên phải X=1225)
    float lightXList[] = { 15.0f, 1225.0f };

    for (float lightX : lightXList) {
        if (mIsRed && redTex) {
            SDL_FRect dst = { lightX, screenY + 10.0f, 40.0f, 60.0f };
            SDL_RenderTexture(renderer, redTex, NULL, &dst);
        } else if (!mIsRed && greenTex) {
            SDL_FRect dst = { lightX, screenY + 10.0f, 40.0f, 60.0f };
            SDL_RenderTexture(renderer, greenTex, NULL, &dst);
        } else {
            // --- VẼ ĐÈN BẰNG CODE THUẦN PIXEL ART (Option A Fallback) ---
            // 1. Thân cột kim loại (Cột chữ nhật xám)
            SDL_SetRenderDrawColor(renderer, 30, 35, 45, 230);
            SDL_FRect body = { lightX, screenY + 10.0f, 40.0f, 60.0f };
            SDL_RenderFillRect(renderer, &body);

            // Viền kim loại sáng
            SDL_SetRenderDrawColor(renderer, 80, 200, 255, 200);
            SDL_FRect bTop    = { lightX, screenY + 10.0f, 40.0f, 2.0f };
            SDL_FRect bBottom = { lightX, screenY + 68.0f, 40.0f, 2.0f };
            SDL_FRect bLeft   = { lightX, screenY + 10.0f, 2.0f, 60.0f };
            SDL_FRect bRight  = { lightX + 38.0f, screenY + 10.0f, 2.0f, 60.0f };
            SDL_RenderFillRect(renderer, &bTop);
            SDL_RenderFillRect(renderer, &bBottom);
            SDL_RenderFillRect(renderer, &bLeft);
            SDL_RenderFillRect(renderer, &bRight);

            if (mIsRed) {
                SDL_SetRenderDrawColor(renderer, 255, 40, 40, 255);
                SDL_FRect redBulb = { lightX + 10.0f, screenY + 16.0f, 20.0f, 20.0f };
                SDL_RenderFillRect(renderer, &redBulb);

                SDL_SetRenderDrawColor(renderer, 255, 120, 120, 200);
                SDL_FRect glow = { lightX + 8.0f, screenY + 14.0f, 24.0f, 24.0f };
                SDL_RenderRect(renderer, &glow);

                SDL_SetRenderDrawColor(renderer, 15, 50, 25, 255);
                SDL_FRect greenBulb = { lightX + 10.0f, screenY + 40.0f, 20.0f, 20.0f };
                SDL_RenderFillRect(renderer, &greenBulb);
            } else {
                SDL_SetRenderDrawColor(renderer, 50, 15, 15, 255);
                SDL_FRect redBulb = { lightX + 10.0f, screenY + 16.0f, 20.0f, 20.0f };
                SDL_RenderFillRect(renderer, &redBulb);

                SDL_SetRenderDrawColor(renderer, 40, 255, 90, 255);
                SDL_FRect greenBulb = { lightX + 10.0f, screenY + 40.0f, 20.0f, 20.0f };
                SDL_RenderFillRect(renderer, &greenBulb);

                SDL_SetRenderDrawColor(renderer, 130, 255, 160, 200);
                SDL_FRect glow = { lightX + 8.0f, screenY + 38.0f, 24.0f, 24.0f };
                SDL_RenderRect(renderer, &glow);
            }
        }

        // Biểu tượng cảnh báo [ ! ] nhấp nháy khi đèn sắp đổi màu (còn dưới 1.5 giây)
        float currentLimit = mIsRed ? mRedDuration : mGreenDuration;
        if (currentLimit - mTimer <= 1.5f) {
            Uint64 ticks = SDL_GetTicks();
            if ((ticks / 180) % 2 == 0) {
                SDL_Color warnColor = mIsRed ? SDL_Color{255, 220, 40, 255} : SDL_Color{255, 50, 50, 255};
                font.drawText(renderer, "!", (int)lightX + 16, (int)screenY - 12, 2, warnColor);
            }
        }
    }
}
