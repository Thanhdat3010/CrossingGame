#include "People.h"
#include "Vehicle.h"
#include "Animal.h"

#include <SDL3_image/SDL_image.h>

CPEOPLE::CPEOPLE() {
    mCharType = CharacterType::KIRITO; // Mặc định là Kirito
    mKiritoTexture = nullptr;
    mAsunaTexture = nullptr;
    mState = true; // Ban đầu còn sống
    setCharacter(CharacterType::KIRITO); // Cài đặt chỉ số tương ứng
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

void CPEOPLE::resetPosition() {
    // Xuất phát ở chính giữa bên dưới màn hình (1280 x 720)
    // Trừ đi nửa độ rộng nhân vật (32/2 = 16) để căn đúng tâm
    mX = 640 - (mWidth / 2);
    mY = 600; // Đặt ở mức Y=600 để mỗi lần bấm Up (mSpeed=80) sẽ lên Y=520, Y=440... vừa khít các làn đường!
    mState = true;
}

void CPEOPLE::Up(int limitY) {
    if (isDead()) return;
    mY -= mSpeed;
    if (mY < limitY) {
        mY = limitY; // Giới hạn biên trên
    }
}

void CPEOPLE::Down(int limitY) {
    if (isDead()) return;
    mY += mSpeed;
    if (mY > limitY) {
        mY = limitY; // Giới hạn biên dưới
    }
}

void CPEOPLE::Left(int limitX) {
    if (isDead()) return;
    mX -= mSpeed;
    if (mX < limitX) {
        mX = limitX; // Giới hạn biên trái
    }
}

void CPEOPLE::Right(int limitX) {
    if (isDead()) return;
    mX += mSpeed;
    if (mX > limitX) {
        mX = limitX; // Giới hạn biên phải
    }
}

void CPEOPLE::setCharacter(CharacterType type) {
    mCharType = type;
    if (mCharType == CharacterType::KIRITO) {
        mSpeed = 80;     // Grid step: 80px/bước = đúng 1 làn đường!
        mWidth = 80;     // Nhân vật vừa khít 1 lane (80×80)
        mHeight = 80;
    } else {
        mSpeed = 80;     // Grid step: 80px/bước = đúng 1 làn đường!
        mWidth = 80;     // Nhân vật vừa khít 1 lane (80×80)
        mHeight = 80;
    }
}



void CPEOPLE::loadTextures(SDL_Renderer* renderer) {
    if (!renderer) return;

    // Tải ảnh PNG cho Kirito & Asuna nếu người dùng cung cấp
    mKiritoTexture = IMG_LoadTexture(renderer, "assets/kirito.png");
    if (!mKiritoTexture) {
        SDL_Log("Chua co assets/kirito.png, game se su dung che do fallback ve chu K pixel.");
    }

    mAsunaTexture = IMG_LoadTexture(renderer, "assets/asuna.png");
    if (!mAsunaTexture) {
        SDL_Log("Chua co assets/asuna.png, game se su dung che do fallback ve chu A pixel.");
    }
}

void CPEOPLE::draw(SDL_Renderer* renderer, CFont& font) {
    if (isDead()) {
        // Vẽ chữ X đỏ rực rỡ báo tử
        SDL_Color deadColor = {214, 40, 40, 255};
        font.drawText(renderer, "X", mX + 2, mY, 4, deadColor);
    } else {
        // Vẽ ô lưới Neon (SAO Hologram Grid Box) bao trọn nhân vật để hiển thị vị trí chuẩn xác
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 80, 200, 255, 120); // Cyan trong suốt
        SDL_FRect gridBox = { (float)mX, (float)mY, (float)mWidth, (float)mHeight };
        SDL_RenderRect(renderer, &gridBox);

        // Vẽ Kirito hoặc Asuna
        if (mCharType == CharacterType::KIRITO) {
            if (mKiritoTexture) {
                SDL_FRect dstRect = { (float)mX, (float)mY, (float)mWidth, (float)mHeight };
                SDL_RenderTexture(renderer, mKiritoTexture, NULL, &dstRect);
            } else {
                // Chế độ dự phòng: Chữ 'K' màu xanh dương lấp lánh (Elucidator)
                SDL_Color kiritoColor = {80, 200, 255, 255};
                font.drawText(renderer, "K", mX, mY, 4, kiritoColor);
            }
        } else {
            // Asuna
            if (mAsunaTexture) {
                SDL_FRect dstRect = { (float)mX, (float)mY, (float)mWidth, (float)mHeight };
                SDL_RenderTexture(renderer, mAsunaTexture, NULL, &dstRect);
            } else {
                // Chế độ dự phòng: Chữ 'A' màu đỏ hồng dễ thương (Lambent Light)
                SDL_Color asunaColor = {255, 130, 170, 255};
                font.drawText(renderer, "A", mX, mY, 4, asunaColor);
            }
        }
    }
}

bool CPEOPLE::isFinish() const {
    // Vỉa hè về đích nằm ở biên trên (y <= 120px — top sidewalk)
    return mY <= 120;
}

bool CPEOPLE::isImpact(CVEHICLE* v) {
    if (!v || isDead()) return false;

    // ═══════════════════════════════════════════════════════════════
    // THỦ CÔNG: AABB getHitbox() không dùng thư viện theo yêu cầu đồ án
    // ═══════════════════════════════════════════════════════════════
    SDL_FRect playerBox = getHitbox();
    SDL_FRect obstacleBox = v->getHitbox();
    
    return (playerBox.x < obstacleBox.x + obstacleBox.w &&
            playerBox.x + playerBox.w > obstacleBox.x &&
            playerBox.y < obstacleBox.y + obstacleBox.h &&
            playerBox.y + playerBox.h > obstacleBox.y);
}

bool CPEOPLE::isImpact(CANIMAL* a) {
    if (!a || isDead()) return false;

    // ═══════════════════════════════════════════════════════════════
    // THỦ CÔNG: AABB getHitbox() không dùng thư viện theo yêu cầu đồ án
    // ═══════════════════════════════════════════════════════════════
    SDL_FRect playerBox = getHitbox();
    SDL_FRect animalBox = a->getHitbox();
    
    return (playerBox.x < animalBox.x + animalBox.w &&
            playerBox.x + playerBox.w > animalBox.x &&
            playerBox.y < animalBox.y + animalBox.h &&
            playerBox.y + animalBox.h > animalBox.y);
}


