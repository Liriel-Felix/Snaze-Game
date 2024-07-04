#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <map>
#include <tuple> // Adicionado para usar std::tie

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Position {
    int x, y;
    bool operator<(const Position& other) const {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }
};

struct GameState {
    std::vector<Position> snake;
    Position spawn;
    Position food;
    Direction direction;
    int width, height;
    bool gameOver;
    int lives;
    int foodCounter;
    std::vector<std::string> grid;
};

GameState initializeGame(const std::string& filename);
void renderGame(const GameState& state, int foodRequired);
bool updateGame(GameState& state);
Direction getNextDirection(const GameState& state);
Position getNextPosition(Position current, Direction direction);
std::vector<Direction> getValidDirections(Position current, const GameState& state);

#endif // GAME_H