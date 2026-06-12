#include "CGAME.h"
#include <iostream>

int main(int argc, char* argv[]) {
    CGAME game;

    // Khởi tạo game với tiêu đề "Crossing Game" và kích thước HD 1280x720
    if (!game.init("Crossing Game", 1280, 720)) {
        std::cerr << "Failed to initialize game!" << std::endl;
        return -1;
    }

    // Chạy vòng lặp game chính
    game.run();

    return 0;
}
