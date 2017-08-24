/*
    Initial author: Convery (tcn@ayria.se)
    Started: 04-08-2017
    License: MIT
    Notes:
        The Ayria package format is just a simple
        and uncompressed ZIP archive.
*/

#include "../../Stdinclude.h"

namespace Ayriapackage
{
    #pragma pack(push, 1)
    struct Header
    {
        uint32_t Signature;
        uint16_t Version;
        uint16_t Flags;
        uint16_t Compression;
        uint16_t Modtime;
        uint16_t Moddate;
        uint32_t CRC32;
        uint32_t Compressedsize;
        uint32_t Uncompressedsize;
        uint16_t Filenamelength;
        uint16_t Extralength;
    };
    #pragma pack(pop)

    Archive Readarchive(std::string Filepath)
    {
        std::ifstream Filestream{ Filepath, std::ios::binary | std::ios::ate};
        Archive Collection{};

        do
        {
            // Open the archive.
            if (!Filestream.is_open()) break;

            // Get the archive size.
            auto Filesize = Filestream.tellg();
            Filestream.seekg(0, std::ios::beg);

            // Read the archive from disk.
            auto Filebuffer = std::make_unique<char[]>(size_t(Filesize));
            if(!Filestream.read(Filebuffer.get(), Filesize)) break;

            // Read each entry from the buffer.
            auto Iterator = Filebuffer.get();
            while (0 == std::memcmp(Iterator, "\x50\x4B\x03\x04", sizeof(uint32_t)))
            {
                // Read the entries header.
                Header Entryheader{ *(Header *)Iterator };
                Iterator += sizeof(Header);

                // Check that it's just stored.
                if (Entryheader.Compressedsize != Entryheader.Uncompressedsize)
                    break;

                // Read the rest of the entry.
                Entry Localentry;
                Localentry.first = std::string(Iterator, Entryheader.Filenamelength);
                Iterator += Entryheader.Filenamelength;
                Iterator += Entryheader.Extralength;
                Localentry.second = std::string(Iterator, Entryheader.Compressedsize);
                Iterator += Entryheader.Compressedsize;
                std::transform(Localentry.first.begin(), Localentry.first.end(), Localentry.first.begin(), tolower);
                Collection.push_back(Localentry);
            }
        } while (false);

        return Collection;
    }
    const Entry *Find(std::string Query, const Archive &Collection)
    {
        std::vector<std::string> Searchtokens;
        size_t Tokenposition = 0;

        // Tokenize the string for wildcards.
        std::transform(Query.begin(), Query.end(), Query.begin(), tolower);
        while (std::string::npos != (Tokenposition = Query.find('*')))
        {
            Searchtokens.push_back(Query.substr(0, Tokenposition));
            Query.erase(0, Tokenposition + 1);
        }
        Searchtokens.push_back(Query);

        // Iterate over the entries in the collection.
        for (auto &Item : Collection)
        {
            for (auto &Token : Searchtokens)
                if (!std::strstr(Item.first.c_str(), Token.c_str()))
                    goto LABEL_CONTINUE;

            return &Item;
            LABEL_CONTINUE:;
        }

        return nullptr;
    }
}

// Reenable the warning again.
#pragma warning(default: 4244)
