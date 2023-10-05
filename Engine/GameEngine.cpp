#include <algorithm>
#include <iostream>
#include <string>
#include <random>
#include <vector>

#include "GameEngine.h"

class GameEngine;

GameEngine::GameEngine(ContentType playerSide)
{
    this->playerSide_ = playerSide;
    this->oppoSide_ = (this->getPlayerSide() == ContentType::WHITE) ? ContentType::BLACK : ContentType::WHITE;

    for (int x = 0; x < BOARD_SIZE; ++x)
    {
        for (int y = 0; y < BOARD_SIZE; ++y)
        {
            if ((x == BOARD_SIZE / 2 - 1 && y == BOARD_SIZE / 2) || (x == BOARD_SIZE / 2 && y == BOARD_SIZE / 2 - 1))
            {
                this->board_[x][y] = Board(x, y, ContentType::WHITE);
                ++this->whiteCount_;
            }
            else if ((x == BOARD_SIZE / 2 && y == BOARD_SIZE / 2) || (x == BOARD_SIZE / 2 - 1 && y == BOARD_SIZE / 2 - 1))
            {
                this->board_[x][y] = Board(x, y, ContentType::BLACK);
                ++this->blackCount_;
            }
            else
            {
                this->board_[x][y] = Board(x, y, ContentType::EMPTY);
            }
        }
    }
}

ContentType GameEngine::getPlayerSide() const
{
    return this->playerSide_;
}

void GameEngine::printBoard() const
{
    std::cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";

    std::string x_axis = "X axis";
    std::string y_axis = "Y axis";
    std::string hori_indent = "       ";

    std::cout << "Current Board:\n";
    std::cout << "\n";
    std::cout << '\n';
    for (int i = 0; i < 2 * BOARD_SIZE / 3; ++i)
    {
        std::cout << " ";
    }
    std::cout << hori_indent << "  " << x_axis << '\n';

    std::cout << "  " << hori_indent;
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        std::cout << i << " ";
    }
    std::cout << "\n";

    for (int y = BOARD_SIZE - 1; y >= 0; --y)
    {
        if (y == BOARD_SIZE / 2)
        {
            std::cout << y_axis << " ";
        }
        else
        {
            std::cout << "       ";
        }
        std::cout << y << " ";
        for (int x = 0; x < BOARD_SIZE; ++x)
        {
            std::cout << this->board_[x][y].getStr() << " ";
        }
        std::cout << y << " ";
        if (y == BOARD_SIZE / 2)
        {
            std::cout << y_axis << " ";
        }
        std::cout << '\n';
    }

    std::cout << "  " << hori_indent;
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        std::cout << i << " ";
    }
    std::cout << '\n';
    for (int i = 0; i < 2 * BOARD_SIZE / 3; ++i)
    {
        std::cout << " ";
    }
    std::cout << hori_indent << "  " << x_axis << '\n';

    int yourPiece, oppoPiece;

    if (this->getPlayerSide() == ContentType::WHITE)
        getNumberColor(yourPiece, oppoPiece);
    else
        getNumberColor(oppoPiece, yourPiece);

    std::cout << "\n"
              << "Number of your piece: " << yourPiece << "\n"
              << "Number of opponent's piece: " << oppoPiece << "\n";
}

void GameEngine::getNumberColor(int &white, int &black) const
{
    white = this->whiteCount_;
    black = this->blackCount_;
}

void GameEngine::printAdditionalInfo() const
{
    std::cout << '\n';
    Board white = Board(-1, -1, ContentType::WHITE);
    Board black = Board(-1, -1, ContentType::BLACK);
    Board empty = Board(-1, -1, ContentType::EMPTY);

    if (playerSide_ == ContentType::WHITE)
        std::cout << white.getStr() << " is your piece | "
                  << black.getStr() << " is opponent's piece | ";
    else
        std::cout << black.getStr() << " is your piece | "
                  << white.getStr() << " is opponent's piece | ";

    std::cout << empty.getStr() << " represents empty checker\n\n";

    std::cout << "Enter a coordinate to add your piece.\n"
              << "Example input:\n2 3\n"
              << "Will add your piece at (x, y) = (2, 3)\n\n";
}

