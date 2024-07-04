#include "game.h"
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <ncurses.h>
#include <getopt.h>

namespace fs = std::filesystem;

/**
 * @brief Imprime a mensagem de uso do programa.
 *
 * Esta função exibe a mensagem de uso do programa, especificando os argumentos 
 * que podem ser passados na linha de comando.
 */
void print_usage() {
    std::cout << "Usage: snaze-game [--life N] [--food N]" << std::endl;
}

/**
 * @brief Imprime a mensagem de boas-vindas do jogo.
 *
 * Esta função exibe a mensagem de boas-vindas do jogo, incluindo as opções de 
 * simulação do jogo e as instruções para iniciar o jogo.
 */
void print_welcome_message() {
    std::cout << "---> Welcome to the classic Snake Game <---\n"
              << "Game simulation options:\n"
              << "--lives <num> Number of lives the snake shall have. Default = 5.\n"
              << "--food <num> Number of food pellets for the entire simulation. Default = 10.\n"
              << "--------------------------------------------------\n"
              << ">>> Press <ENTER> to start the game!\n";
}


/**
 * @brief Função principal que inicia o jogo Snake.
 *
 * Esta função configura o ambiente do jogo, processa os argumentos da linha de comando,
 * inicializa os níveis e controla o loop principal do jogo.
 *
 * @param argc Número de argumentos da linha de comando.
 * @param argv Lista de argumentos da linha de comando.
 * @return Código de status de execução (0 para sucesso, 1 para erro).
 */
int main(int argc, char** argv) {
    srand(time(0));
    std::string assetsDir = "../assets";
    // Verifica se o diretório "assets" existe
    // Se não existe, exibe uma mensagem de erro indicando que o diretório não foi encontrado
    // Retorna 1 para indicar um erro durante a execução do programa
    if (!fs::exists(assetsDir)) {
        std::cerr << "Error: Directory 'assets' does not exist!" << std::endl;
        return 1;
    }

    int initialLives = 5; 
    int foodRequired = 10; 

    // Configuração das opções de linha de comando usando a estrutura option
    static struct option long_options[] = {
        {"life", required_argument, 0, 'l'},
        {"food", required_argument, 0, 'f'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    // Loop para processar as opções de linha de comando usando getopt_long
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
    // Itera sobre os arquivos no diretório especificado por assetsDir e adiciona seus caminhos ao vetor levels
    for (const auto& entry : fs::directory_iterator(assetsDir)) {
        levels.push_back(entry.path().string());
    }

    // Verifica se o vetor levels está vazio, indicando que nenhum arquivo de nível foi encontrado no diretório 'assets'
    if (levels.empty()) {
        std::cerr << "Error: No level files found in 'assets' directory!" << std::endl;
        return 1;
    }

    // Ordena os caminhos dos arquivos de nível armazenados no vetor levels em ordem lexicográfica
    std::sort(levels.begin(), levels.end());

    initscr();
    noecho();
    cbreak();
    timeout(0); // Configura o getch() para não esperar por input

    GameState state;
    const int delay = 200000; // 200ms
    bool allLevelsCompleted = true; // Flag para verificar se todos os níveis foram completados

    // Itera sobre cada nível carregado e inicia o jogo
    for (const auto& level : levels) {
        state = initializeGame(level);
        if (state.gameOver) {
            continue;
        }
        state.lives = initialLives;
        state.foodCounter = 0; 

        while (!state.gameOver) {
            renderGame(state, foodRequired); 
            usleep(delay); 
            state.direction = getNextDirection(state);
            bool foodConsumed = updateGame(state);

            if (foodConsumed && state.foodCounter >= foodRequired) {
                mvprintw(state.height + 2, 0, "Food consumed! Moving to next level...");
                refresh();
                usleep(1000000); 
                break; 
            }
        }

        if (state.lives <= 0) {
            mvprintw(state.height + 2, 0, "Game Over at level: %s", level.c_str());
            refresh();
            usleep(2000000); 
            allLevelsCompleted = false; 
            break;
        }
    }

    // Verifica se todos os níveis foram completados e se ainda há vidas restantes
    if (allLevelsCompleted && state.lives > 0) {
        mvprintw(state.height + 2, 0, "CONGRATULATIONS anaconda WON!\nThanks for playing!");
        refresh();
        usleep(2000000); 
    }

    endwin();

    return 0;
}