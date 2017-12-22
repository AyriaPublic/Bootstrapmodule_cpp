/*
    Initial author: Convery (tcn@ayria.se)
    Started: 22-12-2017
    License: MIT
*/

#include "Stdinclude.hpp"

#if defined _WIN32
BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    // Opt-out of further thread notifications.
    DisableThreadLibraryCalls(hDllHandle);

    // Ensure that the logfile directory exists.
    _mkdir("./Plugins/");
    _mkdir("./Plugins/Logs/");

    return TRUE;
}
#else
__attribute__((constructor)) void DllMain()
{
    // Ensure that the logfile directory exists.
    mkdir("./Plugins/", S_IRWXU | S_IRWXG);
    mkdir("./Plugins/Logs/", S_IRUSR | S_IWUSR);
}
#endif
