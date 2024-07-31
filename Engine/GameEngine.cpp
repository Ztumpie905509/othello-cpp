#include <algorithm>
#include <cmath>
#include <future>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <string_view>
#include <thread>
#include <vector>

std::mutex mtx;

#include "Board.h"
#include "GameEngine.h"
#include "Mcts.h"

class GameEngine;

GameEngine::GameEngine(ContentType playerSide)
{
    this->playerSide_ = playerSide;
    this->oppoSide_ = (this->playerSide_ == ContentType::WHITE) ? ContentType::BLACK : ContentType::WHITE;

    for (int x = 0; x < BOARD_SIZE; ++x)
    {
        for (int y = 0; y < BOARD_SIZE; ++y)
        {
            if ((x == BOARD_SIZE / 2 - 1 && y == BOARD_SIZE / 2) || (x == BOARD_SIZE / 2 && y == BOARD_SIZE / 2 - 1))
            {
                this->board_.update({x, y, ContentType::WHITE});
                ++this->board_.whiteCount_;
            }
            else if ((x == BOARD_SIZE / 2 && y == BOARD_SIZE / 2) || (x == BOARD_SIZE / 2 - 1 && y == BOARD_SIZE / 2 - 1))
            {
                this->board_.update({x, y, ContentType::BLACK});
                ++this->board_.blackCount_;
            }
            else
            {
                this->board_.update({x, y, ContentType::EMPTY});
            }
        }
    }
}

ContentType GameEngine::getType(const Position &pos) const { return this->board_.pos_[pos.x][pos.y].getType(); };

ContentType GameEngine::getPlayerSide() const { return this->playerSide_; }

void GameEngine::printBoard() const
{
    std::ostringstream oss;
    oss << "\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";

    std::string_view x_axis = "X axis";
    std::string_view y_axis = "Y axis";
    std::string_view hori_indent = "       ";

    oss << "Current Board:\n\n\n";
    for (int i = 0; i < 2 * BOARD_SIZE / 3; ++i)
    {
        oss << " ";
    }
    oss << hori_indent << "  " << x_axis << '\n';

    oss << "  " << hori_indent;
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        oss << i << " ";
    }
    oss << "\n";

    for (int y = BOARD_SIZE - 1; y >= 0; --y)
    {
        if (y == BOARD_SIZE / 2)
        {
            oss << y_axis << " ";
        }
        else
        {
            oss << "       ";
        }
        oss << y << " ";
        for (int x = 0; x < BOARD_SIZE; ++x)
        {
            char item = this->board_.getChar({x, y, ContentType::EMPTY});

            auto flippedPos = std::find(this->flipped.begin(), this->flipped.end(), Position{x, y, ContentType::EMPTY});
            if (flippedPos != this->flipped.end())
            {
                oss << printColor::YELLOW << item << printColor::RESET_COLOR << " ";
                continue;
            }

            auto validPos = std::find(this->valid.begin(), this->valid.end(), Position{x, y, ContentType::EMPTY});
            if (validPos != this->valid.end())
            {
                oss << printColor::GREEN << '+' << printColor::RESET_COLOR << " ";
                continue;
            }

            if (this->lastMove != this->board_.pos_[x][y])
            {
                if (item == Board::getChar(this->playerSide_))
                {
                    oss << printColor::BLUE;
                }
                else if (item == Board::getChar(this->oppoSide_))
                {
                    oss << printColor::RED;
                }
                oss << item << printColor::RESET_COLOR << " ";
            }
            else
            {
                oss << printColor::UNDER_YELLOW << item << printColor::RESET_COLOR << " ";
            }
        }
        oss << y << " ";
        if (y == BOARD_SIZE / 2)
        {
            oss << y_axis << " ";
        }
        oss << '\n';
    }

    oss << "  " << hori_indent;
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        oss << i << " ";
    }
    oss << '\n';
    for (int i = 0; i < 2 * BOARD_SIZE / 3; ++i)
    {
        oss << " ";
    }
    oss << hori_indent << "  " << x_axis << '\n';

    int yourPiece, oppoPiece;
    if (this->playerSide_ == ContentType::WHITE)
        this->board_.getNumberColor(yourPiece, oppoPiece);
    else
        this->board_.getNumberColor(oppoPiece, yourPiece);

    oss << "\nNumber of your piece: " << yourPiece << "\nNumber of opponent's piece: " << oppoPiece << "\n";

    std::cout << oss.str();
}

