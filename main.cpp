// Reference: https://courses.cs.washington.edu/courses/cse573/04au/Project/mini1/RUSSIA/Final_Paper.pdf

#include <iostream>
#include <limits>

#include "Engine/GameEngine.h"
#include "Engine/Board.h"

char getUserInput(const std::string &message, const std::string &errorMessage, const std::string &prompt = "Your input: ")
{
    char input;
    std::cout << message << std::endl;
    std::cout << prompt;
    while (!(std::cin >> input) || !(input == 'b' || input == 'w'))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << errorMessage << std::endl;
        std::cout << prompt;
    }
    return input;
}

int getIntInput(const std::string &message, const std::string &errorMessage, float min, float max, const std::string &prompt = "Your input: ")
{
    int input;
    std::cout << message << std::endl;
    std::cout << prompt;
    while (!(std::cin >> input) || !(input >= min && input <= max) || input == 0)
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << errorMessage << std::endl;
        std::cout << prompt;
    }
    return input;
}

int main()
{
    char input = getUserInput("Welcome to othello game!\nPlease choose your side: white (w) or black (b)", "Sorry, your input is invalid.");
    int diff = getIntInput("\nPlease enter the difficulty of the AI range from 1 to 20.\n\nWARNING: Computation time can be increased exponentially with repect to your input.\nWARNING: A reasonable balance between difficulty and efficiency is around level 7-10.\n\nIf you would like an Artificial Retard instead, please enter -1.", "Sorry, your input is invalid.", -1, 20);

    bool playerFirst = true;

    GameEngine *gameEngine;
    if (input == 'w')
    {
        gameEngine = new GameEngine(ContentType::WHITE);
    }
    else
    {
        playerFirst = false;
        gameEngine = new GameEngine(ContentType::BLACK);
    }
    gameEngine->setDiff(diff);

    bool stale = false;
    int staleCount = 0;
    gameEngine->printBoard();

    while (true)
    {
        Position posForPrint;

        if (playerFirst)
        {
            GameOutcome progess = gameEngine->checkWin(false);
            if (progess != GameOutcome::IN_PROGRESS)
                break;

            posForPrint = gameEngine->playerTurn();
            if (posForPrint.x != -1 && posForPrint.y != -1)
            {
                staleCount = 0;
                std::cout << "You have put the piece on (" << posForPrint.x << ", " << posForPrint.y << ").\n";
                gameEngine->printBoard();
            }

            progess = gameEngine->checkWin(false);
            if (progess != GameOutcome::IN_PROGRESS)
                break;
        }

        playerFirst = true;
        posForPrint = gameEngine->opponentTurn();

        if (posForPrint.x != -1 && posForPrint.y != -1)
        {
            staleCount = 0;
            std::cout << "\nThe opponent has put the piece on (" << posForPrint.x << ", " << posForPrint.y << ").\n";
            gameEngine->printBoard();
        }
        else
        {
            ++staleCount;
            std::cout << "\nThe opponent has no where to put the piece, skipping...\n\n";
            gameEngine->printBoard();

            if (staleCount >= 2)
            {
                std::cout << "The game has reached a stalemate. Ending game early...\n\n";
                break;
            }
        }
    }

    GameOutcome result = gameEngine->checkWin(stale);

    if (result == GameOutcome::DRAW)
    {
        std::cout << "It is a draw.\n\n\n";
    }
    else if (result == GameOutcome::WHITE_WIN && gameEngine->getPlayerSide() == ContentType::WHITE)
    {
        std::cout << "You win.\n\n\n";
    }
    else if (result == GameOutcome::BLACK_WIN && gameEngine->getPlayerSide() == ContentType::BLACK)
    {
        std::cout << "You win.\n\n\n";
    }
    else
    {
        std::cout << "You lose.\n\n\n";
    }
    delete gameEngine;
    return 0;
}
