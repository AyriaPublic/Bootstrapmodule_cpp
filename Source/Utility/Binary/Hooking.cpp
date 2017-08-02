/*
    Initial author: Convery (tcn@ayria.se)
    Started: 29-07-2017
    License: MIT
    Notes:
        A hook allows your plugin to take control over the game.
        It may replace a function in the game with your own.
*/

#include "../../Stdinclude.h"

// Restore the memory where the hook was placed.
bool Hooking::Stomphook::Removehook()
{
    auto Protection = Memprotect::Unprotectrange(Savedlocation, 20);
    {
        std::memcpy(Savedlocation, Savedcode, 20);
    }
    Memprotect::Protectrange(Savedlocation, 20, Protection);

    return true;
}
bool Hooking::Callhook::Removehook()
{
    auto Protection = Memprotect::Unprotectrange(Savedlocation, 20);
    {
        std::memcpy(Savedlocation, Savedcode, 20);
    }
    Memprotect::Protectrange(Savedlocation, 20, Protection);

    return true;
}

// Overwrite the games code with a redirection.
#if defined (ENVIRONMENT64)
bool Hooking::Stomphook::Installhook(void *Location, void *Target)
{
    Savedlocation = Location;
    Savedtarget = Target;

    auto Protection = Memprotect::Unprotectrange(Savedlocation, 20);
    {
        std::memcpy(Savedcode, Savedlocation, 20);

        *(uint8_t *)(uint64_t(Savedlocation) + 0) = 0x48;
        *(uint8_t *)(uint64_t(Savedlocation) + 1) = 0xB8;
        *(uint64_t *)(uint64_t(Savedlocation) + 2) = uint64_t(Target);
        *(uint8_t *)(uint64_t(Savedlocation) + 10) = 0xFF;
        *(uint8_t *)(uint64_t(Savedlocation) + 11) = 0xE0;
    }
    Memprotect::Protectrange(Savedlocation, 20, Protection);

    return true;
}
bool Hooking::Callhook::Installhook(void *Location, void *Target)
{
    Savedlocation = Location;
    Savedtarget = Target;

    auto Protection = Memprotect::Unprotectrange(Savedlocation, 20);
    {
        std::memcpy(Savedcode, Savedlocation, 20);

        *(uint8_t *)(uint64_t(Savedlocation) + 0) = 0x48;
        *(uint8_t *)(uint64_t(Savedlocation) + 1) = 0xB8;
        *(uint64_t *)(uint64_t(Savedlocation) + 2) = uint64_t(Target);
        *(uint8_t *)(uint64_t(Savedlocation) + 10) = 0xFF;
        *(uint8_t *)(uint64_t(Savedlocation) + 11) = 0xD0;
    }
    Memprotect::Protectrange(Savedlocation, 20, Protection);

    return true;
}

#else

bool Hooking::Stomphook::Installhook(void *Location, void *Target)
{
    Savedlocation = Location;
    Savedtarget = Target;

    auto Protection = Memprotect::Unprotectrange(Savedlocation, 20);
    {
        std::memcpy(Savedcode, Savedlocation, 20);

        *(uint8_t *)(uint32_t(Savedlocation) + 0) = 0xE9;
        *(uint32_t *)(uint32_t(Savedlocation) + 1) = uint32_t(Target) - uint32_t(Location) - 5;
    }
    Memprotect::Protectrange(Savedlocation, 20, Protection);

    return true;
}
bool Hooking::Callhook::Installhook(void *Location, void *Target)
{
    Savedlocation = Location;
    Savedtarget = Target;

    auto Protection = Memprotect::Unprotectrange(Savedlocation, 20);
    {
        std::memcpy(Savedcode, Savedlocation, 20);

        *(uint8_t *)(uint32_t(Savedlocation) + 0) = 0xE8;
        *(uint32_t *)(uint32_t(Savedlocation) + 1) = uint32_t(Target) - uint32_t(Location) - 5;
    }
    Memprotect::Protectrange(Savedlocation, 20, Protection);

    return true;
}
#endif