void GameEngine::printAdditionalInfo() const
{
    std::cout << '\n';
    Position white = Position(-1, -1, ContentType::WHITE);
    Position black = Position(-1, -1, ContentType::BLACK);
    Position empty = Position(-1, -1, ContentType::EMPTY);

    if (playerSide_ == ContentType::WHITE)
        std::cout << printColor::BLUE << white.getChar() << printColor::RESET_COLOR << " is your piece. | "
                  << printColor::RED << black.getChar() << printColor::RESET_COLOR << " is opponent's piece.\n";
    else
        std::cout << printColor::BLUE << black.getChar() << printColor::RESET_COLOR << " is your piece. | "
                  << printColor::RED << white.getChar() << printColor::RESET_COLOR << " is opponent's piece.\n";

    std::cout << empty.getChar() << " represents empty checker.\n\n"
              << printColor::UNDER_YELLOW << "This" << printColor::RESET_COLOR << " piece represents the last move.\n"
              << printColor::YELLOW << "This" << printColor::RESET_COLOR << " piece represents the flipped piece.\n\n";

    std::cout << printColor::GREEN << '+' << printColor::RESET_COLOR << " is your avaliable positions.\n\n";

    std::cout << "Enter a coordinate to add your piece.\n"
              << "Example input:\n2 3\n"
              << "Will add your piece at (x, y) = (2, 3)\n\n";
}

void GameEngine::setDiff(int d)
{
    this->difficulty_ = d;
}

void GameEngine::addPiece(const Position &pos)
{
    if (pos.x < 0 || pos.x >= BOARD_SIZE || pos.y < 0 || pos.y >= BOARD_SIZE)
    {
        return;
    }
    if (pos.getType() == ContentType::WHITE)
    {
        this->board_.pos_[pos.x][pos.y].update(ContentType::WHITE);
        this->lastMove = this->board_.pos_[pos.x][pos.y];
        ++this->board_.whiteCount_;
    }
    else if (pos.getType() == ContentType::BLACK)
    {
        this->board_.pos_[pos.x][pos.y].update(ContentType::BLACK);
        this->lastMove = this->board_.pos_[pos.x][pos.y];
        ++this->board_.blackCount_;
    }
}

FlipInfo GameEngine::getFlipArray(const Position &pos, ContentType myType) const
{
    FlipInfo info;
    info.flipTo = myType;
    info.pos.clear();

    std::vector<Position> directions = {
        {0, 1, ContentType::EMPTY}, {0, -1, ContentType::EMPTY}, {1, 0, ContentType::EMPTY}, {-1, 0, ContentType::EMPTY}, {1, 1, ContentType::EMPTY}, {-1, -1, ContentType::EMPTY}, {1, -1, ContentType::EMPTY}, {-1, 1, ContentType::EMPTY}};

    for (const Position &dir : directions)
    {
        std::vector<Position> temp;
        bool stepValid = false;

        for (int cur_x = pos.x + dir.x, cur_y = pos.y + dir.y;
             cur_x >= 0 && cur_x < BOARD_SIZE && cur_y >= 0 && cur_y < BOARD_SIZE;
             cur_x += dir.x, cur_y += dir.y)
        {
            if (this->board_.pos_[cur_x][cur_y].getType() == ContentType::EMPTY)
            {
                stepValid = false;
                break;
            }

            if (this->board_.pos_[cur_x][cur_y].getType() == myType)
            {
                stepValid = true;
                break;
            }

            Position current = {cur_x, cur_y, ContentType::EMPTY};
            temp.push_back(current);
        }

        if (stepValid)
        {
            info.pos.insert(info.pos.end(), temp.begin(), temp.end());
        }
    }

    return info;
}

