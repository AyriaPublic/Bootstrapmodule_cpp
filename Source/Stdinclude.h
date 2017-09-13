/*
    Initial author: Convery (tcn@ayria.se)
    Started: 02-08-2017
    License: MIT
    Notes:
*/

#pragma once

// Includes for configuration settings.
#include "Configuration/Defines.h"
#include "Configuration/Macros.h"

// Includes for standard libraries.
#include <unordered_map>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <fstream>
#include <cstdio>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <ctime>

// Includes for platform libraries.
#if defined (_WIN32)
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

// Includes for thirdparty libraries.
/* ... */

// Includes for our utilities.
#include "Utility/Cryptography/FNV1Hash.h"
#include "Utility/Text/Variadicstring.h"
#include "Utility/Binary/Patternscan.h"
#include "Utility/Binary/Memprotect.h"
#include "Utility/Data/Ayriapackage.h"
#include "Utility/Data/Bytebuffer.h"
#include "Utility/Binary/Hooking.h"
#include "Utility/Data/SystemIO.h"
#include "Utility/Text/Logfile.h"
#include "Utility/Data/Base64.h"

// Includes for our components.
/* ... */
