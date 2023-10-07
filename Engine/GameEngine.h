#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <vector>
#include <memory>

#include "Board.h"

constexpr int BOARD_SIZE = 8;
constexpr int TILE_WEIGHTS[BOARD_SIZE][BOARD_SIZE] = {
    {4, -3, 2, 2, 2, 2, -3, 4},
    {-3, -4, -1, -1, -1, -1, -4, -3},
    {2, -1, 1, 0, 0, 1, -1, 2},
    {2, -1, 0, 1, 1, 0, -1, 2},
    {2, -1, 0, 1, 1, 0, -1, 2},
    {2, -1, 1, 0, 0, 1, -1, 2},
    {-3, -4, -1, -1, -1, -1, -4, -3},
    {4, -3, 2, 2, 2, 2, -3, 4},
};

enum class GameOutcome
{
    WHITE_WIN,
    BLACK_WIN,
    DRAW,
    IN_PROGRESS
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

    int blackCount_ = 0;
    int whiteCount_ = 0;

    ContentType playerSide_;
    ContentType oppoSide_;

    Board board_[BOARD_SIZE][BOARD_SIZE];

    Position lastMove;
    std::vector<Position> flipped;
    std::vector<Position> valid;

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
    void flip(FlipInfo);

    static FlipInfo getFlipArray(const GameEngine &, int x, int y, ContentType);
    static std::vector<Position> getAvaliableMove(const GameEngine &, ContentType);

    static int evaluateBoard(const GameEngine &gameEngine, ContentType evalSide);
    int alphaBetaMinimax(GameEngine &gameEngine, int depth, int alpha, int beta, bool maximizingPlayer);

    Position opponentTurn();
    Position playerTurn();

    GameOutcome checkWin(bool) const;
};

#endif
