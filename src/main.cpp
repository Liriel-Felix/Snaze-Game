#include "game.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>  // Para std::sort
#include <unistd.h>   // Para usleep

namespace fs = std::filesystem;

int main() {
    srand(time(0));
    std::string assetsDir = "../assets";  // Ajuste o caminho para subir um nível na hierarquia de diretórios
    std::cout << "Current path is: " << fs::current_path() << std::endl;
    std::cout << "Assets directory path is: " << fs::absolute(assetsDir) << std::endl;

    if (!fs::exists(assetsDir)) {
        std::cerr << "Error: Directory 'assets' does not exist!" << std::endl;
        return 1;
    }

    std::vector<std::string> levels;
    for (const auto& entry : fs::directory_iterator(assetsDir)) {
        levels.push_back(entry.path().string());
    }

    if (levels.empty()) {
        std::cerr << "Error: No level files found in 'assets' directory!" << std::endl;
        return 1;
    }

    std::sort(levels.begin(), levels.end());

    GameState state;
    for (const auto& level : levels) {
        state = initializeGame(level);

        while (!state.gameOver) {
            renderGame(state);
            usleep(200000); // Pause for 200ms
            state.direction = getNextDirection(state);
            bool foodConsumed = updateGame(state);

            if (foodConsumed && state.foodCounter >= 5) {
                std::cout << "Food consumed! Moving to next level..." << std::endl;
                usleep(1000000); // Pause for 1 second before moving to the next level
                break; // Passa para o próximo nível
            }
        }

        if (state.lives <= 0) {
            std::cout << "Game Over at level: " << level << std::endl;
            break;
        }
    }

    if (state.lives > 0) {
        std::cout << "All levels completed!" << std::endl;
    }

    return 0;
}