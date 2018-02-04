/*
    Initial author: Convery (tcn@hedgehogscience.com)
    Started: 26-12-2017
    License: MIT
    Notes:
        Forward declarations for the bootstrapping.
*/

#pragma once
#include "../Stdinclude.hpp"

// Load all plugins from the default directory.
void Loadallplugins();

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
