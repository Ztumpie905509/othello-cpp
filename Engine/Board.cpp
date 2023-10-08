#include <string>

#include "Board.h"
#include "GameEngine.h"

class Position;
class Board;

Position::Position(int x, int y, ContentType ctty) : x(x), y(y), type_(ctty){};

void Position::update(ContentType type)
{
    this->type_ = type;
};

ContentType Position::getType() const
{
    return this->type_;
};

char Position::getChar() const
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

int Position::getX() const
{
    return this->x;
}

int Position::getY() const
{
    return this->y;
}

bool Position::operator==(const Position &other) const
{
    return x == other.x && y == other.y;
}
bool Position::operator!=(const Position &other) const
{
    return x != other.x || y != other.y;
}

Board::Board()
{
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            this->pos_[i][j] = Position{i, j, ContentType::EMPTY};
        }
    }
}

void Board::update(Position pos)
{
    this->pos_[pos.x][pos.y].update(pos.getType());
};

ContentType Board::getType(Position pos) const
{
    return this->pos_[pos.x][pos.y].getType();
}

char Board::getChar(ContentType type)
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

void Board::getNumberColor(int &white, int &black) const
{
    white = this->whiteCount_;
    black = this->blackCount_;
}

char Board::getChar(Position pos) const { return this->pos_[pos.x][pos.y].getChar(); };
