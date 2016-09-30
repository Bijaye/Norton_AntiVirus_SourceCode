//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/NAVEXENT.CPv   1.23   20 Nov 1998 17:02:14   rpulint  $
//
// Description:
//      Contains NAVEX EXTStartUp and EXTShutDown functions
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/NAVEXENT.CPv  $
// 
//    Rev 1.23   20 Nov 1998 17:02:14   rpulint
// WATCOM Changes: char array declaration in InitHeur() - declare each
// char array on a new line !!
// 
//    Rev 1.22   06 Nov 1998 17:15:52   rpulint
// Added Changes for OS/2
// 
//    Rev 1.21   06 Aug 1998 13:47:08   MKEATIN
// APP_STRING is "NAVWIN" on Win16.
// 
//    Rev 1.20   12 Jun 1998 19:59:48   CNACHEN
// Added "infestmode" NAVEX15.INF option to disable infestation mode:
// 
// usage:
// 
// [NAVW32]
// infestmode=0                    ; off
// infestmode=1                    ; on (default)
// 
//
//    Rev 1.19   12 Jun 1998 13:57:40   CNACHEN
// Added infestation check and code.
//
//    Rev 1.18   10 Jun 1998 16:21:46   CNACHEN
// Changed char to TCHAR for NT kernel platform.
//
//    Rev 1.17   05 Jun 1998 15:16:40   CNACHEN
//
//    Rev 1.16   23 Mar 1998 18:08:10   CNACHEN
// Added heuristic support to NTK.
//
//    Rev 1.15   13 Mar 1998 11:12:34   CNACHEN
// Supports VXD platform now.
//
//    Rev 1.14   26 Jan 1998 16:55:22   DDREW
// For NLM we do the PAMLocalInit inside of PAMGlobalInit
//
//    Rev 1.13   13 Nov 1997 16:14:14   CNACHEN
// Fixed bug:  if you disable NAV from the application and the NAVEX15.INF file
// is missing, NAVEXENT.CPP would turn heuristics on even tho it was disabled.
//
//    Rev 1.12   15 Sep 1997 10:36:54   CNACHEN
// Changed SYM_DX to SYM_DOSX
//
//    Rev 1.11   08 Sep 1997 11:22:20   CNACHEN
// Fixed #else if to #elif
//
//    Rev 1.10   05 Sep 1997 12:52:34   CNACHEN
// Added NLM app string.
//
//    Rev 1.9   05 Sep 1997 12:51:42   CNACHEN
// Fixed bug where extension list was not being set if the application had a
// bImplemented of FALSE for "heur".
//
//    Rev 1.8   05 Sep 1997 12:32:36   DDREW
// Need to call LibExitFunction() for NLM or else we stay forever...
//
//    Rev 1.7   23 Aug 1997 11:18:02   STRILLI
// Added checks for USE_HEUR so that we use heuristics on the
// right platforms.
//
//    Rev 1.6   28 Jul 1997 17:30:36   DCHI
// Changed to use DataDirAppendName() to get full path of data file.
//
//    Rev 1.5   18 Jul 1997 16:09:56   CNACHEN
// Added for NAVENG.
//
//    Rev 1.4   01 Jul 1997 13:50:12   CNACHEN
// Added support for heavy hitters.
//
//    Rev 1.3   19 Jun 1997 16:45:36   CNACHEN
// Added command line/options checking and fixed some file close bugs.
//
//    Rev 1.2   11 Jun 1997 17:34:28   CNACHEN
// Added INI file support and extension checking support
//
//    Rev 1.1   04 Jun 1997 11:39:42   CNACHEN
//
//    Rev 1.0   15 May 1997 15:35:02   jsulton
// Initial revision.
//
//    Rev 1.1   08 May 1997 13:00:18   MKEATIN
// Use n30type.h instead of ctsn.h
//
//    Rev 1.0   02 May 1997 13:28:54   CNACHEN
// Initial revision.
//
//************************************************************************

//#include "platform.h"                 // NEW_UNIX
#include "avtypes.h"                    // NEW_UNIX
#include "n30type.h"
#include "callbk15.h"
#include "navex15.h"
#include "heurapi.h"
#include "navheur.h"
#include "inifile.h"
#include "datadir.h"

