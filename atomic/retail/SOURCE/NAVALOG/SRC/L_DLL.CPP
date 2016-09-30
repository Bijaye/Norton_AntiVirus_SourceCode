#include "platform.h"
#include "dbcs.h"

HINSTANCE hDLLInst;

#ifdef SYM_WIN32

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

int WINAPI LibMain (                    // DLL initialization
   HINSTANCE hInst,                     // DLL instance
   WORD wDataSeg,                       // Local Data Segment
   WORD wHeapSize,                      // Local heap size
   LPTSTR lpszCmdLine);                  // Parameter line

int SYM_EXPORT WINAPI WEP(              // Windows' DLL exit procedure
   int nExitType);                      // Type is WEP_FREE_DLL or

// --------------------------------------------------------------------------

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
}


#endif

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

int WINAPI LibMain (                    // DLL initialization
   HINSTANCE hInst,                     // DLL instance
   WORD wDataSeg,                       // Local Data Segment
   WORD wHeapSize,                      // Local heap size
   LPTSTR lpszCmdLine)                   // Parameter line
{
                                        // Parameters not used; compiler
   wDataSeg; lpszCmdLine;               // cries. Make it shut up

#ifdef SYM_WIN16

   if (wHeapSize > 0)                   // No need to lock for
      UnlockData (0);                   //      non-real mode DLL's

#endif

   hDLLInst = hInst;                    // Save DLL's instance handle
   DBCSSetInternalTable ();             // Initialize DBCS
   return (1);                          // Return success to windows
}

// --------------------------------------------------------------------------
// --------------------------------------------------------------------------

int SYM_EXPORT WINAPI WEP(              // Windows' DLL exit procedure
   int nExitType)
{
   nExitType;
   return (1);
}
