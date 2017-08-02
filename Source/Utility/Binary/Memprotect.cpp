/*
    Initial author: Convery (tcn@ayria.se)
    Started: 29-07-2017
    License: MIT
    Notes:
        Sets page permissions as needed for writing.
*/

#include "../../Stdinclude.h"

// Windows version of the memoryprotection.
#if defined (_WIN32)
#include <Windows.h>
void Memprotect::Protectrange(void *Address, const size_t Length, unsigned long Oldprotection)
{
    unsigned long Temp;
    VirtualProtect(Address, Length, Oldprotection, &Temp);
}
unsigned long Memprotect::Unprotectrange(void *Address, const size_t Length)
{
    unsigned long Oldprotection;
    VirtualProtect(Address, Length, PAGE_EXECUTE_READWRITE, &Oldprotection);
    return Oldprotection;
}

#else
#include <unistd.h>
#include <sys/mman.h>

// *nix version of the memoryprotection.
void Memprotect::Protectrange(void *Address, const size_t Length, unsigned long Oldprotection)
{
    int Pagesize = getpagesize();
    Address -= size_t(Address) % Pagesize;
    mprotect(Address, Pagesize, Oldprotection);
}
unsigned long Memprotect::Unprotectrange(void *Address, const size_t Length)
{
    unsigned long Oldprotection = (PROT_READ | PROT_WRITE | PROT_EXEC);

    // Get the old protection of the range, we assume it's continuous.
    std::FILE *Filehandle = std::fopen("/proc/self/maps", "r");
    if(Filehandle)
    {
        char Buffer[1024]{}, Permissions[5]{}, Device[6]{}, Mapname[256]{};
        unsigned long Start, End, Node, Foo;

        while(std::fgets(Buffer, 1024, Filehandle))
        {
            std::sscanf(Buffer, "%lx-%lx %4s %lx %5s %ld %s", &Start, &End, Permissions, &Foo, Device, &Node, Mapname);

            if(Start <= (unsigned long)Address || End >= (unsigned long)Address)
            {
                Oldprotection = 0;

                if(Permissions[0] == 'r') Oldprotection |= PROT_READ;
                if(Permissions[1] == 'w') Oldprotection |= PROT_WRITE;
                if(Permissions[2] == 'x') Oldprotection |= PROT_EXEC;

                break;
            }
        }

        std::fclose(Filehandle);
    }

    // Write the new protection.
    int Pagesize = getpagesize();
    Address -= size_t(Address) % Pagesize;
    mprotect(Address, Pagesize, PROT_READ | PROT_WRITE | PROT_EXEC);
    return Oldprotection;
}
#endif
