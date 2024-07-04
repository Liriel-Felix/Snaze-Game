#include "game.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <filesystem>
#include <algorithm>
#include <ncurses.h>

namespace fs = std::filesystem;

Position getNextPosition(Position current, Direction direction) {
    switch (direction) {
        case UP: current.y--; break;
        case DOWN: current.y++; break;
        case LEFT: current.x--; break;
        case RIGHT: current.x++; break;
    }
    return current;
}

std::vector<Direction> getValidDirections(Position current, const GameState& state) {
    std::vector<Direction> directions;
    std::vector<Direction> allDirections = {UP, DOWN, LEFT, RIGHT};
    for (auto dir : allDirections) {
        Position next = getNextPosition(current, dir);
        if (next.x >= 0 && next.x < state.width && next.y >= 0 && next.y < state.height && state.grid[next.y][next.x] != '#') {
            bool isSnake = false;
            for (const auto& pos : state.snake) {
                if (pos.x == next.x && pos.y == next.y) {
                    isSnake = true;
                    break;
                }
            }
            if (!isSnake) {
                directions.push_back(dir);
            }
        }
    }
    return directions;
}

GameState initializeGame(const std::string& filename) {
    GameState state;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        exit(1);
    }

    file >> state.height >> state.width;
    state.grid.resize(state.height);

    std::string line;
    std::getline(file, line); // Pula o restante da primeira linha

    for (int i = 0; i < state.height; ++i) {
    std::getline(file, state.grid[i]);
    for (int j = 0; j < state.width; ++j) {
        char cell = state.grid[i][j];
        if (cell == '&') {
            state.spawn = {j, i};
            state.snake = {state.spawn}; // Inicializa a cobra na posição de spawn
            state.grid[i][j] = ' ';
        } else if (cell == '$' || std::isdigit(cell)) {
            // Se um célula contém '$' ou um dígito, ignora esse nível
            std::cerr << "Warning: Level " << filename << " contains invalid character '" << cell << "' and will be skipped." << std::endl;
            state.gameOver = true; // Marca gameOver para ignorar esse nível
            return state;
        }
    }
}

    // Gera a comida em uma posição aleatória
    do {
        state.food.x = rand() % state.width;
        state.food.y = rand() % state.height;
    } while (state.grid[state.food.y][state.food.x] != ' ');

    state.direction = RIGHT;
    state.gameOver = false;
    state.lives = 5; // Número inicial de vidas padrão, pode ser ajustado no main
    state.foodCounter = 0; // Número inicial de comidas consumidas
    return state;
}

// Atualize a assinatura da função para aceitar `foodRequired`
void renderGame(const GameState& state, int foodRequired) {
    clear(); // Limpa a tela antes de desenhar

    // Desenha o cabeçalho
    mvprintw(0, 0, "Lives: %d | Score: %d | Food eaten: %d of %d", state.lives, state.foodCounter * 10, state.foodCounter, foodRequired);

    // Desenha o jogo
    for (int y = 0; y < state.height; y++) {
        for (int x = 0; x < state.width; x++) {
            if (x == state.food.x && y == state.food.y) {
                mvprintw(y + 1, x, "F");
            } else if (state.grid[y][x] == '#') {
                mvprintw(y + 1, x, "#");
            } else {
                bool isSnake = false;
                for (const auto& pos : state.snake) {
                    if (pos.x == x && pos.y == y) {
                        mvprintw(y + 1, x, "O");
                        isSnake = true;
                        break;
                    }
                }
                if (!isSnake) {
                    mvprintw(y + 1, x, " ");
                }
            }
        }
    }

    refresh(); // Atualiza a tela
}

bool updateGame(GameState& state) {
    Position next = getNextPosition(state.snake.front(), state.direction);

    if (next.x < 0 || next.x >= state.width || next.y < 0 || next.y >= state.height || state.grid[next.y][next.x] == '#') {
        state.lives--;
        if (state.lives <= 0) {
            state.gameOver = true;
        } else {
            state.snake = {state.spawn}; // Reseta a posição da cobra para o ponto inicial
            state.direction = RIGHT; // Direção inicial
        }
        return false;
    }

    for (const auto& pos : state.snake) {
        if (pos.x == next.x && pos.y == next.y) {
            state.lives--;
            if (state.lives <= 0) {
                state.gameOver = true;
            } else {
                state.snake = {state.spawn}; // Reseta a posição da cobra para o ponto inicial
                state.direction = RIGHT; // Direção inicial
            }
            return false;
        }
    }

    state.snake.insert(state.snake.begin(), next);
    if (next.x == state.food.x && next.y == state.food.y) {
        state.foodCounter++;
        // Gera nova comida
        do {
            state.food.x = rand() % state.width;
            state.food.y = rand() % state.height;
        } while (state.grid[state.food.y][state.food.x] != ' ');
        return true; // Comida consumida
    } else {
        state.snake.pop_back();
    }

    return false;
}

Direction getNextDirection(const GameState& state) {
    struct Node {
        Position pos;
        std::vector<Direction> path;
    };

    std::queue<Node> q;
    std::map<Position, bool> visited;
    q.push({state.snake.front(), {}});

    while (!q.empty()) {
        Node current = q.front();
        q.pop();

        if (current.pos.x == state.food.x && current.pos.y == state.food.y) {
            return current.path.empty() ? state.direction : current.path.front();
        }

        for (auto dir : getValidDirections(current.pos, state)) {
            Position next = getNextPosition(current.pos, dir);
            if (!visited[next]) {
                visited[next] = true;
                std::vector<Direction> newPath = current.path;
                newPath.push_back(dir);
                q.push({next, newPath});
            }
        }
    }

    // Se não encontrar um caminho, retorna a direção atual
    return state.direction;
}