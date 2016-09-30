//************************************************************************
//
// $Header:   S:/NAVEX/VCS/naventry.cpv   1.16   15 Dec 1998 12:16:02   DCHI  $
//
// Description:
//      Contains AVEX DLL entry code for Windows platforms.
//
// Notes:
//      Should only be part of Windows compilations.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/naventry.cpv  $
// 
//    Rev 1.16   15 Dec 1998 12:16:02   DCHI
// Added call to NAVEXInfCheck().
// 
//    Rev 1.15   09 Dec 1998 17:45:38   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.14   08 Dec 1998 13:00:50   DCHI
// Changes for MSX.
// 
//    Rev 1.13   10 Nov 1998 13:09:36   rpulint
// re-checkin the changes made for OS/2 platform
// 
//    Rev 1.11   06 Nov 1998 17:35:50   rpulint
// Added changes for OS/2 platform
// 
//    Rev 1.10   05 Nov 1997 12:04:18   DCHI
// Added retrieval of startup directory for WIN16.
// 
//    Rev 1.9   04 Nov 1997 15:44:44   CNACHEN
// properly includes navheur1.h which is checked in.
// 
//    Rev 1.8   04 Nov 1997 11:16:48   JWILBER
// Commented out reference to navheur1.h until it's checked in.
//
//    Rev 1.7   03 Nov 1997 18:11:48   CNACHEN
// Added Bloodhound support for Picasso.
//
//    Rev 1.6   19 Jun 1997 15:25:52   DCHI
// Added definition of gbInWindows BOOL and initialization in LibMain().
//
//    Rev 1.5   18 Apr 1997 17:57:28   AOONWAL
// Modified during DEV1 malfunction
//
//    Rev ABID  16 Apr 1997 19:45:30   DCHI
// Changes so that mvpdef.dat is only loaded from NAV directory.
//
//    Rev 1.4   07 Apr 1997 18:11:02   DCHI
// Added MVP support.
//
//    Rev 1.3   26 Dec 1996 15:22:24   AOONWAL
// No change.
//
//    Rev 1.2   02 Dec 1996 14:00:18   AOONWAL
// No change.
//
//    Rev 1.1   29 Oct 1996 12:59:12   AOONWAL
// No change.
//
//    Rev 1.0   13 Oct 1995 13:04:42   DCHI
// Initial revision.
//
//************************************************************************

#include "gdefines.h"

#ifdef SYM_OS2

#include "platform.h"
#include "mvp.h"

#include "ctsn.h"
#include "callback.h"
#include "navex.h"

#include "heurapi.h"
#include "navheur1.h"
#include "callfake.h"
void InitEXTEng(void);
extern   void LoadHeuristics(LPVOID);
extern "C" int __hmodule;

unsigned
long
LibMain(HMODULE mod_handle,
		unsigned long Start_up)
{
	/* return to caller */
	return 1UL;
}


void InitEXTEng(void)
{
//    LoadHeuristics((LPVOID)__hmodule);
//    MVPLoadData((LPVOID)__hmodule);
}


#else



#include "platform.h"
#include "mvp.h"

#include "ctsn.h"
#include "callback.h"
#include "navex.h"

#include "heurapi.h"
#include "navheur1.h"
#include "callfake.h"

#include "nvx15inf.h"

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

BOOL gbInWindows = FALSE;

extern char gszStartUpDir[256];

int WINAPI LibMain (HINSTANCE hInst, WORD wDataSeg, WORD wHeapSize,
                        LPSTR lpszCmdLine)
{
   void LoadHeuristics(LPVOID);

   hInstance = hInst;

#ifdef SYM_WIN16

   if (wHeapSize > 0)
      UnlockData (0);

#endif

    gbInWindows = TRUE;

    MVPLoadData((LPVOID)hInst);

#if !defined(NAVEX15) && defined(SYM_WIN)
    // Get the start up directory

    if (GetModuleFileName(hInst,gszStartUpDir,256) == 0)
        gszStartUpDir[0] = 0;
    else
    {
        int     i;
        int     nLastBackslash;

        gszStartUpDir[255] = 0;
        nLastBackslash = -1;
        for (i=0;gszStartUpDir[i];i++)
            if (gszStartUpDir[i] == '\\')
                nLastBackslash = i;

        gszStartUpDir[nLastBackslash + 1] = 0;
    }
#endif // #if !defined(NAVEX15) && defined(SYM_WIN)

#if !defined(NAVEX15)

#ifdef SYM_WIN
    NAVEXInfCheck();    // Load NAVEX.INF settings
#endif

#if defined (USE_BLOODHOUND_PICASSO)
    LoadHeuristics((LPVOID)hInst);
#endif // #if defined (USE_BLOODHOUND_PICASSO)
#endif

   return (1);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------

int WINAPI WEP(int nParam)
{
    void UnloadHeuristics(void);

    MVPUnloadData();

#if !defined(NAVEX15)
#if defined (USE_BLOODHOUND_PICASSO)

    extern NAVEX_HEUR_T        gstHeur;

    // make sure to reset our callbacks so we use our local functions
    // to free memory (instead of the callbacks)

    gstHeur.lpstCallBacks = NULL;

    UnloadHeuristics();
#endif // #if defined (USE_BLOODHOUND_PICASSO)
#endif

   return(1);  // Return success to windows //
}

#endif // SYM_OS2
