#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <memory>
#include <vector>

#include "Board.h"

extern const int BOARD_SIZE;

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

public:
    GameEngine(ContentType);
    ~GameEngine() = default;

    ContentType getPlayerSide() const;

    ContentType getType(Position) const;

    void setDiff(int);

    void printBoard() const;
    void printAdditionalInfo() const;

    void addPiece(Position);
    void flip(FlipInfo);

    FlipInfo getFlipArray(Position, ContentType);
    std::vector<Position> getAvaliableMove(ContentType);

    static int evaluateBoard(const GameEngine &, ContentType);
    int alphaBetaMinimax(GameEngine &, int, int, int, bool);

    Position opponentTurn();
    Position playerTurn();

    Position mcts(GameEngine gameEngine, int numSimulations, ContentType side);
    void mcts(GameEngine, int, ContentType, std::vector<Position> &);
    GameOutcome simulateRandomGame(ContentType);

    static GameOutcome checkWin(const Board &, bool);
    GameOutcome checkWin(bool) const;

    friend Board;
};

#endif
