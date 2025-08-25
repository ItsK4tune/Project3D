#include <iostream>
#include "global.h"
#include "window.h"

#define OPENGL_MAJOR 4
#define OPENGL_MINOR 6

int main()
{
    try
    {
        auto window = Window::Create(OPENGL_MAJOR, OPENGL_MINOR, "GLFW + GLAD");
        window->SetCallback();
        window->Initialize();
        window->MainLoop();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}