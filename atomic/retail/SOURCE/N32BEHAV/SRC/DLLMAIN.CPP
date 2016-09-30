////////////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32BEHAV/VCS/Dllmain.cpv   1.0   23 May 1997 12:43:24   DBUCHES  $
////////////////////////////////////////////////////////////////////////////////////
//
//  DLLMAIN.CPP - Contains DllMain() function implementation for 
//                Norton AntiVirus Behavior DLL
//
//
////////////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32BEHAV/VCS/Dllmain.cpv  $
// 
//    Rev 1.0   23 May 1997 12:43:24   DBUCHES
// Initial revision.
////////////////////////////////////////////////////////////////////////////////////
#include "windows.h"
#include "stdlib.h"
#include "n32behav.h"


////////////////////////////////////////////////////////////////////////////////////
// Local functions
void SortFlags();
int compare( const void *arg1, const void *arg2 );


////////////////////////////////////////////////////////////////////////////////////
// Local Data
int giNumFlags;                         // Number of elements in our array.


////////////////////////////////////////////////////////////////////////////////////
// DLLMain() - Dll entry point
////////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{

    switch( dwReason ) 
        {
        case DLL_PROCESS_ATTACH:
            SortFlags();
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
        }

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
// SortFlags() - This funcion sorts our array of flags for quick lookups
////////////////////////////////////////////////////////////////////////////////////
void SortFlags()
{
    // Get number of elements
    giNumFlags = 0;
    while( gFlags[++giNumFlags] != 0 )
        ;

    // Sort array of flags.
    qsort( (void *) gFlags, giNumFlags, sizeof( DWORD ), compare );
}

////////////////////////////////////////////////////////////////////////////////////
// compare() - Comparision helper function for qsort() and bsearch()
////////////////////////////////////////////////////////////////////////////////////
int compare( const void *arg1, const void *arg2 )
{
    int iRet;
    DWORD dw1 = *((DWORD*)arg1);
    DWORD dw2 = *((DWORD*)arg2);

    if( dw1 < dw2 )
        iRet = -1;
    else if( dw1 == dw2 )
        iRet = 0;
    else if( dw1 > dw2 )
        iRet = 1;

    return iRet;
}


////////////////////////////////////////////////////////////////////////////////////
// GetBehavior() - Check for supported behavior
//
// Input:   dwBehavior -  Behavior ID to search for.
//          dwExtra - Reserved for future expansion.
//
// Returns: TRUE if behavior is supported, 
//          FALSE otherwise
//
////////////////////////////////////////////////////////////////////////////////////
// DBUCHES: Created 5/22/97
////////////////////////////////////////////////////////////////////////////////////
BOOL DECLSPEC_N32BEHAV GetBehavior(DWORD dwBehavior, DWORD dwExtra)
{
    // For future expansion.
    UNREFERENCED_PARAMETER(dwExtra);

    // Search for requested key;
    LPVOID pRet = bsearch( &dwBehavior, gFlags, giNumFlags, sizeof(DWORD), compare);

    // Did we find it?
    if(pRet == NULL)
        return FALSE;

    // Found it.
    return TRUE;
}