void GameEngine::flip(const FlipInfo &info)
{
    for (const Position &pos : info.pos)
    {
        if (info.flipTo == ContentType::WHITE)
        {
            if (this->board_.pos_[pos.x][pos.y].getType() == ContentType::BLACK)
            {
                --this->board_.blackCount_;
            }
            this->board_.pos_[pos.x][pos.y].update(ContentType::WHITE);
            ++this->board_.whiteCount_;
        }
        else if (info.flipTo == ContentType::BLACK)
        {
            if (this->board_.pos_[pos.x][pos.y].getType() == ContentType::WHITE)
            {
                --this->board_.whiteCount_;
            }
            this->board_.pos_[pos.x][pos.y].update(ContentType::BLACK);
            ++this->board_.blackCount_;
        }
    }
    this->flipped = info.pos;
}

GameOutcome GameEngine::simulateRandomGame(bool oppoSide, int depth)
{

    GameEngine simulation(*this);
    ContentType side;
    std::vector<Position> legalMoves;
    bool stale = false;

    for (;;)
    {
        if (oppoSide)
        {
            oppoSide = 0;
            side = simulation.oppoSide_;
        }
        else
        {
            oppoSide = 1;
            side = simulation.playerSide_;
        }

        legalMoves = simulation.getAvaliableMove(side);

        if (legalMoves.empty())
        {
            if (stale)
            {
                break;
            }
            else
            {
                stale = true;
                continue;
            }
        }

        stale = false;

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::minstd_rand generator(seed);
        std::uniform_int_distribution<> dis(0, legalMoves.size() - 1);
        int randomIndex = dis(generator);

        Position randomMove = legalMoves[randomIndex];

        FlipInfo flipPos = simulation.getFlipArray(randomMove, side);
        simulation.addPiece(randomMove);
        simulation.flip(flipPos);
    }

    return simulation.checkWin(stale);
}

void GameEngine::mcts(const GameEngine &gameEngine, int numSimulations, const std::vector<Position> &legalMoves, std::unordered_map<Position, double, Position> &bestMoves)
{
    std::unordered_map<Position, double, Position> threadMoves;

    MCTS mcts(*this, this->oppoSide_, numSimulations);
    mcts.run(threadMoves);

    std::lock_guard<std::mutex> lock(mtx);
    mcts.merge(bestMoves, threadMoves);
}

std::vector<Position> GameEngine::getAvaliableMove(ContentType side) const
{
    int x, y;
    FlipInfo info;

    std::vector<Position> validPlayerPositions;
    validPlayerPositions.reserve(TOTAL_SIZE);

    for (x = 0; x < BOARD_SIZE; ++x)
    {
        for (y = 0; y < BOARD_SIZE; ++y)
        {
            if (this->board_.pos_[x][y].getType() == ContentType::EMPTY)
            {
                info = this->getFlipArray({x, y, side}, side);
                if (info.pos.size() != 0)
                {
                    validPlayerPositions.push_back({x, y, side});
                }
            }
        }
    }
    return validPlayerPositions;
}

