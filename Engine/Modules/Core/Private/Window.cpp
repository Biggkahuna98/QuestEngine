#include "Core/Window.h"

#include "SDL3_Window.h"
#include "Null_Window.h"

namespace Quest
{
    CORE_API Window* CreateWindow(std::string title, int width, int height, WindowType type)
    {
        if (type == WindowType::SDL3)
            return new SDLWindow(title, width, height);
        return new NullWindow();
    }
}