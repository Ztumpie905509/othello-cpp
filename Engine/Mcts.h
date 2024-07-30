#ifndef MCTS_H
#define MCTS_H

#include <unordered_map>
#include <vector>

#include "Board.h"
#include "GameEngine.h"

struct Node
{
    Position move;
    Node *parent;
    std::vector<Node *> children;
    long long wins;
    long long visits;

    Node(Position move, Node *parent = nullptr)
        : move(move), parent(parent), wins(0), visits(0) {}
};

class MCTS
{
public:
    MCTS(GameEngine &gameEngine, ContentType side, int simulations);
    ~MCTS();

    void run(std::unordered_map<Position, double, Position> &threadMoves);
    static void merge(std::unordered_map<Position, double, Position> &original, const std::unordered_map<Position, double, Position> &newMap);

private:
    double explore_constant = 1;
    double prev_rate = -1;

    GameEngine &gameEngine_;
    ContentType side_;
    int simulations_;
    Node *root_;

    Node *select(Node *node);
    void expand(Node *node);
    GameOutcome simulate(Node *node);
    void backpropagate(Node *node, GameOutcome outcome);
    double ucb1(Node *node);
    Position bestMove(Node *node);

    void deleteTree(Node *node);

    void printWinProbabilities(std::unordered_map<Position, double, Position> &threadMoves);
};

#endif
