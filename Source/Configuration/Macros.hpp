/*
    Initial author: Convery (tcn@ayria.se)
    Started: 28-12-2017
    License: MIT
*/

#pragma once

// Debug information logging.
#if defined(NDEBUG)
    #define Printfunction()
    #define Debugprint(string)
#else
    #define Printfunction() Logprefixed(__FUNCTION__, "  Trace")
    #define Debugprint(string) Logprefixed(string, "  Debug")
#endif

// General information.
#define Infoprint(string) Logprefixed(string, "  Info")
#define vaprint(format, ...) Logprint(va(format, __VA_ARGS__))
