/*
    Initial author: Convery (tcn@ayria.se)
    Started: 29-07-2017
    License: MIT
    Notes:
*/

#pragma once

// Includes for configuration settings.
#include "Configuration/Defines.h"
#include "Configuration/Macros.h"

// Includes for standard libraries.
#include <cstdint>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <vector>
#include <memory>
#include <chrono>
#include <mutex>
#include <ctime>

// Includes for thirdparty libraries.
/* ... */

// Includes for our utilities.
#include "Utility/Cryptography/FNV1Hash.h"
#include "Utility/Text/Variadicstring.h"
#include "Utility/Binary/Patternscan.h"
#include "Utility/Binary/Memprotect.h"
#include "Utility/Data/Bytebuffer.h"
#include "Utility/Binary/Hooking.h"
#include "Utility/Text/Logfile.h"
#include "Utility/Data/Base64.h"

// Includes for our components.
/* ... */
