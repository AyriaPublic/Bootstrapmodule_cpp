/*
    Initial author: Convery (tcn@ayria.se)
    Started: 02-08-2017
    License: MIT
    Notes:
        PE files have this 'interesting' feature to call a
        function everytime a thread starts. This is bad as
        it's often used to do integrity checking of the EP.
*/

#include "../Stdinclude.h"

#if defined (_WIN32)
#include <Windows.h>

// Backup of the TLS callback.
size_t OriginalTLS{};

// Find the active callback if available.
size_t GetTLSCallback()
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

// Remove and restore TLS.
void RemoveTLS()
{
    auto Address = GetTLSCallback();
    if(!Address) return;

    auto Callback = *(size_t *)Address;
    if(!Callback) return;

    OriginalTLS = Callback;
    *(size_t *)Address = 0;
}
void RestoreTLS()
{
    if(!OriginalTLS) return;

    auto Address = GetTLSCallback();
    if(!Address) return;

    // Create a new thread to invoke TLS.
    *(size_t *)Address = OriginalTLS;
    std::thread([]() { return 0; }).detach();
}

// Delay TLS calls on startup.
namespace { struct DelayTLS { DelayTLS() { RemoveTLS(); } }; static DelayTLS Delay{}; }

#else

void RestoreTLS() {}

#endif
