#pragma once

#if defined(_WIN32)
    #define QE_PLATFORM_WINDOWS 1
#elif defined(__linux__)
    #define QE_PLATFORM_LINUX 1
#elif defined(__APPLE__)
    #define QE_PLATFORM_MACOS 1
#else
    #error "QuestEngine: unsupported platform"
#endif

#ifndef QE_PLATFORM_WINDOWS
    #define QE_PLATFORM_WINDOWS 0
#endif
#ifndef QE_PLATFORM_LINUX
    #define QE_PLATFORM_LINUX 0
#endif
#ifndef QE_PLATFORM_MACOS
    #define QE_PLATFORM_MACOS 0
#endif
