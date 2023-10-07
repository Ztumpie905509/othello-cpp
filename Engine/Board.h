#ifndef BOARD_H
#define BOARD_H

#include <string>

namespace printColor
{
    const std::string RESET_COLOR = {"\033[0m"};
    const std::string RED = {"\033[31;1m"};
    const std::string GREEN = {"\033[32;1m"};
    const std::string YELLOW = {"\033[33m"};
    const std::string UNDER_YELLOW = {"\033[33;4m"};
    const std::string BLUE = {"\033[34;1m"};
}

enum ContentType
{
    EMPTY,
    WHITE,
    BLACK
};

struct Position
{
    int x = -1, y = -1;
    bool operator==(const Position &other) const
    {
        return x == other.x && y == other.y;
    }
    bool operator!=(const Position &other) const
    {
        return x != other.x || y != other.y;
    }
};

class Board
{
private:
    ContentType type_;
    Position pos_;

public:
    Board();
    Board(Position, ContentType);

    void setType(ContentType);
    ContentType getType() const;

    bool operator==(const Board &) const;
    bool operator!=(const Board &) const;

    Position getPos() const;
    char getChar() const;

    static char getChar(ContentType);
};

#endif
