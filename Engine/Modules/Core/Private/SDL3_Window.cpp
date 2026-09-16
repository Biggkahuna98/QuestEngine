#include "SDL3_Window.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_keycode.h>
//#include <SDL3/SDL_main.h>

namespace Quest
{
    SDLWindow::SDLWindow(std::string title, int width, int height)
    {
        m_Title = title;
        m_Width = width;
        m_Height = height;

        SDL_Init(SDL_INIT_VIDEO);

        m_Window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

        if (m_Window == nullptr)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
            std::terminate();
        }
    }

    SDLWindow::~SDLWindow()
    {
        SDL_DestroyWindow(m_Window);
        SDL_Quit();
    }

    void* SDLWindow::GetNativeWindow()
    {
        return m_Window;
    }

    bool SDLWindow::ProcessEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE)
                    return false;
            }
        }

        return true;
    }

    void SDLWindow::PauseWindow()
    {
    }

    void SDLWindow::SetMouseInputProcessing(bool processing)
    {
    }
}
