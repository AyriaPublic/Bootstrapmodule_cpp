/*
    Initial author: Convery (tcn@ayria.se)
    Started: 29-07-2017
    License: MIT
    Notes:
        Module entrypoint.
*/

#include "Stdinclude.h"

// Delete the last sessions log on startup for windows.
#if defined (_WIN32)
    namespace { struct Deletelog { Deletelog() { Clearlog(); } }; static Deletelog Deleted{}; }
#endif

// Ensure that we have directories for the plugins.
#if defined (_WIN32)
    namespace {
        struct Startupfolders {
            Startupfolders()
            {
                _mkdir("./Plugins");
                _mkdir("./Plugins/Logs");
            }
        };
        static Startupfolders Loader{};
    }
#else
    namespace {
        struct Startupfolders {
            Startupfolders()
            {
                mkdir("./Plugins");
                mkdir("./Plugins/Logs");
            }
        };
        static Startupfolders Loader{};
    }
#endif

// Default entrypoint for windows.
#if defined (_WIN32)
BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    switch (nReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            // Rather not handle all thread updates.
            DisableThreadLibraryCalls(hDllHandle);
            break;
        }
    }

    return TRUE;
}
#endif
