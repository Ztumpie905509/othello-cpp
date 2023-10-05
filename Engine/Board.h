#ifndef BOARD_H
#define BOARD_H

#include <string>

enum ContentType
{
    EMPTY,
    WHITE,
    BLACK
};

class Board
{
private:
    ContentType type_;
    int x_;
    int y_;

public:
    Board() = default;
    Board(int x, int y, ContentType type) : x_(x), y_(y), type_(type) {}

    void setType(ContentType newType) { this->type_ = newType; };
    ContentType getType() const { return this->type_; }

    void getPos(int &x, int &y) const
    {
        x = x_;
        y = y_;
    }
    std::string getStr() const
    {
        if (type_ == ContentType::WHITE)
        {
            return "X";
        }
        else if (type_ == ContentType::BLACK)
        {
            return "O";
        }
        else
        {
            return ".";
        }
    }
};

#endif