#include "avendian.h"               // IBM string scanner
#include "strscan.h"
#include "strscan.h"

//////////////////////////////////////////////////////////////////////////////
// NAVEX1.5 globals
//////////////////////////////////////////////////////////////////////////////

NAVEX_HEUR_T        gstHeur;
SEARCH_INFO_T       gstIBMFile;
SEARCH_INFO_T       gstIBMBoot;
int                 gnUsageCount = 0;
BOOL                gbDisableInfestationMode = FALSE;

#ifdef BUILD_HEAVY_HITTER

#include <stdio.h>
#include <time.h>

FILE *              glpstHeavy = NULL;
FILE *              glpstFalse = NULL;

#endif

#ifdef SYM_WIN32
#define APP_STRING "NAVW32"
#elif defined(SYM_WIN16)
#define APP_STRING "NAVWIN"
#elif defined(SYM_DOSX)
#define APP_STRING "NAVDX"
#elif defined(SYM_NLM)
#define APP_STRING "NAVNLM"
#elif defined(SYM_VXD)
#define APP_STRING "NAVVXD"
#elif defined(SYM_NTK)
#define APP_STRING "NAVNTK"
#elif defined(SYM_OS2)
#define APP_STRING "NAVOS2"
#elif defined(SYM_UNIX)                      // NEW_UNIX
#define APP_STRING "NAVUNIX"                 // NEW_UNIX
#endif

#define TECH_DISABLED           0
#define TECH_ENABLED            1
#define TECH_UNKNOWN            2

//////////////////////////////////////////////////////////////////////////////
// NAVEX1.5 globals
//////////////////////////////////////////////////////////////////////////////

#if defined(USE_HEUR)

EXTSTATUS FreeHeurExtensions
(
    LPCALLBACKREV2      lpstCallBacks,
    LPNAVEX_HEUR        lpstHeur
)
{
    LPEXT_NODE          lpstNode, lpstNext;

    lpstNode = lpstHeur->lpstExtList;

    while (lpstNode != NULL)
    {
	lpstNext = lpstNode->lpstNext;

	lpstCallBacks->PermMemoryFree(lpstNode);
	lpstNode = lpstNext;
    }

    return(EXTSTATUS_OK);
}

extern int mytoupper(int);

EXTSTATUS AddHeurExtensions
(
    LPCALLBACKREV2      lpstCallBacks,
    LPNAVEX_HEUR        lpstHeur,
    LPSTR               lpszExtList
)
{
    char                szCurExt[4];
    int                 i, nExtIndex = 0;
    LPEXT_NODE          lpstNode;

    lpstHeur->lpstExtList = NULL;

    for (i=0;lpszExtList[i];i++)
    {
	if (lpszExtList[i] == ' ' ||
	    lpszExtList[i] == '\t')
	    continue;

	if (lpszExtList[i] == ',')
	{
	    szCurExt[nExtIndex] = 0;
	    nExtIndex = 0;

	    lpstNode = (LPEXT_NODE)lpstCallBacks->
				    PermMemoryAlloc(sizeof(EXT_NODE_T));
	    if (lpstNode == NULL)
	    {
		FreeHeurExtensions(lpstCallBacks,lpstHeur);

		return(EXTSTATUS_MEM_ERROR);
	    }

	    lpstNode->szExt[0] = (TCHAR)szCurExt[0];
	    lpstNode->szExt[1] = (TCHAR)szCurExt[1];
	    lpstNode->szExt[2] = (TCHAR)szCurExt[2];
	    lpstNode->szExt[3] = (TCHAR)szCurExt[3];

	    lpstNode->lpstNext = lpstHeur->lpstExtList;
	    lpstHeur->lpstExtList = lpstNode;
	    continue;
	}

	szCurExt[nExtIndex++] = mytoupper(lpszExtList[i]);
    }

    if (nExtIndex)
    {
	szCurExt[nExtIndex] = 0;

	lpstNode = (LPEXT_NODE)lpstCallBacks->
				PermMemoryAlloc(sizeof(EXT_NODE_T));
	if (lpstNode == NULL)
	{
	    FreeHeurExtensions(lpstCallBacks,lpstHeur);

	    return(EXTSTATUS_MEM_ERROR);
	}

	lpstNode->szExt[0] = (TCHAR)szCurExt[0];
	lpstNode->szExt[1] = (TCHAR)szCurExt[1];
	lpstNode->szExt[2] = (TCHAR)szCurExt[2];
	lpstNode->szExt[3] = (TCHAR)szCurExt[3];

	lpstNode->lpstNext = lpstHeur->lpstExtList;
	lpstHeur->lpstExtList = lpstNode;
    }

    return(EXTSTATUS_OK);
}