void GameEngine::setDiff(int d)
{
    this->difficulty_ = d;
}

void GameEngine::addPiece(int x, int y, ContentType c)
{
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
    {
        return;
    }
    if (c == ContentType::WHITE)
    {
        this->board_[x][y].setType(ContentType::WHITE);
        ++this->whiteCount_;
    }
    else if (c == ContentType::BLACK)
    {
        this->board_[x][y].setType(ContentType::BLACK);
        ++this->blackCount_;
    }
}

FlipInfo GameEngine::getFlipArray(int x, int y, ContentType myType)
{
    FlipInfo info;
    info.flipTo = myType;
    info.pos.clear();

    std::vector<Position> directions = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {-1, -1}, {1, -1}, {-1, 1}};

    for (const Position &dir : directions)
    {
        std::vector<Position> temp;
        bool valid = false;

        for (int cur_x = x + dir.x, cur_y = y + dir.y;
             cur_x >= 0 && cur_x < BOARD_SIZE && cur_y >= 0 && cur_y < BOARD_SIZE;
             cur_x += dir.x, cur_y += dir.y)
        {
            if (this->board_[cur_x][cur_y].getType() == ContentType::EMPTY)
            {
                valid = false;
                break;
            }

            if (this->board_[cur_x][cur_y].getType() == myType)
            {
                valid = true;
                break;
            }

            Position current = {cur_x, cur_y};
            temp.push_back(current);
        }

        if (valid)
        {
            info.pos.insert(info.pos.end(), temp.begin(), temp.end());
        }
    }

    return info;
}

void GameEngine::flip(FlipInfo info)
{
    for (const Position &pos : info.pos)
    {
        if (info.flipTo == ContentType::WHITE)
        {
            if (this->board_[pos.x][pos.y].getType() == ContentType::BLACK)
            {
                --this->blackCount_;
            }
            this->board_[pos.x][pos.y].setType(ContentType::WHITE);
            ++this->whiteCount_;
        }
        else if (info.flipTo == ContentType::BLACK)
        {
            if (this->board_[pos.x][pos.y].getType() == ContentType::WHITE)
            {
                --this->whiteCount_;
            }
            this->board_[pos.x][pos.y].setType(ContentType::BLACK);
            ++this->blackCount_;
        }
    }
}

Position GameEngine::playerTurn()
{
    int x, y;
    FlipInfo info;

    std::vector<Position> validPlayerPositions;
    validPlayerPositions.reserve(BOARD_SIZE * BOARD_SIZE);

    // Find all valid player positions
    for (x = 0; x < BOARD_SIZE; ++x)
    {
        for (y = 0; y < BOARD_SIZE; ++y)
        {
            if (this->board_[x][y].getType() == ContentType::EMPTY)
            {
                info = getFlipArray(x, y, this->getPlayerSide());
                if (!info.pos.empty())
                {
                    validPlayerPositions.push_back({x, y});
                }
            }
        }
    }

    if (validPlayerPositions.empty())
    {
        std::cout << "You do not have a valid position to place a piece. Skipping your turn...\n";
        this->printBoard();

        return {-1, -1};
    }

    this->printAdditionalInfo();

    std::cout << "Your valid positions:\n";
    for (size_t i = 0; i < validPlayerPositions.size(); ++i)
    {
        std::cout << validPlayerPositions[i].x << " " << validPlayerPositions[i].y << " | ";
        if ((i + 1) % 5 == 0)
        {
            std::cout << "\n";
        }
    }

    std::cout << "\n\n"
              << "Your input: ";

    while (true)
    {
        std::cin >> x >> y;

        if (std::cin.fail() || x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE)
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter again: ";
            continue;
        }

        std::vector<Position>::iterator it = std::find(validPlayerPositions.begin(), validPlayerPositions.end(), Position{x, y});
        if (it != validPlayerPositions.end())
        {
            break;
        }
        else
        {
            std::cout << "Invalid position. Please enter again: ";
        }
    }

    info = getFlipArray(x, y, this->getPlayerSide());
    addPiece(x, y, this->getPlayerSide());
    flip(info);

    return {x, y};
}

