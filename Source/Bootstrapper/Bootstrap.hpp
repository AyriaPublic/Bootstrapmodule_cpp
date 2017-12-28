/*
    Initial author: Convery (tcn@ayria.se)
    Started: 26-12-2017
    License: MIT
*/

#pragma once
#include "../Stdinclude.hpp"

// Bootstrapper callbacks.
extern "C" void Resumeexecution();
void Bootstrapcallback();

#if defined(_WIN32)

    // PE file properties.
    size_t GetPEEntrypoint();
    size_t GetTLSCallbackaddress();

    // TLS callback management.
    void RemoveTLS();
    void RestoreTLS();

#else

    // ELF file properties.
    size_t GetELFEntrypoint();

#endif
