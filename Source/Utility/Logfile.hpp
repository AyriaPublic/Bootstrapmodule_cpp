/*
    Initial author: Convery (tcn@ayria.se)
    Started: 22-12-2017
    License: MIT

    Prints to the logfile and STDERR for debugging.
*/

#pragma once
#include "Variadicstring.hpp"
#include "../Stdinclude.hpp"

// Internal state.
namespace
{
    #if !defined(MODULENAME)
    #define MODULENAME "Invalid"
    #endif

    constexpr const char *Filepath = "./Plugins/Logs/" MODULENAME ".log";
    static std::mutex Threadguard;
}

// Output to file.
inline void Logprint(std::string_view Message)
{
    // Prevent multiple writes to the file.
    Threadguard.lock();
    {
        // Append to the logfile.
        auto Filehandle = std::fopen(Filepath, "a");
        if (Filehandle)
        {
            std::fputs(Message.data(), Filehandle);
            std::fputs("\n", Filehandle);
            std::fclose(Filehandle);
        }
    }
    Threadguard.unlock();

    // Duplicate the message to STDERR.
    #if !defined(NDEBUG)
    std::fputs(Message.data(), stderr);
    std::fputs("\n", stderr);
    #endif
}
inline void Logprintasync(std::string Message)
{
    std::thread([&](){ Logprint(Message);}).detach();
}

// Formatted output.
inline void Logprefixed(std::string_view Message, std::string_view Prefix)
{
    Logprint(va("[%-8s] %s", Prefix.data(), Message.data()));
}
inline void Logtimestamped(std::string_view Message)
{
    auto Now = std::time(NULL);
    char Buffer[80]{};

    std::strftime(Buffer, 80, "%H:%M:%S", std::localtime(&Now));
    Logprefixed(Message, Buffer);
}

// Delete the log and create a new one.
inline void Clearlog()
{
    std::remove(Filepath);
    Logtimestamped(MODULENAME " - Starting up..");
}
