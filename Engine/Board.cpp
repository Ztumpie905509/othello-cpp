#include <string>

#include "Board.h"

class Board;

Board::Board() : pos_({-1, -1}), type_(ContentType::EMPTY){};
Board::Board(Position pos, ContentType type) : pos_(pos), type_(type) {}

void Board::setType(ContentType newType) { this->type_ = newType; };
ContentType Board::getType() const { return this->type_; }

bool Board::operator==(const Board &other) const { return this->pos_ == other.pos_; }
bool Board::operator!=(const Board &other) const { return this->pos_ != other.pos_; }

Position Board::getPos() const { return this->pos_; }
char Board::getChar() const
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