#ifdef SYM_NLM

EXTSTATUS InitHeur
(
    LPCALLBACKREV2                          lpstCallBacks,
    WORD                                    wClientVersion,
    struct tag_EXPORT15_TABLE FAR *         lpstChain,
    LPTSTR                                  lpszNAVEXDataDir,
    LPTSTR                                  lpszNAVEXINIFile,
    LPWORD                                  lpwServerVersion
)
{

    char        szDatFileName[SYM_MAX_PATH];
    HFILE       hFile;
    int         nOut;
    char        szExtList[513], szOptionValue[128], szInfestMode[33];
    BOOL        bImplemented;
    LPSTR       lpszNoSpace;

    gbDisableInfestationMode = FALSE;       // go to high if we get lots
					    // of BH viruses - by default

    DataDirAppendName(lpszNAVEXDataDir,_T("virscan5.dat"),szDatFileName); // NEW_UNIX

    // only enable after we've successfully loaded

    gstHeur.bEnabled = FALSE;

    // remeber where our callbacks are

    gstHeur.lpstCallBacks = lpstCallBacks;

    // check our config callbacks to see if we want to use heuristics

    lpstCallBacks->GetConfigInfo("heur",
				 (LPVOID)szOptionValue,
				 sizeof(szOptionValue)-1,
				 &bImplemented,
				 NULL);

    if (bImplemented == TRUE)
    {
	// "/heur" was definitely on the command line or an option; get the
	// string.

	if (szOptionValue[0] >= '0' && szOptionValue[0] <= '3')
	    nOut = szOptionValue[0] - '0';
	else
	    nOut = 2;           // default is level 2: "/heur"

	gstHeur.nHeurLevel = nOut;
    }
    else
    {
	// this should never happen!

	return(EXTSTATUS_INIT_ERROR);
    }

    // now check our ini file to see if we want to use heur and/or ext. list

    hFile = lpstCallBacks->FileOpen(lpszNAVEXINIFile,0);
    if (hFile == (HFILE)-1)
    {
	// defaults: address all defaults

	if (AddHeurExtensions(lpstCallBacks,
			      &gstHeur,
			      "com,exe") != EXTSTATUS_OK)
	{
	    return(EXTSTATUS_INIT_ERROR);
	}
    }
    else
    {
	// see if there's a heuristics extensions list; default is COM,EXE

	GetProfileString(hFile,
			APP_STRING,
			"heurextlist",
			"com,exe",
			szExtList,
			512);

	if (AddHeurExtensions(lpstCallBacks,
			      &gstHeur,
			      szExtList) != EXTSTATUS_OK)
	{
	    if (hFile != (HFILE)-1)
		lpstCallBacks->FileClose(hFile);

	    return(EXTSTATUS_INIT_ERROR);
	}

	//////////////////////////////////////////////////////////////////////
	// infestation mode inf check
	//////////////////////////////////////////////////////////////////////

	GetProfileString(hFile,
			 APP_STRING,
			 "infestmode",
			 "1",
			 szInfestMode,
			 32);

	lpszNoSpace = szInfestMode;

	while (*lpszNoSpace == ' ' || *lpszNoSpace == '\t')
	    lpszNoSpace++;

	if (*lpszNoSpace != '0')
	    gbDisableInfestationMode = FALSE;
	else
	    gbDisableInfestationMode = TRUE;

	//////////////////////////////////////////////////////////////////////
	// infestation mode inf check
	//////////////////////////////////////////////////////////////////////

	if (hFile != (HFILE)-1)
	    lpstCallBacks->FileClose(hFile);
    }

    // perform heur global init

    if (PAMGlobalInit(szDatFileName, &gstHeur.hGHeur) != PAMSTATUS_OK)
    {
	FreeHeurExtensions(lpstCallBacks, &gstHeur);

	return(EXTSTATUS_INIT_ERROR);
    }

    // perform the heur local init (only for NLM!!!)

    if (PAMLocalInit(gstHeur.hGHeur,&gstHeur.hLHeur) != PAMSTATUS_OK)
    {
	// error initializing local; free global!

	FreeHeurExtensions(lpstCallBacks, &gstHeur);

	PAMGlobalClose(gstHeur.hGHeur);

	return(EXTSTATUS_MEM_ERROR);
    }

    // now indicate that we're enabled

    gstHeur.bEnabled = TRUE;

    return ( EXTSTATUS_OK );
}


