/*
    Initial author: Convery (tcn@ayria.se)
    Started: 29-07-2017
    License: MIT
    Notes:
        Logs a message to disk.
*/

#pragma once
#include "../../Stdinclude.h"

// Configuration defines.
#if !defined(LOGFILEDIR) || !defined(MODULENAME)
    #define MODULENAME "Plugintemplate_cpp"
    #define LOGFILEDIR "./"
#endif

// The full URI for the output file.
namespace Logfileinternal
{
    constexpr const char *Outputpath = LOGFILEDIR MODULENAME ".log";
    static std::mutex Threadguard;
}

// Write a string to the logfile.
inline void Logprint(std::string Message)
{
    // Prevent multiple calls to fopen.
    Logfileinternal::Threadguard.lock();
    {
        // Append to the logfile.
        auto Filehandle = std::fopen(Logfileinternal::Outputpath, "a");
        if (Filehandle)
        {
            std::fputs(Message.c_str(), Filehandle);
            std::fputs("\n", Filehandle);
            std::fclose(Filehandle);
        }
    }
    Logfileinternal::Threadguard.unlock();

    // Duplicate the output to stderr if debugging.
    #if !defined (NDEBUG)
    std::fputs(Message.c_str(), stderr);
    std::fputs("\n", stderr);
    #endif
}

// Write a prefixed string.
inline void Logprefixed(std::string Message, std::string Prefix)
{
    Logprint(va("[%-8s] %s", Prefix.c_str(), Message.c_str()));
}
inline void Logtimestamped(std::string Message)
{
    auto Now = std::time(NULL);
    char Buffer[80]{};

    std::strftime(Buffer, 80, "%H:%M:%S", std::localtime(&Now));
    Logprefixed(Message, Buffer);
}

// Delete the log and create a new one.
inline void Clearlog()
{
    std::remove(Logfileinternal::Outputpath);
    Logtimestamped("Starting up..");
}
