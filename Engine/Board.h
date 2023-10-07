#ifndef BOARD_H
#define BOARD_H

#include <string>

namespace printColor
{
    const std::string RESET_COLOR = {"\033[0m"};
    const std::string RED = {"\033[31;1m"};
    const std::string GREEN = {"\033[32;1m"};
    const std::string BOLD_YELLOW = {"\033[33;1m"};
    const std::string YELLOW = {"\033[33m"};
    const std::string CYAN = {"\033[36;1m"};
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
    Board() : pos_({-1, -1}), type_(ContentType::EMPTY){};
    Board(Position pos, ContentType type) : pos_(pos), type_(type) {}

    void setType(ContentType newType) { this->type_ = newType; };
    ContentType getType() const { return this->type_; }

    bool operator==(const Board &other) const { return this->pos_ == other.pos_; }
    bool operator!=(const Board &other) const { return this->pos_ != other.pos_; }

    Position getPos() const { return this->pos_; }
    char getChar() const
    {
        if (this->type_ == ContentType::WHITE)
        {
            return 'X';
        }
        else if (this->type_ == ContentType::BLACK)
        {
            return 'O';
        }
        else
        {
            return '.';
        }
    }

    static char getChar(ContentType type)
    {
        if (type == ContentType::WHITE)
        {
            return 'X';
        }
        else if (type == ContentType::BLACK)
        {
            return 'O';
        }
        else
        {
            return '.';
        }
    }
};

#endif
