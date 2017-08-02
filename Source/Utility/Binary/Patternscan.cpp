/*
    Initial author: Convery (tcn@ayria.se)
    Started: 29-07-2017
    License: MIT
    Notes:
        Scans through the hosts memory to find a pattern.
        We recommend to use the formatted version.
*/

#include "../../Stdinclude.h"

#if !defined (_WIN32)
extern char _etext, _end, __executable_start;
#endif

namespace Internal
{
    std::pair<size_t, size_t> Coderange;
    std::pair<size_t, size_t> Datarange;

    #if defined (_WIN32)
    #include <Windows.h>

    struct Rangeinitializer
    {
        Rangeinitializer()
        {
            HMODULE Module = GetModuleHandleA(NULL);
            if (!Module) return;

            PIMAGE_DOS_HEADER DOSHeader = (PIMAGE_DOS_HEADER)Module;
            PIMAGE_NT_HEADERS NTHeader = (PIMAGE_NT_HEADERS)((DWORD_PTR)Module + DOSHeader->e_lfanew);

            Coderange.first = size_t(Module) + NTHeader->OptionalHeader.BaseOfCode;
            Coderange.second = Coderange.first + NTHeader->OptionalHeader.SizeOfCode;

            Datarange.first = Coderange.second;
            Datarange.second = Datarange.first + NTHeader->OptionalHeader.SizeOfInitializedData;
        }
    };

#else
    
    struct Rangeinitializer
    {
        Rangeinitializer()
        {
            /*
                NOTE(Convery):
                This code is for linux, OSX may not support it.
                If it fails under OSX, try this:

                #include <mach-o/getsect.h>

                Coderange.first = size_t(???);
                Coderange.second = size_t(get_etext());

                Datarange.first = size_t(get_etext());
                Datarange.second = size_t(get_end());
            */

            Coderange.first = size_t(&__executable_start);
            Coderange.second = size_t(&_etext);
            Datarange.first = size_t(&_etext);
            Datarange.second = size_t(&_end);
        }
    };
#endif

    static Rangeinitializer Initializer{};
}

namespace Patternscan
{
    // Formatting, e.g. "00 04 EB 84 ? ? 32"
    Pattern Fromstring(std::string Input)
    {
        Pattern Result;

        // Iterate through the input.
        for (auto Iterator = Input.c_str(); *Iterator; ++Iterator)
        {
            // Ignore whitespace.
            if (*Iterator == ' ') continue;

            // Check for inactive bytes.
            if (*Iterator == '?')
            {
                Result.first.append(1, '\x00');
                Result.second.append(1, '\x00');
                continue;
            }

            // Grab two bytes from the input.
            Result.first.append(1, char(strtoul(std::string(Iterator, 2).c_str(), nullptr, 16)));
            Result.second.append(1, '\x01');
            Iterator++;
        }

        return Result;
    }
    std::string Tostring(Pattern Input)
    {
        std::string Result;

        // Iterate through the input.
        for (size_t i = 0; i < Input.first.size(); ++i)
        {
            // Check for inactive bytes.
            if (Input.second[i] == '\x00')
            {
                Result.append("? ");
                continue;
            }

            // Grab the byte.
            Result.append(va("%02hhX ", Input.first[i]));
        }

        return Result;
    }

    // Base functionality for the scanner.
    size_t Find(Pattern Input, size_t Rangestart, size_t Rangeend)
    {
        uint8_t Firstbyte = Input.first[0];

        // We do not handle masks that start with an invalid byte.
        if (Input.second[0] == '\x00') return 0;

        // Iterate over the specified range.
        for (; Rangestart < Rangeend; ++Rangestart)
        {
            // Skip irrelevant bytes.
            if (*(uint8_t *)Rangestart != Firstbyte) continue;

            // Compare the data to the pattern.
            if ([](size_t Address, Pattern &Input)
            {
                for (size_t i = 1; i < Input.first.size(); ++i)
                {
                    // Skip inactive bytes.
                    if (Input.second[i] == '\x00') continue;

                    // Break on an invalid compare.
                    if (*(char *)(Address + i) != Input.first[i])
                        return false;
                }

                return true;

            }(Rangestart, Input)) return Rangestart;
        }

        // Invalid address.
        return 0;
    }
    std::vector<size_t> Findall(Pattern Input, size_t Rangestart, size_t Rangeend)
    {
        std::vector<size_t> Result;
        size_t Address = Rangestart;

        do
        {
            Address = Find(Input, Address, Rangeend);
            if (Address) Result.push_back(Address++);

        } while (Address);

        return Result;
    }

    // Built-in ranges for the application.
    size_t FindCode(Pattern Input)
    {
        return Find(Input, Internal::Coderange.first, Internal::Coderange.second);
    }
    size_t FindData(Pattern Input)
    {
        return Find(Input, Internal::Datarange.first, Internal::Datarange.second);
    }
    std::vector<size_t> FindallCode(Pattern Input)
    {
        return Findall(Input, Internal::Coderange.first, Internal::Coderange.second);
    }
    std::vector<size_t> FindallData(Pattern Input)
    {
        return Findall(Input, Internal::Datarange.first, Internal::Datarange.second);
    }
}
