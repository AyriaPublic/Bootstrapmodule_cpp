/*
    Initial author: Convery (tcn@ayria.se)
    Started: 03-08-2017
    License: MIT
    Notes:
        Inserts a hook on the games entrypoint.
        PE hooks the entrypoint to support .NET.
*/

#include "../Stdinclude.h"

#if defined (_WIN32)
#include <Windows.h>

// Global scope to be accessable from ASM.
uint8_t Originaltext[20]{};
size_t Entrypoint{};
void Callback();

// Access to external modules.
extern "C" void Resumeprogram();
extern void Loadallplugins();
extern void RestoreTLS();

// Get the games entrypoint.
size_t Getentrypoint()
{
    // Module(NULL) gets the host application.
    HMODULE Modulehandle = GetModuleHandleA(NULL);
    if(!Modulehandle) return 0;

    // Traverse the PE header.
    PIMAGE_DOS_HEADER DOSHeader = (PIMAGE_DOS_HEADER)Modulehandle;
    PIMAGE_NT_HEADERS NTHeader = (PIMAGE_NT_HEADERS)((DWORD_PTR)Modulehandle + DOSHeader->e_lfanew);

    return (size_t)((DWORD_PTR)Modulehandle + NTHeader->OptionalHeader.AddressOfEntryPoint);
}

// Windows x64 defines this in a .asm file.
#if defined (ENVIRONMENT64)
extern "C" void Resumeprogram()
{
    // Clang and CL does not want to cooperate here.
    #if defined (__clang__)
        *((size_t*)__builtin_frame_address(0) + 1) = Entrypoint);
    #else
        *(size_t *)_AddressOfReturnAddress() = Entrypoint);
    #endif
}
#endif

// The callback from the games entrypoint.
void Callback()
{
    Printfunction();

    // Remove our hackery.
    Restoreentrypoint();
    RestoreTLS();

    // Do what we came here for.
    Loadallplugins();

    // Resume execution at the entrypoint.
    Resumeprogram();
}

// Install the hook on startup.
namespace { struct Overwriteentrypoint { Overwriteentrypoint() { Removeentrypoint(); }; }; static Overwriteentrypoint Loader{}; }


#endif
