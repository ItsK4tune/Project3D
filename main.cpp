#include <iostream>
#include "global.h"
#include "game.h"

#define OPENGL_MAJOR 4
#define OPENGL_MINOR 6

int main()
{
    try
    {
        auto game = Game::Create(OPENGL_MAJOR, OPENGL_MINOR, "GLFW + GLAD");
        game->SetCallback();
        game->Initialize();
        game->MainLoop();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}