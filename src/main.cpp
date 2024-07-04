#include "game.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <ncurses.h>
#include <getopt.h>

namespace fs = std::filesystem;

void print_usage() {
    std::cout << "Usage: snaze-game [--life N] [--food N]" << std::endl;
}

void print_welcome_message() {
    std::cout << "---> Welcome to the classic Snake Game <---\n"
              << "Game simulation options:\n"
              << "--lives <num> Number of lives the snake shall have. Default = 5.\n"
              << "--food <num> Number of food pellets for the entire simulation. Default = 10.\n"
              << "--------------------------------------------------\n"
              << ">>> Press <ENTER> to start the game!\n";
}

int main(int argc, char** argv) {
    srand(time(0));
    std::string assetsDir = "../assets";
    if (!fs::exists(assetsDir)) {
        std::cerr << "Error: Directory 'assets' does not exist!" << std::endl;
        return 1;
    }

    int initialLives = 5; // Número inicial de vidas padrão
    int foodRequired = 10; // Número inicial de comidas necessárias padrão

    static struct option long_options[] = {
        {"life", required_argument, 0, 'l'},
        {"food", required_argument, 0, 'f'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "l:f:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'l':
                initialLives = std::stoi(optarg);
                break;
            case 'f':
                foodRequired = std::stoi(optarg);
                break;
            default:
                print_usage();
                return 1;
        }
    }

    print_welcome_message();
    std::cin.get();

    std::vector<std::string> levels;
    for (const auto& entry : fs::directory_iterator(assetsDir)) {
        levels.push_back(entry.path().string());
    }

    if (levels.empty()) {
        std::cerr << "Error: No level files found in 'assets' directory!" << std::endl;
        return 1;
    }

    std::sort(levels.begin(), levels.end());

    initscr();
    noecho();
    cbreak();
    timeout(0); // Configura o getch() para não esperar por input

    GameState state;
    const int delay = 200000; // 200ms
    bool allLevelsCompleted = true; // Flag para verificar se todos os níveis foram completados

    for (const auto& level : levels) {
        state = initializeGame(level);
        if (state.gameOver) {
            continue; // Pula para o próximo nível se o atual contém caracteres inválidos
        }
        state.lives = initialLives;
        state.foodCounter = 0; // Resetar contador de comida para cada nível

        while (!state.gameOver) {
            renderGame(state, foodRequired); // Passa foodRequired para renderGame
            usleep(delay); // Pausa por 200ms
            state.direction = getNextDirection(state);
            bool foodConsumed = updateGame(state);

            if (foodConsumed && state.foodCounter >= foodRequired) {
                mvprintw(state.height + 2, 0, "Food consumed! Moving to next level...");
                refresh();
                usleep(1000000); // Pausa por 1 segundo antes de mover para o próximo nível
                break; // Passa para o próximo nível
            }
        }

        if (state.lives <= 0) {
            mvprintw(state.height + 2, 0, "Game Over at level: %s", level.c_str());
            refresh();
            usleep(2000000); // Pausa por 2 segundos antes de sair
            allLevelsCompleted = false; // Se o jogador perder todas as vidas, não completou todos os níveis
            break;
        }
    }

    if (allLevelsCompleted && state.lives > 0) {
        mvprintw(state.height + 2, 0, "CONGRATULATIONS anaconda WON!\nThanks for playing!");
        refresh();
        usleep(2000000); // Pausa por 2 segundos antes de sair
    }

    endwin();

    return 0;
}