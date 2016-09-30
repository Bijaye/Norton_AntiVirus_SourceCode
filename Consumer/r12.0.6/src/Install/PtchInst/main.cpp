#include <stdafx.h>

#define INITIIDS
#include "NAVPatchInst.h"

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_ENTRY(IID_PatchInst, CNAVPatchInst)
SYM_OBJECT_MAP_END()

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if(DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hModule);
    }

    return TRUE;
}