#else

EXTSTATUS InitHeur
(
    LPCALLBACKREV2                          lpstCallBacks,
    WORD                                    wClientVersion,
    struct tag_EXPORT15_TABLE FAR *         lpstChain,
    LPTSTR                                  lpszNAVEXDataDir,
    LPTSTR                                  lpszNAVEXINIFile,
    LPWORD                                  lpwServerVersion
)
{

#if defined(SYM_VXD)
    char        szDatFileName[2 * SYM_MAX_PATH * sizeof(WCHAR)];
#elif defined(SYM_NTK)
    TCHAR       szDatFileName[2 * SYM_MAX_PATH];
#elif defined(SYM_WIN)
    TCHAR       szDatFileName[2 * SYM_MAX_PATH];
#else
    char        szDatFileName[SYM_MAX_PATH];
#endif

    HFILE       hFile;
    int         nOut, nUseHeur;
    char        szExtList[513]  ;
    char        szOptionValue[128] ;
    char        szInfestMode[33];
    BOOL        bImplemented;
    LPSTR       lpszNoSpace;

    gbDisableInfestationMode = FALSE;       // go to high if we get lots
					    // of BH viruses - by default

    DataDirAppendName(lpszNAVEXDataDir,_T("virscan5.dat"),szDatFileName); // NEW_UNIX

    // only enable after we've successfully loaded

    gstHeur.bEnabled = FALSE;

    // remeber where our callbacks are

    gstHeur.lpstCallBacks = lpstCallBacks;

    // first check our config callbacks to see if we want to use heuristics

    // precedence:
    // 1. command line option or built in option
    // 2. ini file
    // 3. default is on

    lpstCallBacks->GetConfigInfo(_T("heur"),
				 (LPVOID)szOptionValue,
				 sizeof(szOptionValue)-1,
				 &bImplemented,
				 NULL);

    if (bImplemented == TRUE)
    {
	// "/heur" was definitely on the command line or an option; get the
	// string.

	if (szOptionValue[0] >= '0' && szOptionValue[0] <= '3')
	    nOut = szOptionValue[0] - '0';
	else
	    nOut = 2;           // default is level 2: "/heur"

	gstHeur.nHeurLevel = nOut;

	if (nOut != 0)
	    nUseHeur = TECH_ENABLED;
	else
	    nUseHeur = TECH_DISABLED;
    }
    else
    {
	// default: heur was not known by the app; check our ini file or
	// use defaults

	nUseHeur = TECH_UNKNOWN;
    }

    // now check our ini file to see if we want to use heur and/or ext. list

    hFile = lpstCallBacks->FileOpen(lpszNAVEXINIFile,0);
    if (hFile == (HFILE)-1)
    {
	// defaults: address all defaults

	if (nUseHeur == TECH_ENABLED || nUseHeur == TECH_DISABLED)
	{
	    // OK. just add extensions and continue
	}
	else
	{
	    // fow now: assume Heuristics are on by default!

	    nUseHeur = TECH_ENABLED;
	    gstHeur.nHeurLevel = 2;
	}

	if (AddHeurExtensions(lpstCallBacks,
			      &gstHeur,
			      "com,exe") != EXTSTATUS_OK)
	{
	    return(EXTSTATUS_INIT_ERROR);
	}
    }

    // now that we've opened the file, see if heuristics are on
    // assuming we don't already know

    if (hFile != (HFILE)-1 && nUseHeur == TECH_UNKNOWN)
    {
	GetProfileInt(hFile,
		      APP_STRING,
		      "heuristics",
		      2,            // 3=on by default
		      &nOut);

	if (nOut != 0)
	{
	    gstHeur.nHeurLevel = nOut;

	    nUseHeur = TECH_ENABLED;
	}
	else
	    nUseHeur = TECH_DISABLED;
    }

    if (hFile != (HFILE)-1 && nUseHeur == TECH_ENABLED)
    {
	// see if there's a heuristics extensions list; default is COM,EXE

	GetProfileString(hFile,
			APP_STRING,
			"heurextlist",
			"com,exe",
			szExtList,
			512);

	if (AddHeurExtensions(lpstCallBacks,
			      &gstHeur,
			      szExtList) != EXTSTATUS_OK)
	{
	    nUseHeur = TECH_DISABLED;
	}

	//////////////////////////////////////////////////////////////////////
	// infestation mode inf check
	//////////////////////////////////////////////////////////////////////

	GetProfileString(hFile,
			 APP_STRING,
			 "infestmode",
			 "1",
			 szInfestMode,
			 32);

	lpszNoSpace = szInfestMode;

	while (*lpszNoSpace == ' ' || *lpszNoSpace == '\t')
	    lpszNoSpace++;

	if (*lpszNoSpace != '0')
	    gbDisableInfestationMode = FALSE;
	else
	    gbDisableInfestationMode = TRUE;

	//////////////////////////////////////////////////////////////////////
	// infestation mode inf check
	//////////////////////////////////////////////////////////////////////

    }

    if (nUseHeur == TECH_ENABLED)
    {
	// perform heur global init

	if (PAMGlobalInit(szDatFileName, &gstHeur.hGHeur) != PAMSTATUS_OK)
	{
	    if (hFile != (HFILE)-1)
		lpstCallBacks->FileClose(hFile);

	    FreeHeurExtensions(lpstCallBacks, &gstHeur);

	    return(EXTSTATUS_INIT_ERROR);
	}

	// now indicate that we're enabled

	gstHeur.bEnabled = TRUE;

#ifdef BUILD_HEAVY_HITTER

	glpstHeavy = fopen("c:\\navheavy.dat","at");
	if (glpstHeavy != NULL)
	{
	    long t;

	    time(&t);

	    fprintf(glpstHeavy,"\n\nHeavy hitter list created %s\n\n",ctime(&t));
	}

	glpstFalse = fopen("c:\\navfalse.dat","at");
	if (glpstFalse != NULL)
	{
	    time_t t;

	    time(&t);

	    fprintf(glpstFalse,"\n\nFalse ID list created %s\n\n",ctime(&t));
	}
#endif

    }
    else
    {
	// this won't be done later because bloodhound is not enabled...

	FreeHeurExtensions(lpstCallBacks, &gstHeur);
    }

    if (hFile != (HFILE)-1)
	lpstCallBacks->FileClose(hFile);

    return ( EXTSTATUS_OK );
}

