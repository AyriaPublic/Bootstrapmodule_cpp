/*
    Initial author: Convery (tcn@ayria.se)
    Started: 26-12-2017
    License: MIT
    Notes:
        TLS callbacks are intended to run before
        main() and everytime a new thread is 
        created. This is used by packers.
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

    TLSBackup = Callback;
    *(size_t *)Address = 0;
}
void RestoreTLS()
{
    if(!TLSBackup) return;

    auto Address = GetTLSCallbackaddress();
    if(!Address) return;

    // Create a new thread to invoke TLS.
    *(size_t *)Address = TLSBackup;
    std::thread([]() { return 0; }).detach();
}

#endif

