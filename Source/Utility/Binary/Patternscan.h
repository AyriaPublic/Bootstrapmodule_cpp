/*
    Initial author: Convery (tcn@ayria.se)
    Started: 29-07-2017
    License: MIT
    Notes:
        Scans through the hosts memory to find a pattern.
        We recommend to use the formatted version.
*/

#pragma once
#include "../../Stdinclude.h"

namespace Patternscan
{
    using Pattern = std::pair<std::string /* Data */, std::string /* Mask */>;

    // Base functionality for the scanner.
    size_t Find(Pattern Input, size_t Rangestart, size_t Rangeend);
    std::vector<size_t> Findall(Pattern Input, size_t Rangestart, size_t Rangeend);

    // Built-in ranges for the application.
    size_t FindCode(Pattern Input);
    size_t FindData(Pattern Input);
    std::vector<size_t> FindallCode(Pattern Input);
    std::vector<size_t> FindallData(Pattern Input);

    // Formatting, e.g. "00 04 EB 84 ? ? 32"
    Pattern Fromstring(std::string Input);
    std::string Tostring(Pattern Input);
}
