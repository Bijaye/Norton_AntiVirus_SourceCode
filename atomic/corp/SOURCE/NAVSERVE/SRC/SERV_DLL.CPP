#include "platform.h"

/////////////////////////////////////////////////////////////////////
// External function prototypes

VOID SYM_EXPORT WINAPI DeInitNLMMessenger (VOID);

/////////////////////////////////////////////////////////////////////
// Globals

HINSTANCE hInstance;

/////////////////////////////////////////////////////////////////////
// The code

int WINAPI LibMain (
    HINSTANCE   hInst,
    WORD        wDataSeg,
    WORD        wHeapSize,
    LPSTR       lpszCmdLine )
{
    hInstance = hInst;

#ifndef SYM_WIN32
    if (wHeapSize > 0)
        UnlockData (0);
#endif

    return (TRUE);
}

int SYM_EXPORT WINAPI WEP(int nParam)
{
                                        // Special code for our CNLMMessenger
                                        // object cleanup!
#ifdef SYM_WIN32
    DeInitNLMMessenger();
#endif

    return (TRUE);  // Return success to windows //
}


#ifdef	SYM_WIN32
BOOL WINAPI
DllMain (HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	BOOL	bResult = TRUE;

	switch (dwReason)
	{
		case	DLL_PROCESS_ATTACH:
			bResult = LibMain (hInstance, 0, 0, NULL);
			break;

		case	DLL_PROCESS_DETACH:
			WEP (0);
			break;
	}
	return bResult;
}
#endif
