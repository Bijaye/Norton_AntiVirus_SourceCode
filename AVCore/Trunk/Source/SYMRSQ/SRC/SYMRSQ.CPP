

#include "platform.h"
#include "version.h"		// DLL version checking routines
#include "format.h"
#include "symfmt.h"
#include "symsys.h"
#include "ngui.h"
#include "symrsq.h"


#ifdef  SYM_WIN


#ifdef __cplusplus
extern "C" {
#endif
	HINSTANCE hInst_SYMRSQ = NULL;
#ifdef __cplusplus
}
#endif




//HINSTANCE hInst_SYMFMT = NULL;			// These will point to the same instance
//HINSTANCE hInst_SYMSYS = NULL;			// of hInst_SYMRSQ; they will allow
//										// .c and .cpp files to access their
//										// instance handle as usual. (This is
//										// done to load resources.)
//#ifdef __cplusplus
//extern "C" {
//#endif
//HINSTANCE hInst_SYMFAT = NULL;
//#ifdef __cplusplus
//}
//#endif									// This particular handle is used in
//										// dirscan.c; need to tell compiler to
//										// treat it as C, NOT C++ .





#if defined(SYM_WIN32)


/****************************************************************************
   FUNCTION: DllMain

   PURPOSE:  Called when a new process is either using or not using
             this DLL anymore.

*******************************************************************************/
BOOL WINAPI DllMain(HANDLE hInstDLL, DWORD dwReason, LPVOID lpvReserved)
{
    hInst_SYMRSQ = (HINSTANCE) hInstDLL;

//										// Set the mock instance handles:
//	hInst_SYMFMT = (HINSTANCE) hInstDLL;			
//	hInst_SYMFAT = (HINSTANCE) hInstDLL;			
//	hInst_SYMSYS = (HINSTANCE) hInstDLL;			

    return(TRUE);
}

										// End SYM_WIN32
#else									// SYM_WIN16


/****************************************************************************
   FUNCTION: LibMain(HANDLE, WORD, WORD, LPSTR)

   PURPOSE:  Is called by LibEntry.  LibEntry is called by Windows when
             the DLL is loaded.  The LibEntry routine is provided in
             the LIBENTRY.OBJ in the SDK Link Libraries disk.  (The
             source LIBENTRY.ASM is also provided.)  
****************************************************************************/
int WINAPI LibMain(HINSTANCE hModule, UINT wDataSeg, UINT cbHeapSize, 
                       LPCSTR lpszCmdLine)
{
    hInst_SYMRSQ = hModule;

//								// Set the mock instance handles:
//Inst_SYMFMT = hModule;			
//Inst_SYMFAT = hModule;			
//Inst_SYMSYS = hModule;			

    if (cbHeapSize != 0)
        UnlockData(0);

    return (1);
}


/****************************************************************************
    FUNCTION: WEP(int)

    PURPOSE:  Performs cleanup tasks when the DLL is unloaded.  WEP() is
              called automatically by Windows when the DLL is unloaded (no
              remaining tasks still have the DLL loaded).  It is strongly
              recommended that a DLL have a WEP() function, even if it does
              nothing but returns success (1), as in this example.

****************************************************************************/
int SYM_EXPORT WINAPI WEP (int bSystemExit)
{
    return(1);
}


#endif									// End SYM_WIN16



/****************************************************************************
    FUNCTION: SYMRSQCheckVersion()

    PURPOSE:  A DLL or appliction which links to SYMRSQ calls this function
	      in order to ensure that the version of SYMRSQ it is linked to
              is late enough.
    
    INPUT:    lpExpected  expected version 
	      CallBack    function to call in case of mismatch
****************************************************************************/
void SYM_EXPORT WINAPI SYMRSQCheckVersion( LPVERREC lpExpected,
                                       CHECKVERCALLBACK_TYP CallBack )
{
    static char     	szFmt[] = "%2u.%2u.0.%2u";
    char            	szExpected[12];
    char            	szActual[12];
    static VERREC 	Actual = {
				   NULL,
				   SYMRSQ_VERSION_MAJOR,
				   SYMRSQ_VERSION_MINOR, 0,
				   SYMRSQ_BUILD_NUMBER,  0
                                 };

                                        // Check versions of DLLs I call.

    // SYMKRNLCheckVersion( &Actual, CallBack );

                                        // Check my own version.
    wsprintf(szExpected, szFmt,
                lpExpected->uMajor, lpExpected->uMinor, lpExpected->uBuild);
    wsprintf(szActual, szFmt, Actual.uMajor, Actual.uMinor, Actual.uBuild);

    if (STRICMP(szExpected, szActual) > 0)
        CallBack( hInst_SYMRSQ, lpExpected, &Actual );
    return;
}       



//////////////////////////////////////////////////////////////////////////
// CheckVersion
//
// This function is run-time linked and executed.  Note that all it does
// is call the static-link version.  That way, other DLLs can static-link
// to the static-link function, and we can still run-time link to a 
// known function name ("CheckVersion").
//
// Input:   lpExpected  expected version
//          CallBack    function to call in case of mismatch
//////////////////////////////////////////////////////////////////////////
void SYM_EXPORT WINAPI CheckVersion( LPVERREC lpExpected,
    CHECKVERCALLBACK_TYP CallBack )
{
    SYMRSQCheckVersion(lpExpected, CallBack);

    //SYMFATCheckVersion(lpExpected, CallBack);
    //SYMFMTCheckVersion(lpExpected, CallBack);
    //SYMSYSCheckVersion(lpExpected, CallBack);
}


#endif									// End SYM_WIN

