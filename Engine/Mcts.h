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

    explicit Node(Position move, Node *parent = nullptr)
        : move(move), parent(parent), wins(0), visits(0) {}
};

class MCTS
{
public:
    MCTS(GameEngine &gameEngine, ContentType side, int simulations);
    MCTS(const MCTS &other);
    ~MCTS();

    void run(std::unordered_map<Position, double, Position> &threadMoves);
    static void merge(std::unordered_map<Position, double, Position> &original, const std::unordered_map<Position, double, Position> &newMap);

    void operator=(const MCTS &other);

private:
    double explore_constant = 1;
    double prev_rate = -1;

    GameEngine &gameEngine_;
    ContentType side_;
    int simulations_;
    Node *root_;

    Node *select(Node *node);
    void expand(Node *node);
    GameOutcome simulate(const Node *node);
    void backpropagate(Node *node, GameOutcome outcome) const;
    double ucb1(const Node *node) const;
    // Position bestMove(Node *node);

    static void deleteTree(Node *node);

    void printWinProbabilities(std::unordered_map<Position, double, Position> &threadMoves);
};

#endif
