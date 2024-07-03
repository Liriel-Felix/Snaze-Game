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
    std::cout << "Usage: t1 [--life N] [--food N]" << std::endl;
}

int main(int argc, char** argv) {
    srand(time(0));
    std::string assetsDir = "../assets";
    if (!fs::exists(assetsDir)) {
        std::cerr << "Error: Directory 'assets' does not exist!" << std::endl;
        return 1;
    }

    int initialLives = 5; // Número inicial de vidas padrão
    int foodRequired = 5; // Número inicial de comidas necessárias padrão

    // Estrutura para definir os argumentos nomeados
    static struct option long_options[] = {
        {"life", required_argument, 0, 'l'},
        {"food", required_argument, 0, 'f'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    // Parsing dos argumentos
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

    std::vector<std::string> levels;
    for (const auto& entry : fs::directory_iterator(assetsDir)) {
        levels.push_back(entry.path().string());
    }

    if (levels.empty()) {
        std::cerr << "Error: No level files found in 'assets' directory!" << std::endl;
        return 1;
    }

    std::sort(levels.begin(), levels.end());

    // Inicializa o ncurses
    initscr();
    noecho();
    cbreak();
    timeout(0); // Configura o getch() para não esperar por input

    GameState state;
    const int delay = 200000; // 200ms

    for (const auto& level : levels) {
        state = initializeGame(level);
        state.lives = initialLives;

        while (!state.gameOver) {
            renderGame(state);
            usleep(delay); // Pausa por 200ms
            state.direction = getNextDirection(state);
            bool foodConsumed = updateGame(state);

            if (foodConsumed && state.foodCounter >= foodRequired) {
                mvprintw(state.height + 2, 0, "Food consumed! Moving to next level...");
                // Espera pela tecla Enter
                int ch;
                while ((ch = getch()) != '\n' && ch != '\r') {
                   refresh();
                }
                usleep(1000000); // Pausa por 1 segundo antes de mover para o próximo nível
                break; // Passa para o próximo nível
            }
        }

        if (state.lives <= 0) {
            mvprintw(state.height + 2, 0, "Game Over at level: %s", level.c_str());
            refresh();
            usleep(2000000); // Pausa por 2 segundos antes de sair
            break;
        }
    }

    if (state.lives > 0) {
        mvprintw(state.height + 2, 0, "All levels completed!");
        refresh();
        usleep(2000000); // Pausa por 2 segundos antes de sair
    }

    // Finaliza o ncurses
    endwin();

    return 0;
}