/*
    Initial author: Convery (tcn@ayria.se)
    Started: 22-12-2017
    License: MIT

    Global configurations, defaults.
*/

#pragma once

// Identifying name for this module.
#define MODULENAME "Nativebootstrap"

// Fixup for Visual Studio 2015 no longer defining this.
#if !defined(_DEBUG) && !defined(NDEBUG)
#define NDEBUG
#endif

// Platform identification.
#if defined(_MSC_VER)
    #define EXPORT_ATTR __declspec(dllexport)
    #define IMPORT_ATTR __declspec(dllimport)
#elif defined(__GNUC__)
    #define EXPORT_ATTR __attribute__((visibility("default")))
    #define IMPORT_ATTR
#else
    #define EXPORT_ATTR
    #define IMPORT_ATTR
    #error Compiling for unknown platform.
#endif

// Environment identification.
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
    #define ENVIRONMENT64
#endif
