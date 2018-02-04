/*
    Initial author: Convery (tcn@hedgehogscience.com)
    Started: 22-12-2017
    License: MIT
    Notes:
        Provides the entrypoint for Windows and Nix.
*/

#include "Stdinclude.hpp"

#if defined _WIN32
BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    switch (nReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            // Opt-out of further thread notifications.
            DisableThreadLibraryCalls(hDllHandle);

            // Ensure that the logfile directory exists.
            _mkdir("./Plugins/");
            _mkdir("./Plugins/Logs/");

            // Clear the previous sessions logfile.
            Clearlog();

            // Remove the TLS callback as it may cause issues.
            RemoveTLS();

            // Start bootstrapping.
            InstallPECallback();
        }
    }

    return TRUE;
}
#else
__attribute__((constructor)) void DllMain()
{
    // Ensure that DllMain is only called once.
    std::string_view Environment = getenv("LINUX_HACK");
    if(Environment.data()) return;
    setenv("LINUX_HACK", "1", 0);

    // Ensure that the logfile directory exists.
    mkdir("./Plugins/", S_IRWXU | S_IRWXG);
    mkdir("./Plugins/Logs/", S_IRUSR | S_IWUSR);

    // Clear the previous sessions logfile.
    Clearlog();

    // Start bootstrapping.
    InstallELFCallback();
}
#endif
