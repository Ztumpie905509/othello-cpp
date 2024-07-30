#ifndef BOARD_H
#define BOARD_H

#include <ostream>

const int BOARD_SIZE = 8;
const int TOTAL_SIZE = 64;

class Board;
class Position;
class GameEngine;
class MCTS;

namespace printColor
{
    const std::string RESET_COLOR = {"\033[0m"};
    const std::string RED = {"\033[31;1m"};
    const std::string GREEN = {"\033[32;1m"};
    const std::string YELLOW = {"\033[33m"};
    const std::string UNDER_YELLOW = {"\033[33;4m"};
    const std::string BLUE = {"\033[34;1m"};
}

enum class ContentType
{
    EMPTY,
    WHITE,
    BLACK
};

class Position
{
private:
    int x, y;
    ContentType type_;

    void update(ContentType);
    char getChar() const;

public:
    ContentType getType() const;

    Position();
    Position(int x, int y, ContentType ctty);
    Position(const Position &) = default;

    int getX() const;
    int getY() const;

    bool operator==(const Position &) const;
    bool operator!=(const Position &) const;
    bool operator<(const Position &) const;
    size_t operator()(const Position &) const;

    friend std::ostream &operator<<(std::ostream &, const Position &);

    friend Board;
    friend GameEngine;
    friend MCTS;
};

class Board
{
private:
    int blackCount_ = 0;
    int whiteCount_ = 0;

    Position pos_[BOARD_SIZE][BOARD_SIZE];

public:
    Board();
    Board(const Board &) = default;

    void getNumberColor(int &white, int &black) const;

    void update(const Position &);
    ContentType getType(const Position &) const;
    char getChar(const Position &) const;

    static char getChar(ContentType);

    friend GameEngine;
};

#endif
