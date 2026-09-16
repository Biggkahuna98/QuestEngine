#pragma once

#ifdef SANDBOX_INTERNAL_USE_ONLY
    #define SANDBOX_API __declspec(dllexport)
#else
    #define SANDBOX_API __declspec(dllimport)
#endif