int GameEngine::alphaBetaMinimax(GameEngine &gameEngine, int depth, int alpha, int beta, bool maximizingPlayer)
{
    if (depth <= 0 || this->blackCount_ + this->whiteCount_ == BOARD_SIZE * BOARD_SIZE)
    {
        return evaluateBoard(gameEngine, this->oppoSide_);
    }

    if (maximizingPlayer)
    {
        int maxScore = std::numeric_limits<int>::min();

        std::vector<Position> validPlayerPositions;
        validPlayerPositions.reserve(BOARD_SIZE * BOARD_SIZE);

        // Find all valid player positions
        for (int x = 0; x < BOARD_SIZE; ++x)
        {
            for (int y = 0; y < BOARD_SIZE; ++y)
            {
                if (gameEngine.board_[x][y].getType() == ContentType::EMPTY)
                {
                    FlipInfo info = gameEngine.getFlipArray(x, y, this->getPlayerSide());
                    if (!info.pos.empty())
                    {
                        validPlayerPositions.push_back({x, y});
                    }
                }
            }
        }

        for (const Position &move : validPlayerPositions)
        {
            GameEngine boardCopy(gameEngine);

            boardCopy.addPiece(move.x, move.y, gameEngine.getPlayerSide());
            FlipInfo flipInfo = boardCopy.getFlipArray(move.x, move.y, gameEngine.getPlayerSide());
            boardCopy.flip(flipInfo);

            int score = alphaBetaMinimax(boardCopy, depth - 1, alpha, beta, false);

            alpha = std::max(alpha, score);

            if (score > maxScore)
            {
                maxScore = score;
            }

            if (beta <= alpha)
            {
                break;
            }
        }

        return maxScore;
    }
    else
    {
        int minScore = std::numeric_limits<int>::max();

        std::vector<Position> validOpponentPositions;
        validOpponentPositions.reserve(BOARD_SIZE * BOARD_SIZE);

        // Find all valid opponent positions
        for (int x = 0; x < BOARD_SIZE; ++x)
        {
            for (int y = 0; y < BOARD_SIZE; ++y)
            {
                if (gameEngine.board_[x][y].getType() == ContentType::EMPTY)
                {
                    FlipInfo info = gameEngine.getFlipArray(x, y, gameEngine.oppoSide_);
                    if (!info.pos.empty())
                    {
                        validOpponentPositions.push_back({x, y});
                    }
                }
            }
        }

        for (const Position &move : validOpponentPositions)
        {
            GameEngine boardCopy(gameEngine);

            boardCopy.addPiece(move.x, move.y, gameEngine.oppoSide_);
            FlipInfo flipInfo = boardCopy.getFlipArray(move.x, move.y, gameEngine.oppoSide_);
            boardCopy.flip(flipInfo);

            int score = alphaBetaMinimax(boardCopy, depth - 1, alpha, beta, true);

            beta = std::min(beta, score);

            if (score < minScore)
            {
                minScore = score;
            }

            if (beta <= alpha)
            {
                break;
            }
        }

        return minScore;
    }
}

