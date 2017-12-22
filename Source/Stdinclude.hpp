/*
    Initial author: Convery (tcn@ayria.se)
    Started: 22-12-2017
    License: MIT

    A single header for all modules used.
*/

#pragma once

// The configuration settings.
#include "Configuration/Defines.hpp"

// Standard libraries.
#include <string_view>
#include <cstdint>
#include <vector>

// Platformspecific libraries.
#if defined (_WIN32)
    #include <Windows.h>
    #include <direct.h>
    #include <intrin.h>
    #undef min
    #undef max
#else
#endif

// Utility modules.