#endif // #ifdef SYM_NLM

#endif // #if defined(USE_HEUR)

#if defined(USE_HEUR)

EXTSTATUS CloseHeur
(
    LPCALLBACKREV2                          lpstCallBacks
)
{
    if (gstHeur.bEnabled == TRUE)
    {
#ifdef BUILD_HEAVY_HITTER

	if (glpstHeavy != NULL)
	    fclose(glpstHeavy);

	if (glpstFalse != NULL)
	    fclose(glpstFalse);
#endif

    // only free local PAM information for NLM!

#ifdef SYM_NLM
	PAMLocalClose(gstHeur.hLHeur);
#endif

	PAMGlobalClose(gstHeur.hGHeur);
	FreeHeurExtensions(lpstCallBacks, &gstHeur);
    }

    return(EXTSTATUS_OK);
}

#endif // #if defined(USE_HEUR)


// EXTStartUp
//  lpstCallBacks       Callbacks provided by cleint
//  wClientVersion      Version number of client for compatability checking
//  lpstChain           Pointer to the ENGINE1.5 export table, to be passed
//                      in to the NAVEX1.5 EXTStartUp function.
//  lpszNAVEXDataDir    Path, including trailing backslash to data file dir.
//  lpszNAVEXINIFile    Full path and filename to NAVEX1.5 .ini file.
//  lpwServerVersion    Pointer to a WORD to contain the NAVEX/ENGINE1.5 version #
//
// Description:
//  This should only be called for the NAVEX1.5 module; it will automatically
//  call the ENGINE1.5 EXTStartUp if lpstChain points to its export table.  This
//  function loads all data files and sets up all global data for NAVEX scanning.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_MEM_ERROR           if memory error
//                        EXTSTATUS_FILE_ERROR          if file error
//                        EXTSTATUS_INIT_ERROR          general init. error
//

