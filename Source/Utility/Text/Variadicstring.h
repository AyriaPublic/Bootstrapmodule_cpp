/*
    Initial author: Convery (tcn@ayria.se)
    Started: 29-07-2017
    License: MIT
    Notes:
        Creates a string from variadic input.
*/

#pragma once
#include "../../Stdinclude.h"

inline std::string va(std::string Format, ...)
{
    auto Resultbuffer = std::make_unique<char[]>(2049);
    std::va_list Varlist;

    // Create a new string from the arguments and truncate as needed.
    va_start(Varlist, Format);
    std::vsnprintf(Resultbuffer.get(), 2048, Format.c_str(), Varlist);
    va_end(Varlist);

    return std::string(Resultbuffer.get());
}
