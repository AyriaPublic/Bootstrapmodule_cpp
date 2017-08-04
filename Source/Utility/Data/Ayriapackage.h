/*
    Initial author: Convery (tcn@ayria.se)
    Started: 04-08-2017
    License: MIT
    Notes:
        The Ayria package format is just a simple
        and uncompressed ZIP archive.
*/

#pragma once
#include "../../Stdinclude.h"

namespace Ayriapackage
{
    using Entry = std::pair<std::string, std::string>;
    using Archive = std::vector<Entry>;

    Archive Readarchive(std::string Filepath);
    const Entry *Find(std::string Query, const Archive &Collection);
}
