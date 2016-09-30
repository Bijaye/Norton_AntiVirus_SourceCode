
#include "platform.h"
#include "symkrnl.h"
#include "npt.h"
#include "sumisc.h"
#include "symguii.h"

#ifdef SYM_WIN32

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
        SymKRNLAttach(hInstance, dwReason, lpReserved);
        SymUTILAttach(hInstance, 0, 0, NULL);
        SymGUIAttach(hInstance, 0, 0, NULL);
        SymNPTAttach(hInstance, 0, 0, NULL);
	    break;

	case DLL_PROCESS_DETACH:
        SymKRNLAttach(hInstance, dwReason, lpReserved);
        SymGUIDetach(0);
	    break;
	}

    return (TRUE);
}

#else // #ifdef SYM_WIN32

/****************************************************************************
   FUNCTION: LibMain(HANDLE, WORD, WORD, LPSTR)

   PURPOSE:  Is called by LibEntry.  LibEntry is called by Windows when
             the DLL is loaded.  The LibEntry routine is provided in
             the LIBENTRY.OBJ in the SDK Link Libraries disk.  (The
             source LIBENTRY.ASM is also provided.)
             the initialization is successful.

*******************************************************************************/

#pragma optimize("", off)	            // required for thunking to work
int WINAPI LibMain (
    HINSTANCE   hInstance,
    UINT        wDataSeg,
    UINT        cbHeapSize,
    LPSTR       lpszCmdLine
    )
{
    SymKRNLAttach ( hInstance );
    SymUTILAttach ( hInstance, wDataSeg, cbHeapSize, lpszCmdLine );
    SymGUIAttach  ( hInstance, wDataSeg, cbHeapSize, lpszCmdLine );

    if (cbHeapSize != 0)
        UnlockData(0);

    return ( TRUE );
}

/****************************************************************************
    FUNCTION:  WEP(int)

    PURPOSE:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
              called automatically by Windows when the DLL is unloaded (no
              remaining tasks still have the DLL loaded).  It is strongly
              recommended that a DLL have a WEP() function, even if it does
              nothing but returns success (1), as in this example.

*******************************************************************************/

EXTERNC int SYM_EXPORT WINAPI WEP (
    int     bSystemExit
    )
{
    SymKRNLDetach();
    SymGUIDetach ( bSystemExit );

    return ( TRUE );
}

#endif