EXTSTATUS FAR WINAPI NLOADDS EXTStartUp
(
    LPCALLBACKREV2                          lpstCallBacks,
    WORD                                    wClientVersion,
    struct tag_EXPORT15_TABLE FAR *         lpstChain,
    LPTSTR                                  lpszNAVEXDataDir,
    LPTSTR                                  lpszNAVEXINIFile,
    LPWORD                                  lpwServerVersion
)
{
    TCHAR       szDatFileName[SYM_MAX_PATH];


    if (gnUsageCount > 0)
    {
	gnUsageCount++;
	return(EXTSTATUS_OK);
    }

    gnUsageCount++;

    // init file heuristics (bloodhound)

#if defined(USE_HEUR)


    if (InitHeur(lpstCallBacks,
		 wClientVersion,
		 lpstChain,
		 lpszNAVEXDataDir,
		 lpszNAVEXINIFile,
		 lpwServerVersion) != EXTSTATUS_OK)
	return(EXTSTATUS_FILE_ERROR);

#endif // #if defined(USE_HEUR)

    // always load string scanning data


    DataDirAppendName(lpszNAVEXDataDir,_T("virscan8.dat"),szDatFileName);

    if (LoadAllStringData(lpstCallBacks,
			  szDatFileName,
			  IBM_FILE_BASE,
			  &gstIBMFile) != STRING_LOAD_OK)
    {
	CloseHeur(lpstCallBacks);

	return(EXTSTATUS_FILE_ERROR);
    }

    // always load the boot scanning data if we're not on NLM

    if (LoadAllStringData(lpstCallBacks,
			  szDatFileName,
			  IBM_BOOT_BASE,
			  &gstIBMBoot) != STRING_LOAD_OK)
    {
	// its ok to free if we did not load - the data will be zeroed anyway

	FreeAllStringData(lpstCallBacks,&gstIBMFile);
	CloseHeur(lpstCallBacks);

	return(EXTSTATUS_FILE_ERROR);
    }

    return ( EXTSTATUS_OK );
}





// EXTShutDown
//
//  lpstCallBacks       Callbacks provided by cleint
//
// Description:
//
//  Call this function only for NAVEX1.5 module.  Do not call for ENGINE1.5.
//  This func. frees all global scanning data allocated by NAVEX/ENGINE.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_MEM_ERROR           if memory error
//                        EXTSTATUS_FILE_ERROR          if file error
//                        EXTSTATUS_INIT_ERROR          general init. error
//

EXTSTATUS FAR WINAPI NLOADDS EXTShutDown
(
    LPCALLBACKREV2                          lpstCallBacks
)
{
    gnUsageCount--;

    if (gnUsageCount > 0)
	return(EXTSTATUS_OK);

#if defined(USE_HEUR)

    CloseHeur(lpstCallBacks);

#endif // #if defined(USE_HEUR)

    FreeAllStringData(lpstCallBacks,&gstIBMFile);   // free string scanning data
    FreeAllStringData(lpstCallBacks,&gstIBMBoot);   // free string scanning data

#ifdef SYM_NLM
    LibraryExitFunction();
#endif

    return ( EXTSTATUS_OK );
}

