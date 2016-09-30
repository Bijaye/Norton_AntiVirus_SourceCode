#include "platform.h"
#include "options.h"
#include "security.h"

HINSTANCE hInstance;

#if defined(SYM_WIN32)

// ***************************************************************************

extern "C" int WINAPI LibMain (
    HINSTANCE hInst,
    WORD wDataSeg,
    WORD wHeapSize,
    LPSTR lpszCmdLine);

extern "C" int SYM_EXPORT WINAPI WEP (
    int bSystemExit);

/****************************************************************************
   FUNCTION: DllMain

   PURPOSE:  Called when a new process is either using or not using
             this DLL anymore.

*******************************************************************************/
BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved)
{
    BOOL        bResult = TRUE;

    switch (dwReason)
        {
        case DLL_PROCESS_ATTACH:
            bResult = LibMain(hInstDLL, 0, 0, NULL);
            break;

        case DLL_PROCESS_DETACH:
            WEP(0);
            break;
        }   // switch

    return (bResult);
}   //  DllMain


#endif


extern "C" int WINAPI LibMain (
    HINSTANCE hInst,
    WORD wDataSeg,
    WORD wHeapSize,
    LPSTR lpszCmdLine)
{
#ifdef SYM_WIN16
    if (wHeapSize != 0)
        UnlockData(0);
#endif
    hInstance = hInst;

    return (1);
}

extern "C" int SYM_EXPORT WINAPI WEP (int bSystemExit)
{
    return(1);
}

