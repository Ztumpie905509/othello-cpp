#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <vector>
#include <memory>

#include "Board.h"

constexpr int BOARD_SIZE = 8;
constexpr int TILE_WEIGHTS[BOARD_SIZE][BOARD_SIZE] = {
    {100, -25, 10, 5, 5, 10, -25, 100},
    {-25, -50, 1, 1, 1, 1, -50, -25},
    {10, 1, 3, 2, 2, 3, 1, 10},
    {5, 1, 2, 1, 1, 2, 1, 5},
    {5, 1, 2, 1, 1, 2, 1, 5},
    {10, 1, 3, 2, 2, 3, 1, 10},
    {-25, -50, 1, 1, 1, 1, -50, -25},
    {100, -25, 10, 5, 5, 10, -25, 100}};

enum class GameOutcome
{
    WHITE_WIN,
    BLACK_WIN,
    DRAW,
    IN_PROGRESS
};

struct Position
{
    int x, y;
    bool operator==(const Position &other) const
    {
        return x == other.x && y == other.y;
    }
};

struct FlipInfo
{
    ContentType flipTo;
    std::vector<Position> pos;
};

class GameEngine
{
private:
    int difficulty_ = 0;

    ContentType playerSide_;
    ContentType oppoSide_;

    Board board_[BOARD_SIZE][BOARD_SIZE];

public:
    GameEngine(ContentType);
    ~GameEngine() = default;

    ContentType getPlayerSide() const;

    ContentType getBoard(Position pos) const { return this->board_[pos.x][pos.y].getType(); };

    void setDiff(int);

    void printBoard() const;
    void printAdditionalInfo() const;

    void getNumberColor(int &white, int &black) const;

    void addPiece(int x, int y, ContentType);
    FlipInfo getFlipArray(int x, int y, ContentType);
    void flip(FlipInfo);

    static int evaluateBoard(const GameEngine &gameEngine, ContentType evalSide);
    int alphaBetaMinimax(GameEngine &gameEngine, int depth, int alpha, int beta, bool maximizingPlayer);

    Position opponentTurn();
    Position playerTurn();

    GameOutcome checkWin(bool) const;
};

#endif
