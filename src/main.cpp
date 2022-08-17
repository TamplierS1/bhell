#include <iostream>

#include "game.h"

int main()
{
    try
    {
        Game game;
        game.run();
    }
    catch (std::exception& e)
    {
        std::cout << e.what();
    }

    return EXIT_SUCCESS;
}