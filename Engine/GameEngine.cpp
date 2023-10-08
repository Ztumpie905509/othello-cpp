#include <algorithm>
#include <iostream>
#include <mutex>
#include <string>
#include <random>
#include <thread>
#include <vector>

std::mutex mtx;

#include "Board.h"
#include "GameEngine.h"

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

ContentType GameEngine::getType(Position pos) const { return this->board_.pos_[pos.x][pos.y].getType(); };

ContentType GameEngine::getPlayerSide() const { return this->playerSide_; }

void GameEngine::printBoard() const
{
    std::cout << "\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";

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
            char item = this->board_.getChar({x, y, ContentType::EMPTY});

            std::vector<Position>::const_iterator flippedPos = std::find(this->flipped.begin(), this->flipped.end(), Position{x, y, ContentType::EMPTY});
            if (flippedPos != this->flipped.end())
            {
                std::cout << printColor::YELLOW << item << printColor::RESET_COLOR << " ";
                continue;
            }

            std::vector<Position>::const_iterator validPos = std::find(this->valid.begin(), this->valid.end(), Position{x, y, ContentType::EMPTY});
            if (validPos != this->valid.end())
            {
                std::cout << printColor::GREEN << '+' << printColor::RESET_COLOR << " ";
                continue;
            }

            if (this->lastMove != this->board_.pos_[x][y])
            {
                if (item == Board::getChar(this->playerSide_))
                {
                    std::cout << printColor::BLUE;
                }
                else if (item == Board::getChar(this->oppoSide_))
                {
                    std::cout << printColor::RED;
                }
                std::cout << item << printColor::RESET_COLOR << " ";
            }
            else
                std::cout << printColor::UNDER_YELLOW << item << printColor::RESET_COLOR << " ";
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

    if (this->playerSide_ == ContentType::WHITE)
        this->board_.getNumberColor(yourPiece, oppoPiece);
    else
        this->board_.getNumberColor(oppoPiece, yourPiece);

    std::cout << "\n"
              << "Number of your piece: " << yourPiece << "\n"
              << "Number of opponent's piece: " << oppoPiece << "\n";
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

void GameEngine::addPiece(Position pos)
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

FlipInfo GameEngine::getFlipArray(Position pos, ContentType myType)
{
    FlipInfo info;
    info.flipTo = myType;
    info.pos.clear();

    std::vector<Position> directions = {
        {0, 1, ContentType::EMPTY}, {0, -1, ContentType::EMPTY}, {1, 0, ContentType::EMPTY}, {-1, 0, ContentType::EMPTY}, {1, 1, ContentType::EMPTY}, {-1, -1, ContentType::EMPTY}, {1, -1, ContentType::EMPTY}, {-1, 1, ContentType::EMPTY}};

    for (const Position &dir : directions)
    {
        std::vector<Position> temp;
        bool valid = false;

        for (int cur_x = pos.x + dir.x, cur_y = pos.y + dir.y;
             cur_x >= 0 && cur_x < BOARD_SIZE && cur_y >= 0 && cur_y < BOARD_SIZE;
             cur_x += dir.x, cur_y += dir.y)
        {
            if (this->board_.pos_[cur_x][cur_y].getType() == ContentType::EMPTY)
            {
                valid = false;
                break;
            }

            if (this->board_.pos_[cur_x][cur_y].getType() == myType)
            {
                valid = true;
                break;
            }

            Position current = {cur_x, cur_y, ContentType::EMPTY};
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

GameOutcome GameEngine::simulateRandomGame(ContentType side)
{
    GameEngine simulation(*this);

    while (true)
    {
        std::vector<Position> legalMoves = simulation.getAvaliableMove(side);
        if (legalMoves.empty())
            break;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, legalMoves.size() - 1);
        int randomIndex = dis(gen);

        Position randomMove = legalMoves[randomIndex];

        FlipInfo flipPos = simulation.getFlipArray(randomMove, side);
        simulation.addPiece(randomMove);
        simulation.flip(flipPos);
    }

    return simulation.checkWin(false);
}

void GameEngine::mcts(GameEngine gameEngine, int numSimulations, ContentType side, std::vector<Position> &bestMoves)
{
    std::vector<Position> legalMoves = gameEngine.getAvaliableMove(side);

    if (legalMoves.empty())
        return;

    std::vector<double> winRates(legalMoves.size(), 0.0);

    for (int i = 0; i < legalMoves.size(); ++i)
    {
        Position move = legalMoves[i];
        int winCount = 0;

        for (int j = 0; j < numSimulations; ++j)
        {
            GameEngine simulation(*this);
            FlipInfo flipPos = simulation.getFlipArray(move, side);
            simulation.addPiece(move);
            simulation.flip(flipPos);

            GameOutcome outcome = simulation.simulateRandomGame(side);

            int score = evaluateBoard(simulation, side);
            if (score > 0)
                ++winCount;
        }

        winRates[i] = static_cast<double>(winCount) / numSimulations;
    }

    int bestMoveIndex = std::distance(winRates.begin(), std::max_element(winRates.begin(), winRates.end()));

    std::lock_guard<std::mutex> lock(mtx);
    bestMoves.push_back(legalMoves[bestMoveIndex]);
}

std::vector<Position> GameEngine::getAvaliableMove(ContentType side)
{
    int x, y;
    FlipInfo info;

    std::vector<Position> validPlayerPositions;
    validPlayerPositions.reserve(BOARD_SIZE * BOARD_SIZE);

    for (x = 0; x < BOARD_SIZE; ++x)
    {
        for (y = 0; y < BOARD_SIZE; ++y)
        {
            if (this->board_.pos_[x][y].getType() == ContentType::EMPTY)
            {
                info = this->getFlipArray({x, y, side}, side);
                if (info.pos.size() != 0)
                {
                    validPlayerPositions.push_back({x, y, ContentType::EMPTY});
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
        std::cout << validPlayerPositions[i].x << " " << validPlayerPositions[i].y << " | ";
        if ((i + 1) % 5 == 0)
        {
            std::cout << "\n";
        }
    }
    std::cout << "\n\n"
              << "Your input: ";

#ifdef DEBUG
    int maxDepth = this->difficulty_ / 2 + 1;

    int alpha = std::numeric_limits<int>::min();
    int beta = std::numeric_limits<int>::max();

    int bestScore = std::numeric_limits<int>::min();
    Position bestMove = validPlayerPositions[0];

    std::shuffle(validPlayerPositions.begin(), validPlayerPositions.end(), std::mt19937(0));

    for (int depth = 0; depth < maxDepth; ++depth)
    {
        for (const Position &move : validPlayerPositions)
        {
            GameEngine boardCopy(*this);

            boardCopy.addPiece({move.x, move.y, this->playerSide_});
            info = boardCopy.getFlipArray({move.x, move.y, this->playerSide_}, this->playerSide_);
            boardCopy.flip(info);

            int score = alphaBetaMinimax(boardCopy, depth, alpha, beta, true);

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

int GameEngine::alphaBetaMinimax(GameEngine &gameEngine, int depth, int alpha, int beta, bool maximizingPlayer)
{
    if (depth <= 0 || this->board_.blackCount_ + this->board_.whiteCount_ == BOARD_SIZE * BOARD_SIZE)
    {
        return evaluateBoard(gameEngine, this->oppoSide_);
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

        std::vector<Position> validOpponentPositions = getAvaliableMove(gameEngine.oppoSide_);

        for (const Position &move : validOpponentPositions)
        {
            GameEngine boardCopy(gameEngine);

            boardCopy.addPiece({move.x, move.y, gameEngine.oppoSide_});
            FlipInfo flipInfo = boardCopy.getFlipArray({move.x, move.y, gameEngine.oppoSide_}, gameEngine.oppoSide_);
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
    std::vector<Position> validOpponentPositions = this->getAvaliableMove(this->oppoSide_);
    FlipInfo flipInfo;

#ifdef DEBUG
    this->valid = validOpponentPositions;
    this->printBoard();
    std::cout << validOpponentPositions.size() << " Opponent valid positions:\n";
    for (size_t i = 0; i < validOpponentPositions.size(); ++i)
    {
        std::cout << validOpponentPositions[i].x << " " << validOpponentPositions[i].y << " | ";
        if ((i + 1) % 5 == 0)
        {
            std::cout << "\n";
        }
    }
    this->valid.clear();

#endif

    if (validOpponentPositions.size() == 0)
    {
        std::cout << "The opponent does not have a valid position to place a piece. Skipping opponent's turn...\n";
        return {-1, -1, ContentType::EMPTY};
    }

    int x, y;
    if (difficulty_ == -1 || validOpponentPositions.size() == 1 || this->board_.blackCount_ + this->board_.whiteCount_ == 4)
    {
        std::uniform_int_distribution<std::size_t> distribution(0, validOpponentPositions.size() - 1);

#ifdef DEBUG
        std::mt19937 generator(0);
#else
        std::mt19937 generator(std::random_device{}());
#endif

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
        int depth = 100 * this->difficulty_ * this->difficulty_;

        int numThreads = std::thread::hardware_concurrency() / 4;
        if (!numThreads)
            ++numThreads;

        std::vector<std::thread> threads;

        std::vector<Position> bestMoves;
        for (int i = 0; i < numThreads; ++i)
        {
            threads.emplace_back([=, &bestMoves]
                                 { this->mcts(*this, depth / numThreads, this->oppoSide_, bestMoves); });
        }

        for (auto &thread : threads)
        {
            thread.join();
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, bestMoves.size() - 1);
        int randomIndex = dis(gen);

        Position bestMove = bestMoves[randomIndex];

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
    int white, black;
    this->board_.getNumberColor(white, black);

    if (stale)
    {
        if (white == black)
            return GameOutcome::DRAW;
        else if (white > black)
            return GameOutcome::WHITE_WIN;
        else if (black > white)
            return GameOutcome::BLACK_WIN;
        else
            return GameOutcome::IN_PROGRESS;
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
    return GameOutcome::IN_PROGRESS;
}
