/*
    Initial author: Convery (tcn@ayria.se)
    Started: 29-07-2017
    License: MIT
    Notes:
        Sets page permissions as needed for writing.
*/

#pragma once
#include "../../Stdinclude.h"

namespace Memprotect
{
    void Protectrange(void *Address, const size_t Length, unsigned long Oldprotection);
    unsigned long Unprotectrange(void *Address, const size_t Length);
}
