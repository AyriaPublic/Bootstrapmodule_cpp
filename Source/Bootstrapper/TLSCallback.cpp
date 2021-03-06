/*
    Initial author: Convery (tcn@hedgehogscience.com)
    Started: 26-12-2017
    License: MIT
    Notes:
        Temporarily removes the TLS callback to
        prevent packers from messing with init.
*/

#include "../Stdinclude.hpp"

#if defined(_WIN32)

size_t TLSBackup{};

// Read the address from the PE file.
size_t GetTLSCallbackaddress()
{
    // Module(NULL) gets the host application.
    HMODULE Modulehandle = GetModuleHandleA(NULL);
    if(!Modulehandle) return 0;

    // Traverse the PE header.
    PIMAGE_DOS_HEADER DOSHeader = (PIMAGE_DOS_HEADER)Modulehandle;
    PIMAGE_NT_HEADERS NTHeader = (PIMAGE_NT_HEADERS)((DWORD_PTR)Modulehandle + DOSHeader->e_lfanew);
    IMAGE_DATA_DIRECTORY TLSDirectory = NTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];

    // Get the callback address.
    if(!TLSDirectory.VirtualAddress) return 0;
    return size_t(((size_t *)((DWORD_PTR)Modulehandle + TLSDirectory.VirtualAddress))[3]);
}

void RemoveTLS()
{
    auto Address = GetTLSCallbackaddress();
    if(!Address) return;

    auto Callback = *(size_t *)Address;
    if(!Callback) return;

    auto Protection = Memprotect::Unprotectrange((void *)Address, 8);
    {
        TLSBackup = Callback;
        *(size_t *)Address = 0;
    }
    Memprotect::Protectrange((void *)Address, 8, Protection);
}
void RestoreTLS()
{
    if(!TLSBackup) return;

    auto Address = GetTLSCallbackaddress();
    if(!Address) return;

    auto Protection = Memprotect::Unprotectrange((void *)Address, 8);
    {
        *(size_t *)Address = TLSBackup;
    }
    Memprotect::Protectrange((void *)Address, 8, Protection);

    // Create a new thread to invoke TLS.
    std::thread([]() { return 0; }).detach();
}

#endif

