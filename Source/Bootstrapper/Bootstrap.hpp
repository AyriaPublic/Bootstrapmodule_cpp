/*
    Initial author: Convery (tcn@ayria.se)
    Started: 26-12-2017
    License: MIT
*/

#pragma once
#include "../Stdinclude.hpp"

#if defined(_WIN32)

    // PE file properties.
    size_t GetPEEntrypoint();
    size_t GetTLSCallbackaddress();

    // TLS callback management.
    void RemoveTLS();
    void RestoreTLS();

    // Bootstrapping.
    void InstallPECallback();

#else

    // ELF file properties.
    size_t GetELFEntrypoint();

    // Bootstrapping.
    void InstallELFCallback();

#endif
