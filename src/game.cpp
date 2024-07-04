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

/**
 * @brief Calcula a próxima posição da cobra com base na direção fornecida.
 *
 * Esta função recebe a posição atual da cobra e uma direção, e retorna a nova 
 * posição da cobra após mover-se uma unidade na direção especificada.
 *
 * @param current A posição atual da cobra.
 * @param direction A direção em que a cobra deve se mover.
 * @return A nova posição da cobra após mover-se na direção especificada.
 */
Position getNextPosition(Position current, Direction direction) {
    switch (direction) {
        case UP: current.y--; break;
        case DOWN: current.y++; break;
        case LEFT: current.x--; break;
        case RIGHT: current.x++; break;
    }
    return current;
}

/**
 * @brief Obtém as direções válidas para a próxima movimentação da cobra.
 *
 * Esta função avalia todas as direções possíveis (cima, baixo, esquerda, direita)
 * a partir da posição atual da cobra e retorna as direções que não resultam em colisão
 * com paredes ou com o próprio corpo da cobra.
 *
 * @param current A posição atual da cobra.
 * @param state O estado atual do jogo, contendo informações sobre a grade, a posição da cobra e outros detalhes.
 * @return Um vetor contendo as direções válidas para a próxima movimentação da cobra.
 */
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

/**
 * @brief Inicializa o estado do jogo a partir de um arquivo de nível.
 *
 * Esta função carrega o estado do jogo a partir de um arquivo de nível especificado,
 * inicializando a cobra, a grade do jogo e a posição da comida. Se o arquivo contém
 * caracteres inválidos, o nível será ignorado.
 *
 * @param filename O nome do arquivo de nível a ser carregado.
 * @return O estado inicializado do jogo.
 */
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
    std::getline(file, line); 

    for (int i = 0; i < state.height; ++i) {
    std::getline(file, state.grid[i]);
    for (int j = 0; j < state.width; ++j) {
        char cell = state.grid[i][j];
        if (cell == '&') {
            state.spawn = {j, i};
            state.snake = {state.spawn};
            state.grid[i][j] = ' ';
        } else if (cell == '$' || std::isdigit(cell)) {
            std::cerr << "Warning: Level " << filename << " contains invalid character '" << cell << "' and will be skipped." << std::endl;
            state.gameOver = true;
            return state;
        }
    }
}
    do {
        state.food.x = rand() % state.width;
        state.food.y = rand() % state.height;
    } while (state.grid[state.food.y][state.food.x] != ' ');

    state.direction = RIGHT;
    state.gameOver = false;
    state.lives = 5; 
    state.foodCounter = 0; 
    return state;
}

/**
 * @brief Renderiza o estado atual do jogo no terminal.
 *
 * Esta função limpa a tela e imprime o estado atual do jogo, incluindo vidas restantes, pontuação,
 * quantidade de comida consumida e a posição atual da comida. A função percorre o tabuleiro de jogo
 * e imprime caracteres para representar a comida, as partes da cobra e as paredes. Após a renderização,
 * a tela é atualizada para refletir as mudanças feitas.
 *
 * @param state O estado atual do jogo a ser renderizado.
 * @param foodRequired Número total de comidas que devem ser consumidas para completar o jogo.
 */
void renderGame(const GameState& state, int foodRequired) {
    clear(); 

    mvprintw(0, 0, "Lives: %d | Score: %d | Food eaten: %d of %d", state.lives, state.foodCounter * 10, state.foodCounter, foodRequired);

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

    refresh(); 
}

/**
 * @brief Atualiza o estado do jogo após cada movimento da cobra.
 *
 * Esta função verifica se a próxima posição da cabeça da cobra é válida,
 * atualiza o estado do jogo conforme necessário (por exemplo, movendo a cobra,
 * gerando nova comida, verificando colisões) e verifica se o jogo acabou devido
 * a colisões ou se todas as comidas foram consumidas.
 *
 * @param state O estado atual do jogo.
 * @return Verdadeiro se a comida foi consumida nesta iteração, falso caso contrário.
 */
bool updateGame(GameState& state) {
    Position next = getNextPosition(state.snake.front(), state.direction);

    if (next.x < 0 || next.x >= state.width || next.y < 0 || next.y >= state.height || state.grid[next.y][next.x] == '#') {
        state.lives--;
        if (state.lives <= 0) {
            state.gameOver = true;
        } else {
            state.snake = {state.spawn}; 
            state.direction = RIGHT; 
        }
        return false;
    }

    for (const auto& pos : state.snake) {
        if (pos.x == next.x && pos.y == next.y) {
            state.lives--;
            if (state.lives <= 0) {
                state.gameOver = true;
            } else {
                state.snake = {state.spawn}; 
                state.direction = RIGHT; 
            }
            return false;
        }
    }

    state.snake.insert(state.snake.begin(), next);
    if (next.x == state.food.x && next.y == state.food.y) {
        state.foodCounter++;
        do {
            state.food.x = rand() % state.width;
            state.food.y = rand() % state.height;
        } while (state.grid[state.food.y][state.food.x] != ' ');
        return true; 
    } else {
        state.snake.pop_back();
    }

    return false;
}

/**
 * @brief Calcula a próxima direção que a cobra deve seguir para alcançar a comida mais próxima.
 *
 * Esta função utiliza uma busca em largura para encontrar o caminho mais curto da cabeça da cobra
 * até a posição atual da comida no tabuleiro. Retorna a primeira direção do caminho encontrado
 * que a cobra deve seguir para se mover em direção à comida.
 *
 * @param state O estado atual do jogo.
 * @return A próxima direção que a cobra deve seguir para alcançar a comida mais próxima.
 *         Se não houver um caminho válido encontrado, retorna a direção atual da cobra.
 */
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

    return state.direction;
}