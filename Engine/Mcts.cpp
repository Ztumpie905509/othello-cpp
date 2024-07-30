#include <algorithm>
#include <cmath>
#include <iostream>
#include <mutex>
#include <unordered_map>

#include "Mcts.h"

std::mutex consoleMutex;

MCTS::MCTS(GameEngine &gameEngine, ContentType side, int simulations)
    : gameEngine_(gameEngine), side_(side), simulations_(simulations)
{
    this->root_ = new Node(Position{-1, -1, ContentType::EMPTY});
}

MCTS::~MCTS() { deleteTree(root_); }

void MCTS::run(std::unordered_map<Position, double, Position> &threadMoves)
{
    for (int i = 0; i < simulations_; ++i)
    {
        Node *node = this->select(this->root_);
        if (node->visits > 0)
        {
            this->expand(node);
            node = node->children.back();
        }
        GameOutcome outcome = this->simulate(node);
        this->backpropagate(node, outcome);
    }

    this->printWinProbabilities(threadMoves);
}

void MCTS::merge(std::unordered_map<Position, double, Position> &original, const std::unordered_map<Position, double, Position> &newMap)
{
    for (const std::unordered_map<Position, double>::const_iterator::value_type &it : newMap)
    {
        if (original.count(it.first) > 0)
        {
            original[it.first] += it.second;
        }
        else
        {
            original.insert(it);
        }
    }
}

Node *MCTS::select(Node *node)
{
    while (!node->children.empty())
    {
        node = *std::max_element(node->children.begin(), node->children.end(),
                                 [this](Node *a, Node *b)
                                 {
                                     return this->ucb1(a) < this->ucb1(b);
                                 });
    }
    return node;
}

void MCTS::expand(Node *node)
{
    std::vector<Position> legalMoves = this->gameEngine_.getAvaliableMove(this->side_);
    for (const Position &move : legalMoves)
    {
        node->children.push_back(new Node(move, node));
    }
}

GameOutcome MCTS::simulate(Node *node)
{
    GameEngine simulation(this->gameEngine_);
    Position move = node->move;
    if (move.x != -1 && move.y != -1)
    {
        FlipInfo flipPos = simulation.getFlipArray(move, this->side_);
        simulation.addPiece(move);
        simulation.flip(flipPos);
    }
    return simulation.simulateRandomGame(true, TOTAL_SIZE);
}

void MCTS::backpropagate(Node *node, GameOutcome outcome)
{
    while (node != nullptr)
    {
        node->visits++;
        if ((outcome == GameOutcome::WHITE_WIN && side_ == ContentType::WHITE) ||
            (outcome == GameOutcome::BLACK_WIN && side_ == ContentType::BLACK))
        {
            node->wins++;
        }
        node = node->parent;
    }
}

double MCTS::ucb1(Node *node)
{
    if (node->visits == 0)
        return std::numeric_limits<double>::max();
    return (1 - this->explore_constant) * static_cast<double>(node->wins) / node->visits +
           this->explore_constant * std::sqrt(2 * std::log(node->parent->visits) / node->visits);
}

Position MCTS::bestMove(Node *node)
{
    Node *bestChild = *std::max_element(node->children.begin(), node->children.end(),
                                        [](Node *a, Node *b)
                                        {
                                            return a->wins < b->wins;
                                        });
    return bestChild->move;
}

void MCTS::deleteTree(Node *node)
{
    for (Node *child : node->children)
    {
        this->deleteTree(child);
    }
    delete node;
}

void MCTS::printWinProbabilities(std::unordered_map<Position, double, Position> &threadMoves)
{
    std::lock_guard<std::mutex> lock(consoleMutex);

    std::cout << "Win probabilities for each available move:\n";
    Position bestMove = {-1, -1, ContentType::EMPTY};

    double bestWinRate = 0;

    for (Node *child : this->root_->children)
    {
        double winRate = static_cast<double>(child->wins) / child->visits;
        std::cout << "Move (" << child->move.x << ", " << child->move.y << "): " << winRate * 100 << "%\n";

        threadMoves.insert({child->move, winRate});

        if (winRate >= bestWinRate)
        {
            bestWinRate = winRate;
            bestMove = child->move;
        }
    }

    std::cout << "Best move: (" << bestMove.x << ", " << bestMove.y << ") with a win rate of "
              << bestWinRate * 100 << "%\n\n\n";
    if (this->prev_rate != -1)
    {
        this->explore_constant -= (this->prev_rate - bestWinRate);
    }
    this->prev_rate = bestWinRate;
}
