/*
    Initial author: Convery (tcn@ayria.se)
    Started: 22-12-2017
    License: MIT
    Notes:
        Creates a readable string from variadic input.
*/

#pragma once
#include "../Stdinclude.hpp"

inline std::string va(std::string_view Format, ...)
{
    auto Resultbuffer = std::make_unique<char[]>(2048);
    std::va_list Varlist;

    // Create a new string from the arguments and truncate as needed.
    va_start(Varlist, Format);
    std::vsnprintf(Resultbuffer.get(), 2048, Format.data(), Varlist);
    va_end(Varlist);

    return std::move(Resultbuffer.get());
}