Position GameEngine::playerTurn()
{
    int x, y;
    FlipInfo info;

    std::vector<Position> validPlayerPositions = this->getAvaliableMove(this->playerSide_);
    this->valid = validPlayerPositions;

    this->printBoard();
    this->printAdditionalInfo();

    if (validPlayerPositions.size() == 0)
    {
        std::cout << "You do not have a valid position to place a piece. Skipping your turn...\n";
        this->printBoard();

        return {-1, -1, ContentType::EMPTY};
    }

    std::cout << "Your valid positions:\n";
    for (size_t i = 0; i < validPlayerPositions.size(); ++i)
    {
        std::cout << validPlayerPositions[i] << " | ";
        if ((i + 1) % 5 == 0)
        {
            std::cout << "\n";
        }
    }
    std::cout << "\n\n"
              << "Your input: ";

#ifdef DEBUG
    int maxDepth = 3;

    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    int bestScore = std::numeric_limits<int>::min();
    Position bestMove = validPlayerPositions[0];

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::minstd_rand generator(seed);

    std::shuffle(validPlayerPositions.begin(), validPlayerPositions.end(), generator);

    for (int depth = 0; depth < maxDepth; ++depth)
    {
        for (const Position &move : validPlayerPositions)
        {
            GameEngine boardCopy(*this);

            boardCopy.addPiece({move.x, move.y, this->playerSide_});
            info = boardCopy.getFlipArray({move.x, move.y, this->playerSide_}, this->playerSide_);
            boardCopy.flip(info);

            int score = alphaBetaMinimax(boardCopy, depth, alpha, beta, true, this->playerSide_);

            if (score > bestScore)
            {
                bestScore = score;
                bestMove = move;
            }
        }
    }
    x = bestMove.x;
    y = bestMove.y;
#else
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

        std::vector<Position>::iterator it = std::find(validPlayerPositions.begin(), validPlayerPositions.end(), Position{x, y, ContentType::EMPTY});
        if (it != validPlayerPositions.end())
        {
            break;
        }
        else
        {
            std::cout << "Invalid position. Please enter again: ";
        }
    }

#endif

    info = this->getFlipArray({x, y, this->playerSide_}, this->playerSide_);
    this->addPiece({x, y, this->playerSide_});
    this->flip(info);
    this->valid.clear();

    return {x, y, ContentType::EMPTY};
}

