#pragma once

#include "Core/CoreExport.h"

#include <string>

namespace Quest
{
    enum class WindowType
    {
        Null,
        SDL3
    };

    class CORE_API Window
    {
    public:
        virtual ~Window() = default;

        int GetWidth() const { return m_Width; };
        int GetHeight() const { return m_Height; };
        bool GetIsMinimized() const { return m_IsMinimized; };
        bool GetIsProcessingMouseInput() const { return m_IsProcessingMouseInput; };

        virtual void* GetNativeWindow() = 0;
        virtual bool ProcessEvents() = 0; // returns whether the window should exit
        virtual void PauseWindow() = 0;

        virtual void SetMouseInputProcessing(bool processing) = 0;
    protected:
        WindowType m_Type = WindowType::SDL3;
        std::string m_Title = "Quest Engine";
        int m_Width = 0;
        int m_Height = 0;
        bool m_IsMinimized = false;
        bool m_IsProcessingMouseInput = true;
    };

    CORE_API Window* CreateWindow(std::string title, int width, int height, WindowType type = WindowType::SDL3);
}