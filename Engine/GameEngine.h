#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <memory>
#include <vector>
#include <unordered_map>

#include "Board.h"

extern const int BOARD_SIZE;
extern const int TOTAL_SIZE;

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

enum class ContentType;
class Position;

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

    ContentType playerSide_;
    ContentType oppoSide_;

    Board board_;

    Position lastMove;
    std::vector<Position> flipped;
    std::vector<Position> valid;

    ContentType getType(Position) const;

    void printAdditionalInfo() const;

    void addPiece(Position);
    void flip(FlipInfo);

    FlipInfo getFlipArray(Position, ContentType) const;
    std::vector<Position> getAvaliableMove(ContentType) const;

    static int evaluateBoard(const GameEngine &, ContentType);
    int alphaBetaMinimax(GameEngine &, int, int, int, bool);

    void mcts(GameEngine, int, bool, std::vector<Position> &);
    GameOutcome simulateRandomGame(bool, int depth);

public:
    GameEngine() = default;
    GameEngine(ContentType);
    ~GameEngine() = default;

    void setDiff(int);

    Position opponentTurn();
    Position playerTurn();

    void printBoard() const;
    ContentType getPlayerSide() const;
    GameOutcome checkWin(bool) const;

    friend Board;
};

#endif
