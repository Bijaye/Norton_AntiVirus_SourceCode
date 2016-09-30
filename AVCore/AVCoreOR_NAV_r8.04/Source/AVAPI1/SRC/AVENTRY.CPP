
#if defined(SYM_WIN)

#include "platform.h"

HINSTANCE hInstance;

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI LibMain (
    HINSTANCE hInst,
    WORD wDataSeg,
    WORD wHeapSize,
    LPSTR lpszCmdLine );

int WINAPI WEP (
    int nParam );

// ---------------------------------------------------------------------------

#if defined(SYM_WIN32)

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

BOOL WINAPI DllMain (
    HINSTANCE   hInstDLL,
    DWORD       dwReason,
    LPVOID      lpvReserved )
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

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI LibMain (HINSTANCE hInst, WORD wDataSeg, WORD wHeapSize,
                        LPSTR lpszCmdLine)
{
   hInstance = hInst;

#ifdef SYM_WIN16

   if (wHeapSize > 0)
      UnlockData (0);

#endif

   return (1);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI WEP(int nParam)
{
   return(1);  // Return success to windows //
}

#endif

