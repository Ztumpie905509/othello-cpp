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
    while (!(std::cin >> input) || !(input >= min && input <= max))
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
    int diff = getIntInput("\nPlease enter the difficulty of the AI range from 1 to 7.\nWARNING: Computation time can be increased exponentially with repect to your input.\n\nIf you would like an Artificial Retard instead, please enter 0.", "Sorry, your input is invalid.", 0, 7);

    GameEngine *gameEngine;
    if (input == 'w')
    {
        gameEngine = new GameEngine(ContentType::WHITE);
    }
    else
    {
        gameEngine = new GameEngine(ContentType::BLACK);
    }
    gameEngine->setDiff(diff);

    bool stale = false;
    int staleCount = 0;
    bool skipPlayer;
    input == 'w' ? skipPlayer = false : skipPlayer = true;
    gameEngine->printBoard();
    gameEngine->printAdditionalInfo();

    while (true)
    {
        Position posForPrint;

        if (!skipPlayer)
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

        skipPlayer = false;
        posForPrint = gameEngine->opponentTurn();
        if (posForPrint.x != -1 && posForPrint.y != -1)
        {
            staleCount = 0;
            std::cout << "\nThe opponent has put the piece on (" << posForPrint.x << ", " << posForPrint.y << ").\n";
            gameEngine->printBoard();
            gameEngine->printAdditionalInfo();
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
    gameEngine->printBoard();

    if (result == GameOutcome::WHITE_WIN && gameEngine->getPlayerSide() == ContentType::WHITE)
    {
        std::cout << "You win.\n\n\n";
    }
    else if (result == GameOutcome::BLACK_WIN && gameEngine->getPlayerSide() == ContentType::BLACK)
    {
        std::cout << "You win.\n\n\n";
    }
    else if (result == GameOutcome::DRAW)
    {
        std::cout << "It is a draw.\n\n\n";
    }
    else
    {
        std::cout << "You lose.\n\n\n";
    }
    delete gameEngine;
    return 0;
}
