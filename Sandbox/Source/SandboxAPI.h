#pragma once

#ifdef BUILD_DLL
#define SANDBOX_API __declspec(dllexport)
#else
//#define SANDBOX_API __declspec(dllimport)
#define SANDBOX_API
#endif