#include "CTRAFFICLIGHT.h"
#include <SDL3_image/SDL_image.h>
#include <cmath>

CTRAFFICLIGHT::CTRAFFICLIGHT(int laneY, float redDur, float greenDur)
    : mLaneY(laneY), mIsRed(false), mTimer(0.0f),
      mRedDuration(redDur), mGreenDuration(greenDur),
      mRedTexture(nullptr), mGreenTexture(nullptr) {}

void CTRAFFICLIGHT::initTextures(SDL_Renderer* renderer) {
    if (!renderer) return;
    mRedTexture = IMG_LoadTexture(renderer, "assets/images/environment/traffic_light_red.png");
    mGreenTexture = IMG_LoadTexture(renderer, "assets/images/environment/traffic_light_green.png");
}

void CTRAFFICLIGHT::update(float deltaTime) {
    mTimer += deltaTime;
    float currentLimit = mIsRed ? mRedDuration : mGreenDuration;
    if (mTimer >= currentLimit) {
        mTimer -= currentLimit;
        mIsRed = !mIsRed;
    }
}

void CTRAFFICLIGHT::draw(SDL_Renderer* renderer, CFont& font, float cameraY) {
    float screenY = (float)mLaneY - cameraY;
    // Bỏ qua nếu làn nằm ngoài màn hình
    if (screenY < -80.0f || screenY > 720.0f) return;

    // Vị trí vẽ hai đèn giao thông ở 2 đầu vỉa hè (bên trái X=15, bên phải X=1225)
    float lightXList[] = { 15.0f, 1225.0f };

    for (float lightX : lightXList) {
        if (mIsRed && mRedTexture) {
            SDL_FRect dst = { lightX, screenY + 10.0f, 40.0f, 60.0f };
            SDL_RenderTexture(renderer, mRedTexture, NULL, &dst);
            continue;
        }
        if (!mIsRed && mGreenTexture) {
            SDL_FRect dst = { lightX, screenY + 10.0f, 40.0f, 60.0f };
            SDL_RenderTexture(renderer, mGreenTexture, NULL, &dst);
            continue;
        }

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

        // 2. Đèn Đỏ (Hình vuông/tròn phía trên)
        if (mIsRed) {
            // Đèn Đỏ SÁNG NEON (Glow effect)
            SDL_SetRenderDrawColor(renderer, 255, 40, 40, 255);
            SDL_FRect redBulb = { lightX + 10.0f, screenY + 16.0f, 20.0f, 20.0f };
            SDL_RenderFillRect(renderer, &redBulb);

            // Viền sáng tỏa nhiệt xung quanh
            SDL_SetRenderDrawColor(renderer, 255, 120, 120, 200);
            SDL_FRect glow = { lightX + 8.0f, screenY + 14.0f, 24.0f, 24.0f };
            SDL_RenderRect(renderer, &glow);

            // Đèn Xanh TỐI (Tắt)
            SDL_SetRenderDrawColor(renderer, 15, 50, 25, 255);
            SDL_FRect greenBulb = { lightX + 10.0f, screenY + 40.0f, 20.0f, 20.0f };
            SDL_RenderFillRect(renderer, &greenBulb);
        } else {
            // Đèn Đỏ TỐI (Tắt)
            SDL_SetRenderDrawColor(renderer, 50, 15, 15, 255);
            SDL_FRect redBulb = { lightX + 10.0f, screenY + 16.0f, 20.0f, 20.0f };
            SDL_RenderFillRect(renderer, &redBulb);

            // Đèn Xanh SÁNG NEON (Glow effect)
            SDL_SetRenderDrawColor(renderer, 40, 255, 90, 255);
            SDL_FRect greenBulb = { lightX + 10.0f, screenY + 40.0f, 20.0f, 20.0f };
            SDL_RenderFillRect(renderer, &greenBulb);

            // Viền sáng tỏa nhiệt xung quanh
            SDL_SetRenderDrawColor(renderer, 130, 255, 160, 200);
            SDL_FRect glow = { lightX + 8.0f, screenY + 38.0f, 24.0f, 24.0f };
            SDL_RenderRect(renderer, &glow);
        }
    }
}
