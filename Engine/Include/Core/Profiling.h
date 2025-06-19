#pragma once

#ifdef QE_DEBUG_MODE
#include <tracy/Tracy.hpp>
#define PROFILE_SCOPE(name) ZoneScopedN(name)
#define PROFILE_FUNC() ZoneScoped
#define PROFILE_MARK_FRAME() FrameMark
#else
#define PROFILE_SCOPE(name)
#define PROFILE_FUNC()
#define PROFILE_MARK_FRAME()
#endif