int GameEngine::alphaBetaMinimax(const GameEngine &gameEngine, int depth, int alpha, int beta, bool maximizingPlayer, ContentType evalSide) const
{
    if (depth <= 0 || this->board_.blackCount_ + this->board_.whiteCount_ == TOTAL_SIZE)
    {
        return evaluateBoard(gameEngine, evalSide);
    }

    if (maximizingPlayer)
    {
        int maxScore = std::numeric_limits<int>::min();
        std::vector<Position> validPlayerPositions = gameEngine.getAvaliableMove(gameEngine.playerSide_);
        for (const Position &move : validPlayerPositions)
        {
            GameEngine boardCopy(gameEngine);
            boardCopy.addPiece({move.x, move.y, gameEngine.playerSide_});
            FlipInfo flipInfo = boardCopy.getFlipArray({move.x, move.y, gameEngine.playerSide_}, gameEngine.playerSide_);
            boardCopy.flip(flipInfo);
            int score = alphaBetaMinimax(boardCopy, depth - 1, alpha, beta, false, evalSide);
            maxScore = std::max(maxScore, score);
            alpha = std::max(alpha, score);
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
        std::vector<Position> validOpponentPositions = getAvaliableMove(gameEngine.oppoSide_);
        for (const Position &move : validOpponentPositions)
        {
            GameEngine boardCopy(gameEngine);
            boardCopy.addPiece({move.x, move.y, gameEngine.oppoSide_});
            FlipInfo flipInfo = boardCopy.getFlipArray({move.x, move.y, gameEngine.oppoSide_}, gameEngine.oppoSide_);
            boardCopy.flip(flipInfo);
            int score = alphaBetaMinimax(boardCopy, depth - 1, alpha, beta, true, evalSide);
            minScore = std::min(minScore, score);
            beta = std::min(beta, score);
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
    std::vector<Position> validOpponentPositions = this->getAvaliableMove(this->oppoSide_);
    FlipInfo flipInfo;

    if (validOpponentPositions.size() == 0)
    {
        std::cout << "The opponent does not have a valid position to place a piece. Skipping opponent's turn...\n";
        return {-1, -1, ContentType::EMPTY};
    }

#ifdef DEBUG
    this->valid = validOpponentPositions;
    this->printBoard();
    std::cout << "\nAI valid positions:\n";
    for (size_t i = 0; i < validOpponentPositions.size(); ++i)
    {
        std::cout << validOpponentPositions[i] << " | ";
        if ((i + 1) % 5 == 0)
        {
            std::cout << "\n";
        }
    }
    std::cout << "\n";
    this->valid.clear();
#endif

    int x, y;
    if (difficulty_ == -1 || validOpponentPositions.size() == 1 || this->board_.blackCount_ + this->board_.whiteCount_ == 4)
    {
        std::uniform_int_distribution<std::size_t> distribution(0, validOpponentPositions.size() - 1);
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::minstd_rand generator(seed);
        std::size_t number = distribution(generator);
        x = validOpponentPositions[number].x;
        y = validOpponentPositions[number].y;
        flipInfo = this->getFlipArray({x, y, this->oppoSide_}, this->oppoSide_);
        this->addPiece({x, y, this->oppoSide_});
        this->flip(flipInfo);
        return {x, y, ContentType::EMPTY};
    }
    else
    {
        int simCount = this->difficulty_ * this->difficulty_ * 75;
        int numThreads = std::thread::hardware_concurrency() / 2;
        if (!numThreads)
            ++numThreads;

        std::unordered_map<Position, double, Position> bestMoves;
        std::vector<std::future<void>> futures;

        for (int i = 0; i < numThreads; ++i)
        {
            futures.push_back(std::async(std::launch::async, [=, &validOpponentPositions, &bestMoves]
                                         { this->mcts(*this, simCount / numThreads + 1, validOpponentPositions, bestMoves); }));
        }

        for (std::future<void> &future : futures)
        {
            future.get();
        }

        Position bestMove;
        double bestRate = 0;
        for (std::pair<const Position, double> &it : bestMoves)
        {
            it.second /= numThreads;
        }

        for (const std::pair<Position, double> &node : bestMoves)
        {
            double curRate = node.second;
            if (curRate >= bestRate)
            {
                bestRate = curRate;
                bestMove = node.first;
            }
        }

#ifdef DEBUG
        std::cout << "Final probabilities for all threads combined:\n";
        for (const std::pair<const Position, double> &it : bestMoves)
        {
            std::cout << "Move (" << it.first.x << ", " << it.first.y << "): " << it.second * 100 << "%\n";
        }
        std::cout << "Best move: (" << bestMove.x << ", " << bestMove.y << ") with a win rate of "
                  << bestRate * 100 << "%\n\n\n";
#endif

        flipInfo = this->getFlipArray({bestMove.x, bestMove.y, this->oppoSide_}, this->oppoSide_);
        this->addPiece({bestMove.x, bestMove.y, this->oppoSide_});
        this->flip(flipInfo);
        return bestMove;
    }
}

int GameEngine::evaluateBoard(const GameEngine &gameEngine, ContentType evalSide)
{
    int whiteScore = 0, blackScore = 0, score = 0;
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            if (gameEngine.getType({i, j, ContentType::EMPTY}) == ContentType::EMPTY)
                continue;

            if (gameEngine.getType({i, j, ContentType::EMPTY}) == ContentType::BLACK)
            {
                blackScore += TILE_WEIGHTS[i][j];
            }
            else
            {
                whiteScore += TILE_WEIGHTS[i][j];
            }
        }
    }

    if (evalSide == ContentType::WHITE)
        score = whiteScore - blackScore;
    else
        score = blackScore - whiteScore;

    return score;
}

GameOutcome GameEngine::checkWin(bool stale) const
{
    int white = 0, black = 0;
    this->board_.getNumberColor(white, black);

    if (stale)
    {
        if (white == black)
        {
            return GameOutcome::DRAW;
        }
        return (white > black) ? GameOutcome::WHITE_WIN : GameOutcome::BLACK_WIN;
    }

    if (black == 0 || (white + black == TOTAL_SIZE && white > black))
    {
        return GameOutcome::WHITE_WIN;
    }
    if (white == 0 || (white + black == TOTAL_SIZE && white < black))
    {
        return GameOutcome::BLACK_WIN;
    }
    if (white + black == TOTAL_SIZE && white == black)
    {
        return GameOutcome::DRAW;
    }

    return GameOutcome::IN_PROGRESS;
}
