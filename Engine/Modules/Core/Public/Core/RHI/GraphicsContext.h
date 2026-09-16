#pragma once

#include <string_view>

#include "Core/Core.h"
#include "GraphicsDevice.h"

namespace Quest
{
    enum class GraphicsAPI
    {
        Vulkan
    };

    class MessageCallback
    {
    public:
        enum class Severity
        {
            Info,
            Warning,
            Error
        };
        virtual ~MessageCallback() = default;
        virtual void Message(Severity level, std::string_view message) = 0;
        virtual void Info(std::string_view message) { Message(Severity::Info, message); }
        virtual void Warning(std::string_view message) { Message(Severity::Warning, message); }
        virtual void Error(std::string_view message) { Message(Severity::Error, message); }
    };

    struct ContextDesc
    {
        GraphicsAPI api = GraphicsAPI::Vulkan;
        MessageCallback* messageCallback = nullptr;
        Window* window = nullptr;
        bool enableAPISpecificValidation = true;
    };

    class GraphicsContext
    {
    public:
        virtual ~GraphicsContext() = default;
        virtual GraphicsDevice* CreateDevice(const DeviceDesc& desc) = 0;
    };
}

using CreateGraphicsContextFn = Quest::GraphicsContext* (*)(Quest::ContextDesc desc);
using DestroyGraphicsContextFn = void (*)(Quest::GraphicsContext* context);