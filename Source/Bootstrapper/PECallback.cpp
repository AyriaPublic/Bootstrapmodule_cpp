/*
    Initial author: Convery (tcn@hedgehogscience.com)
    Started: 08-01-2018
    License: MIT
    Notes:
        Hooks the games entrypoint and jumps back to it.
*/

#include "../Stdinclude.hpp"

#if defined(_WIN32)

extern "C" void Resumeprogram();
extern "C" size_t Entrypoint{};
uint8_t Originaltext[20]{};
void Callback();

// PE file properties.
size_t GetPEEntrypoint()
{
    // Module(NULL) gets the host application.
    HMODULE Modulehandle = GetModuleHandleA(NULL);
    if(!Modulehandle) return 0;

    // Traverse the PE header.
    PIMAGE_DOS_HEADER DOSHeader = (PIMAGE_DOS_HEADER)Modulehandle;
    PIMAGE_NT_HEADERS NTHeader = (PIMAGE_NT_HEADERS)((DWORD_PTR)Modulehandle + DOSHeader->e_lfanew);

    return (size_t)((DWORD_PTR)Modulehandle + NTHeader->OptionalHeader.AddressOfEntryPoint);
}

// Bootstrapping.
void InstallPECallback()
{
    Printfunction();

    Entrypoint = GetPEEntrypoint();
    if (!Entrypoint) return;

    auto Protection = Memprotect::Unprotectrange((void *)Entrypoint, 20);
    {
        // Take a backup of the text segment.
        std::memcpy(Originaltext, (void *)Entrypoint, 20);

        // Per architecture patching.
        #if defined(ENVIRONMENT64)
            *(uint8_t *)(Entrypoint + 0) = 0x48;                    // mov rax, Callback
            *(uint8_t *)(Entrypoint + 1) = 0xB8;                    // --
            *(uint64_t *)(Entrypoint + 2) = (uint64_t)Callback;     // --
            *(uint8_t *)(Entrypoint + 10) = 0xFF;                   // jmp rax
            *(uint8_t *)(Entrypoint + 11) = 0xE0;                   // --
        #else
            *(uint8_t *)(Entrypoint + 0) = 0xE9;                    // jmp short Callback
            *(uint32_t *)(Entrypoint + 1) = ((uint32_t)Callback - (Entrypoint + 5));
        #endif
    }
    Memprotect::Protectrange((void *)Entrypoint, 20, Protection);
}
void Restoreentrypoint()
{
    if (!Entrypoint) return;
    Printfunction();

    auto Protection = Memprotect::Unprotectrange((void *)Entrypoint, 20);
    {
        // Restore the text segment.
        std::memcpy((void *)Entrypoint, Originaltext, 20);
    }
    Memprotect::Protectrange((void *)Entrypoint, 20, Protection);
}

// The callback from the games entrypoint.
void Callback()
{
    Printfunction();

    // Remove our hackery.
    Restoreentrypoint();

    // Do what we came here for.
    Loadallplugins();

    // Resume execution at the entrypoint.
    Resumeprogram();
}

// CL x64 does not support inline assembly so this is defined in win64resume.asm
#if !defined(ENVIRONMENT64)
extern "C" void Resumeprogram()
{
    Printfunction();

    // Clang and CL implements this differently, bug?
    #if defined (__clang__)
        *((size_t*)__builtin_frame_address(0) + 1) = Entrypoint;
    #else
        *(size_t *)_AddressOfReturnAddress() = Entrypoint;
    #endif
}
#endif

#endif
