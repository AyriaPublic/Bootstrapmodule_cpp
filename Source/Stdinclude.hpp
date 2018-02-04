/*
    Initial author: Convery (tcn@hedgehogscience.com)
    Started: 22-12-2017
    License: MIT
    Notes:
        Provides a single include-file for all modules.
*/

#pragma once

// The configuration settings.
#include "Configuration/Defines.hpp"
#include "Configuration/Macros.hpp"

// Standard libraries.
#include <unordered_map>
#include <string_view>
#include <cstdint>
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <string>
#include <mutex>
#include <ctime>

// Platformspecific libraries.
#if defined(_WIN32)
    #include <Windows.h>
    #include <direct.h>
    #include <intrin.h>
    #undef min
    #undef max
#else
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/mman.h>
    #include <unistd.h>
    #include <dirent.h>
    #include <dlfcn.h>
#endif

// Utility modules.
#include "Utility/Variadicstring.hpp"
#include "Utility/Filesystem.hpp"
#include "Utility/Memprotect.hpp"
#include "Utility/Logfile.hpp"

// Components in this project.
#include "Bootstrapper/Bootstrap.hpp"
