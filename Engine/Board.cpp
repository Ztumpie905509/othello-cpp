#include <functional>
#include <string>

#include "Board.h"
#include "GameEngine.h"

class Position;
class Board;

Position::Position() : x(-1), y(-1), type_(ContentType::EMPTY) {};
Position::Position(int x, int y, ContentType ctty) : x(x), y(y), type_(ctty) {};

std::size_t Position::operator()(const Position &p) const
{
    std::size_t h1 = std::hash<int>()(p.getX());
    std::size_t h2 = std::hash<int>()(p.getY());
    std::size_t h3 = std::hash<ContentType>()(p.getType());

    return h1 + (h2 * 31) + (h3 * 31 * 31);
}

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

bool Position::operator<(const Position &other) const
{
    return this->x < other.x && this->y < other.y;
}

std::ostream &operator<<(std::ostream &ostream, const Position &p)
{
    ostream << p.getX() << " " << p.getY();
    return ostream;
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

void Board::update(const Position &pos)
{
    this->pos_[pos.x][pos.y].update(pos.getType());
};

ContentType Board::getType(const Position &pos) const
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

char Board::getChar(const Position &pos) const
{
    return this->pos_[pos.x][pos.y].getChar();
};
