// Reference: https://courses.cs.washington.edu/courses/cse573/04au/Project/mini1/RUSSIA/Final_Paper.pdf

#include <chrono>

#include <iostream>
#include <limits>

#include "Engine/GameEngine.h"

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

int getIntInput(const std::string &message, const std::string &errorMessage, int min, int max, const std::string &prompt = "Your input: ")
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
    std::cout << "\n";
    char input = getUserInput("Welcome to othello game!\nPlease choose your side: white (w) or black (b)", "Sorry, your input is invalid.");
    int diff = getIntInput("\nPlease enter the difficulty of the AI range from 1 to 100.\n\nWARNING: Computation time can be increased exponentially with repect to your input.\nWARNING: A number beyond 15 may take a long time to compute.\n\nIf you would like an Artificial Retard instead, please enter -1.", "Sorry, your input is invalid.", -1, 100);

    bool playerFirst = true;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

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

    while (true)
    {
        Position posForPrint;

        if (playerFirst)
        {
            GameOutcome progess = gameEngine->checkWin(false);
            if (progess != GameOutcome::IN_PROGRESS)
                break;

            posForPrint = gameEngine->playerTurn();
            if (posForPrint.getX() != -1 && posForPrint.getY() != -1)
            {
                staleCount = 0;
                std::cout << "You have put the piece on (" << posForPrint.getX() << ", " << posForPrint.getY() << ").\n";
            }

            progess = gameEngine->checkWin(false);
            if (progess != GameOutcome::IN_PROGRESS)
                break;
        }

        playerFirst = true;
        posForPrint = gameEngine->opponentTurn();
        std::cout << "\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";

        if (posForPrint.getX() != -1 && posForPrint.getY() != -1)
        {
            staleCount = 0;
            std::cout << "\nThe opponent has put the piece on (" << posForPrint.getX() << ", " << posForPrint.getY() << ").\n";
        }
        else
        {
            ++staleCount;
            std::cout << "\nThe opponent has no where to put the piece, skipping...\n\n";

            if (staleCount >= 2)
            {
                std::cout << "The game has reached a stalemate. Ending game early...\n\n";
                break;
            }
        }
    }

    GameOutcome result = gameEngine->checkWin(stale);
    gameEngine->printBoard();

    if (result == GameOutcome::DRAW)
    {
        std::cout << "\n\nIt is a draw.\n\n";
    }
    else if (result == GameOutcome::WHITE_WIN && gameEngine->getPlayerSide() == ContentType::WHITE)
    {
        std::cout << "\n\nYou win.\n\n";
    }
    else if (result == GameOutcome::BLACK_WIN && gameEngine->getPlayerSide() == ContentType::BLACK)
    {
        std::cout << "\n\nYou win.\n\n";
    }
    else
    {
        std::cout << "\n\nYou lose.\n\n";
    }

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    int64_t milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();

    int64_t seconds = milliseconds / 1000;
    milliseconds %= 1000;

    int64_t minutes = seconds / 60;
    seconds %= 60;

    int64_t hours = minutes / 60;
    minutes %= 60;

    std::cout << "This game took: " << hours << " hours, " << minutes << " minutes, " << seconds << " seconds, " << milliseconds << " milliseconds.\n\n";

    delete gameEngine;
    return 0;
}
