/*
    Initial author: Convery (tcn@ayria.se)
    Started: 29-07-2017
    License: MIT
    Notes:
        Configuration defines that are used throughout the module.
*/

#pragma once

// As Visual Studio 2015 no longer defines NDEBUG, we'll have to.
#if !defined(_DEBUG) && !defined(NDEBUG)
#define NDEBUG
#endif

// Platform attributes.
#if defined(_MSC_VER)
    #if defined(_WIN64)
        #define ENVIRONMENT64
    #endif
    #define EXPORT_ATTR __declspec(dllexport)
    #define IMPORT_ATTR __declspec(dllimport)
#elif defined (__GNUC__)
    #if defined(__x86_64__) || defined(__ppc64__)
        #define ENVIRONMENT64
    #endif
    #define EXPORT_ATTR __attribute__((visibility("default")))
    #define IMPORT_ATTR
#else
    #define EXPORT_ATTR
    #define IMPORT_ATTR
    #error Compiling for unknown platform.
#endif

// Logging functions save to this directory.
#define LOGFILEDIR "./Plugins/Logs/"

// The name that will be used in default functions.
#define MODULENAME "Nativebootstrap"

// Disable windows annoyance.
#if defined (_WIN32)
    #define _CRT_SECURE_NO_WARNINGS
#endif