Position GameEngine::opponentTurn()
{
    std::vector<Position> validOpponentPositions;

    // Find all valid opponent positions
    for (int x = 0; x < BOARD_SIZE; ++x)
    {
        for (int y = 0; y < BOARD_SIZE; ++y)
        {
            if (this->board_[x][y].getType() == ContentType::EMPTY)
            {
                FlipInfo flipInfo = getFlipArray(x, y, this->oppoSide_);
                if (!flipInfo.pos.empty())
                {
                    validOpponentPositions.push_back({x, y});
                }
            }
        }
    }

    if (validOpponentPositions.empty())
    {
        std::cout << "The opponent does not have a valid position to place a piece. Skipping opponent's turn...\n";
        return {-1, -1};
    }

    int x, y;
    if (difficulty_ == -1 || validOpponentPositions.size() == 1 || this->blackCount_ + this->whiteCount_ == 4)
    {
        std::uniform_int_distribution<std::size_t> distribution(0, validOpponentPositions.size() - 1);
        std::mt19937 generator(std::random_device{}());
        std::size_t number = distribution(generator);
        x = validOpponentPositions[number].x;
        y = validOpponentPositions[number].y;

        FlipInfo flipInfo = getFlipArray(x, y, this->oppoSide_);
        addPiece(x, y, this->oppoSide_);
        flip(flipInfo);

        return {x, y};
    }
    else
    {
        int alpha = std::numeric_limits<int>::min();
        int beta = std::numeric_limits<int>::max();
        int depth = this->difficulty_;

        std::vector<Position> validOpponentPositions;
        validOpponentPositions.reserve(BOARD_SIZE * BOARD_SIZE);

        // Find all valid opponent positions
        for (int x = 0; x < BOARD_SIZE; ++x)
        {
            for (int y = 0; y < BOARD_SIZE; ++y)
            {
                if (board_[x][y].getType() == ContentType::EMPTY)
                {
                    FlipInfo info = getFlipArray(x, y, oppoSide_);
                    if (!info.pos.empty())
                    {
                        validOpponentPositions.push_back({x, y});
                    }
                }
            }
        }

        if (validOpponentPositions.empty())
        {
            std::cout << "Opponent does not have a valid position to place a piece. Skipping their turn...\n";
            return {-1, -1};
        }

        int bestScore = std::numeric_limits<int>::min();
        Position bestMove = {-1, -1};

        for (const Position &move : validOpponentPositions)
        {
            GameEngine boardCopy(*this);

            boardCopy.addPiece(move.x, move.y, this->oppoSide_);
            FlipInfo flipInfo = boardCopy.getFlipArray(move.x, move.y, this->oppoSide_);
            boardCopy.flip(flipInfo);

            int score = alphaBetaMinimax(boardCopy, depth, alpha, beta, false);

            if (score > bestScore)
            {
                bestScore = score;
                bestMove = move;
            }
        }

        FlipInfo flipInfo = getFlipArray(bestMove.x, bestMove.y, this->oppoSide_);
        addPiece(bestMove.x, bestMove.y, this->oppoSide_);
        flip(flipInfo);
        return bestMove;
    }
}

int GameEngine::evaluateBoard(const GameEngine &gameEngine, ContentType evalSide)
{
    int score = 0;
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            if (gameEngine.getBoard({i, j}) == ContentType::EMPTY)
                continue;

            if (gameEngine.getBoard({i, j}) == evalSide)
            {
                score += TILE_WEIGHTS[i][j];
            }
            else
            {
                score -= TILE_WEIGHTS[i][j];
            }
        }
    }
    return score;
}

GameOutcome GameEngine::checkWin(bool stale) const
{
    int white, black;
    getNumberColor(white, black);

    if (stale)
    {
        if (white > black)
            return GameOutcome::WHITE_WIN;
        else if (black > white)
            return GameOutcome::BLACK_WIN;
        else
            return GameOutcome::DRAW;
    }
    else
    {
        if (black == 0 || (white + black == BOARD_SIZE * BOARD_SIZE && white > black))
            return GameOutcome::WHITE_WIN;
        else if (white == 0 || (white + black == BOARD_SIZE * BOARD_SIZE && white < black))
            return GameOutcome::BLACK_WIN;
        else if (white + black == BOARD_SIZE * BOARD_SIZE && white == black)
            return GameOutcome::DRAW;
        else
            return GameOutcome::IN_PROGRESS;
    }
}
