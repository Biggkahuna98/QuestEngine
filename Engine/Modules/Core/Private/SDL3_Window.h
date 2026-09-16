#pragma once

#include "Core/Window.h"

class SDL_Window;

namespace Quest
{
    class SDLWindow : public Window
    {
    public:
        SDLWindow(std::string title, int width, int height);
        ~SDLWindow() override;

        void* GetNativeWindow() override;
        bool ProcessEvents() override;
        void PauseWindow() override;
        void SetMouseInputProcessing(bool processing) override;
    private:
        SDL_Window* m_Window = nullptr;
    };
}