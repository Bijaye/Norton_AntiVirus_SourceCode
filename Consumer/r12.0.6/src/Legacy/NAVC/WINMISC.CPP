// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVC/VCS/winmisc.cpv   1.2   14 Aug 1997 17:05:44   MKEATIN  $
//
// Description:
//
// Contents:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVC/VCS/winmisc.cpv  $
// 
//    Rev 1.2   14 Aug 1997 17:05:44   MKEATIN
// Renamed NAVSYSL.DAT to NAVSYSR.DAT.  Avoids conflicts with FAT32 boot
// CRC's from older versions of NAV.
// 
//    Rev 1.1   12 Aug 1997 18:28:52   MKEATIN
// Added GetNavexInfFile().
// 
//    Rev 1.0   06 Feb 1997 20:56:32   RFULLER
// Initial revision
// 
//    Rev 1.1   13 Jan 1997 14:14:18   MKEATIN
// Now we check for NAVOPTS.DAT before NAVCOPTS.DAT
// 
//    Rev 1.0   31 Dec 1996 15:21:24   MKEATIN
// Initial revision.
// 
//    Rev 1.1   11 Dec 1996 17:46:50   JBELDEN
// setup code so navcopts is looked for and used and if not found navopts.dat
// is used.
// 
//    Rev 1.0   06 Dec 1996 11:51:36   JBELDEN
// Initial revision.
// 
//    Rev 1.34   05 Apr 1996 17:02:24   MKEATIN
// Changed typecast from LPSTR to LPBYTE for buffer passed to GetInfectionData()
// 
//    Rev 1.33   06 Feb 1996 19:56:26   GWESTER
// Moved SchedulerIsInstalled() from .EXE to here.
// 
//    Rev 1.32   02 Feb 1996 17:41:12   DALLEE
// #ifdef SYM_WIN around new windows-specific calls.
// 
//    Rev 1.31   31 Jan 1996 14:54:32   GWESTER
// Added NAVVER.H for Registry references.
// 
//    Rev 1.30   31 Jan 1996 14:41:50   GWESTER
// Moved GetCustomVersion(), SetCustomVersion() from EXE to here.
// 
//    Rev 1.29   09 Nov 1995 14:04:18   GWESTER
// Changed GetStartDir() handling of LFNs. If we are running NT and GetStartDir()
// can't lookup an SFN equivalent, it will return the LFN.
// 
//    Rev 1.28   03 Nov 1995 20:22:30   GWESTER
// Changed NT-check from compile-time to run-time. We won't have to split this
// DLL until we absolutely have to.
// 
//    Rev 1.27   03 Nov 1995 19:55:34   GWESTER
// Changed NT-specific version so that TalkToVxD () does nothing.
// 
//
//    Rev 1.26   24 Oct 1995 08:45:06   DALLEE
// Removed last change -- System Inoculation will be removed entirely from
// Gargoyle builds. We should leave the functionality unchanged until changes
// are required.
// 
// 
//    Rev 1.25   17 Oct 1995 18:21:30   DALLEE
// W32 platform: rename sysinteg database to NAVSYSNT.DAT when running under
// Windows NT.
// 
//    Rev 1.24   03 Jul 1995 18:17:50   DALLEE
// Changed OemToAnsi() to NOemToAnsi() cover so DOS doesn't chunk.
// 
//    Rev 1.23   01 Jul 1995 16:02:00   Sheathco
// Correction for special character sets, (OEMtoAnsi)
// 
// 
//    Rev 1.22   20 Jun 1995 21:30:38   MARKL
// Since when does GetModuleFilename() return a string in OEM and not ANSI??
// Got rid of the OemToAnsi() calls immediately following each call to
// GetModuleFilename().  If this is indeed true, there is a TON of source
// to go through in every product and quake.
//
//    Rev 1.21   07 Jun 1995 18:09:52   MARKL
// Now looks for NAVAPW based on its window class since the VxD.
// 
//    Rev 1.20   31 May 1995 17:03:20   KEITH
// Allow unload of NAVWAP immediately when turned off in config.
// 
//    Rev 1.19   17 May 1995 12:02:02   SZIADEH
// selfcheck code.
// 
//    Rev 1.18   11 May 1995 18:46:16   MARKL
// Fixed defect 35187.  Now autolaunches VxD or displays appropriate messages
// if "Load AutoProtect At Startup" is changed.
// 
//    Rev 1.17   17 Apr 1995 15:47:16   MFALLEN
// sym_max_path+1
// 
//    Rev 1.16   12 Apr 1995 17:15:48   MFALLEN
// Use quake NameShortenFilename() instead of MakePathShorter()
// 
//    Rev 1.15   04 Apr 1995 19:38:42   SZIADEH
// changed "NAVSYS.DAT" to "NAVSYSL.DAT"
// 
//    Rev 1.14   31 Mar 1995 06:51:42   MFALLEN
// Removed old file validation. The new IsValidLFN or IsValidSFN should be used.
// 
//    Rev 1.13   30 Mar 1995 16:42:02   MFALLEN
// GetStartDir() will always return the short name of the startup directory. THis
// is to be consistent with previous releases.
// 
//    Rev 1.12   30 Mar 1995 14:01:04   MFALLEN
// Replaced NameGetIllegalChars() with NameReturnIllegalChars()
// 
//    Rev 1.11   27 Mar 1995 16:20:38   DALLEE
// Final(?) "fix" to FileIsDOSExecutable().
// Replaced NameWildcardMatch() code which didn't understand full paths, with
// NameReturnExtension() and STRICMP().  Simplistic, maybe.  Working, yes.
// 
//    Rev 1.10   22 Mar 1995 15:58:22   MARKL
// Added TalkToVxD.  This routine use to be in NAVW.CPP
// 
//    Rev 1.9   16 Mar 1995 14:32:24   DALLEE
// Moved szOptions buffer here where it's required.
// (TOOLS programs weren't linking)
// 
//    Rev 1.8   09 Mar 1995 15:48:52   DALLEE
// Removed "windowsx.h" - we pick it up from platform.h with <> brackets.
// Since this comes right from the SDK, we don't want to pick it up
// as a dependency w/ MKDEPS.
// 
//    Rev 1.7   03 Mar 1995 16:23:08   DALLEE
// For NAVBOOT (DOS) - Return szOptionsDir rather than program dir from
// GetProgramDir().
// 
//    Rev 1.6   10 Feb 1995 10:47:52   MFALLEN
// Fixed for W16
// 
//    Rev 1.5   23 Jan 1995 17:44:18   DALLEE
// Added routines to get name of NAVSTART.DAT
// 
//    Rev 1.4   20 Jan 1995 19:09:44   MFALLEN
// All the DOS and VXD strings are accessed using the LoadString() functions
// that emulates it's Windows counterpart. All the string ID's are stored in the
// stbexts.h file that identical for all the platforms. DOS and VXD strings are
// contained in the UTILSTR.STR file.
// 
//    Rev 1.3   16 Jan 1995 13:16:44   MFALLEN
// Was not compiling for DOS platform.
// 
//    Rev 1.2   27 Dec 1994 16:58:12   MFALLEN
// Fixed the FileIsDosExecutable function so it behaves correctly under W32.
// 
//    Rev 1.1   22 Dec 1994 09:08:50   MFALLEN
// Patched NavSelfTest while it's under construction.
// 
//    Rev 1.0   16 Dec 1994 10:36:48   MFALLEN
// Initial revision.
// 
//    Rev 1.14   15 Dec 1994 20:14:12   MFALLEN
// From NIRVANA
// 
//    Rev 1.13   16 Oct 1994 22:56:22   DALLEE
// Added call to VirusScanSaveMemoryInfo() on failed self-test.
// 
//    Rev 1.12   17 Nov 1993 17:57:20   MFALLEN
// A lot of fixes for the inline.
// 
//    Rev 1.11   26 Oct 1993 16:03:32   DALLEE
// Fix to FileIsDOSExecutable().
// 
//    Rev 1.10   20 Oct 1993 19:16:06   DALLEE
// Added FileIsDOSExecutable().
//
//    Rev 1.9   14 Oct 1993 16:31:54   MFALLEN
// Fixed casting bug.
// 
//    Rev 1.8   07 Oct 1993 17:52:34   MFALLEN
// Make sure we internally deal only with Ansi strings
// 
//    Rev 1.7   20 Sep 1993 05:01:30   BRAD
// If value coming in was too large, wouldn't work.
// 
//    Rev 1.6   16 Sep 1993 04:39:52   BRAD
// Changed ZIP_SEPARATOR.
// 
//    Rev 1.5   23 Aug 1993 00:39:28   BARRY
// Added MODULE_NAME and fixed MemUnlock arguments for VMM debugging
// 
//    Rev 1.4   17 Aug 1993 23:25:52   MFALLEN
// New function to shorten path
// 
//    Rev 1.3   16 Aug 1993 23:12:22   MFALLEN
// Fix
// 
//    Rev 1.2   16 Aug 1993 21:54:04   MFALLEN
// GetSysIntegFile()
// 
//    Rev 1.1   16 Aug 1993 14:10:26   MFALLEN
// New parameters to GetSysIntegFile()
//
//    Rev 1.0   12 Aug 1993 11:57:58   MFALLEN
// Initial Revision
//
//    Rev 1.26   27 Jul 1993 19:29:32   MFALLEN
// Removed GetTsrFlag()
//************************************************************************


#include "platform.h"
#include "xapi.h"
#include "navopts.h"

#ifdef SYM_WIN16
    #include "cts4.h"
#else
    #include "ctsn.h"
#endif

#include "navutil.h"
#include "disk.h"
#include "virscan.h"
#include "navapcom.h"

#ifdef SYM_WIN
    #include "ngui.h"
    #include "msgdisp.h"
    #define  TRACE_ID   627
    #include "file.h"
#endif

#ifdef SYM_WIN32
#include "navwcomm.h"
#endif

#include "stbexts.h"

#ifdef SYM_WIN32
#include "navver.h"
#endif



#ifdef __cplusplus
extern "C" {
#endif

extern BOOL PASCAL FileExistsInDirPath( LPSTR, LPSTR );
LPSTR SYM_EXPORT WINAPI GetNavcBinFilename(LPSTR);

#ifdef __cplusplus
}
#endif




MODULE_NAME;

										// Indicate regular version, or Netscape version;
										// this will be expanded in the future.
static	UINT	uCustomVersion = VERSION_STANDARD;


#ifdef SYM_WIN
static  UINT    uTsrVar = 0;

//***************************************************************************
// DisableDialog()
//
// Description:
//      This routine disables the child controls of the dialogs
//      comprising the Vishnu control panel.
//
// Parameters:
//      hWnd                    [in] Dialog Handle
//      UINT FAR *              [in] Array of Child window IDs not to disable
//
// Return Value: 
//      none
// 
//***************************************************************************
// 03/10/1993 SGULLEY Function Created.                                   
// 04/15/1993 Martin  Function Modified to be more generic
//***************************************************************************
VOID SYM_EXPORT WINAPI DisableDialog(       // Disables child controls
    HWND hDlg,                          // [in] Parent dialog handle
    BOOL bDisable,                      // [in] disable children
    UINT FAR *lpSaveChildren)           // [in] Childrens to save
{
   BOOL SYM_EXPORT CALLBACK DisableChildWindow (HWND hChild, LPARAM lParam);
   BOOL SYM_EXPORT CALLBACK EnableChildWindow (HWND hChild, LPARAM lParam);
   extern HINSTANCE hInstance;
   FARPROC lpfn;

   if (bDisable == TRUE)
      lpfn = MakeProcInstance((FARPROC)DisableChildWindow, hInstance);
   else
      lpfn = MakeProcInstance((FARPROC)EnableChildWindow, hInstance);

   if (lpfn)
      {
      EnumChildWindows(hDlg, (WNDENUMPROC)lpfn, (LPARAM)lpSaveChildren);
      FreeProcInstance(lpfn);
      }
}

BOOL SYM_EXPORT CALLBACK DisableChildWindow (HWND hChild, LPARAM lParam)
{
   #define CLASSNAMELEN 25

#ifdef SYM_WIN32
    auto    DWORD  wChildStyle;
#else
    auto    WORD   wChildStyle;
#endif

   auto  BOOL  bFound;
   auto  UINT  FAR *lpSaveChildren;
   auto  UINT  uChildID, i;
   auto  char  szClassName[CLASSNAMELEN + 1];

   lpSaveChildren = (UINT FAR*)lParam;

   if (lpSaveChildren && (uChildID = GetDlgCtrlID(hChild)))
      {
      for (i = 0, bFound = FALSE; lpSaveChildren[i]; i++)
         if (lpSaveChildren[i] == uChildID)
            {
            bFound = TRUE;
            break;
            }

      if (bFound == FALSE)
         {
         EnableWindow(hChild, FALSE);
         GetClassName(hChild, szClassName, CLASSNAMELEN);

         if (!STRCMP((LPSTR)szClassName,"Button"))
            {
#ifdef SYM_WIN32
            wChildStyle = GetClassLong(hChild, GCL_STYLE);
#else
            wChildStyle = GetClassWord(hChild, GCW_STYLE);
#endif
            if (wChildStyle & BS_CHECKBOX              ||
                wChildStyle & BS_AUTOCHECKBOX)
               {
               Button_SetCheck(hChild,FALSE);
               }
            else if (wChildStyle & BS_RADIOBUTTON      || 
                     wChildStyle & BS_AUTORADIOBUTTON)
               {              
               Button_SetState(hChild,FALSE);
               }
            }
 
         if (!STRCMP((LPSTR)szClassName,"ListBox"))
            {
            ListBox_ResetContent(hChild);
            }

         if (!STRCMP((LPSTR)szClassName,"ComboBox"))
            {
            ComboBox_SetText(hChild,"");
            }
 
         if (!STRCMP((LPSTR)szClassName,"Edit"))
            {
            Edit_SetText(hChild,"");
            }
/*
         if (!STRCMP((LPSTR)szClassName,"Static"))
            {                        
                                     // make sure were not erasing 
                                     // required text
            if (GetDlgCtrlID(hChild) != -1)
               {
               Static_SetText(hChild,"");
               }
            }
*/
         if (!STRCMP((LPSTR)szClassName,"NGui_Tunnel"))
            {
            Tunnel_SetText(hChild,"");
            }
         }
      }

   return (TRUE);
}

BOOL SYM_EXPORT CALLBACK EnableChildWindow (HWND hChild, LPARAM lParam)
{
   #define CLASSNAMELEN 25

#ifdef SYM_WIN32
    auto    DWORD   wChildStyle;
#else
    auto    WORD    wChildStyle;
#endif

   auto  UINT  uChildID;
   auto  char  szClassName[CLASSNAMELEN + 1];

   if (uChildID = GetDlgCtrlID(hChild))
      {
      EnableWindow(hChild, TRUE);
      GetClassName(hChild, szClassName, CLASSNAMELEN);

      if (!STRCMP((LPSTR)szClassName,"Button"))
         {
#ifdef SYM_WIN32
         wChildStyle = GetClassLong(hChild, GCL_STYLE);
#else
         wChildStyle = GetClassWord(hChild, GCW_STYLE);
#endif
         if (wChildStyle & BS_CHECKBOX              || 
             wChildStyle & BS_AUTOCHECKBOX)
            {
            Button_SetCheck(hChild, FALSE);
            }
         else if (wChildStyle & BS_RADIOBUTTON      || 
                  wChildStyle & BS_AUTORADIOBUTTON)
            {
            Button_SetState(hChild, TRUE);
            }
         }
      }

   return (TRUE);
}


#endif                                       // This is an #endif for SYM_WIN //


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

VOID SYM_EXPORT WINAPI HMS_Time(DWORD dwTime, HMS FAR *Hms)
{
   MEMSET(Hms, 0, sizeof(HMS));

   if (dwTime >= 3600)
      {
      Hms->uHour = (UINT)(dwTime / 3600);

      dwTime -= ((DWORD)Hms->uHour * 3600);

      if ( dwTime >= 3600 )
          dwTime = (3600 - 1);
      }

   if (dwTime >= 60)
      {
      Hms->uMinute = (UINT)(dwTime / 60);
      dwTime -= (Hms->uMinute * 60);
      }

   Hms->uSecond = (UINT)dwTime;
}


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 19xx - 1993          **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author: Martin Fallenstedt          Date: January 8, 1993     **** //
// ****                                                                **** //
// ****  Functionality:                                                **** //
// ****     Returns directory where NAV is located.                    **** //
// ****     Used to determine locations of options files.              **** //
// ****                                                                **** //
// ****     For NAVBOOT - we return the options directory the user     **** //
// ****     selected at startup, not the program directory.            **** //
// ****                                                                **** //
// ****  Input:                                                        **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:                                                       **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //

#ifdef SYM_DOS

 #ifdef __cplusplus
 extern "C" {
 #endif

 char    szOptionsDir [SYM_MAX_PATH+1];

 #ifdef __cplusplus
 }
 #endif

#endif

VOID SYM_EXPORT WINAPI GetStartDir (HINSTANCE hInstance, LPSTR lpPath, UINT uSize)
{
#ifndef SYM_DOS
    auto    char    szPath [ SYM_MAX_PATH + 1 ];

    GetModuleFileName ( hInstance, szPath, sizeof(szPath) );
    NameStripFile ( szPath );

    STATUS ShortNameReturnValue = NameReturnShortName ( szPath, lpPath );

    if  (
        (ShortNameReturnValue != NOERR) &&
        (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        )
        {
                                        // If we are running NT, and it
                                        // can't convert the LFN to a
                                        // SFN, return the LFN.
        STRCPY ( lpPath , szPath );
        }
#else
    if (EOS != szOptionsDir[0])
        {
        STRCPY(lpPath, szOptionsDir);
        }
    else
        {
        DOSGetProgramName(lpPath);
        NameStripFile(lpPath);
        }
#endif
}

VOID SYM_EXPORT WINAPI GetSysIntegFile (LPSTR lpPath, UINT uSize, LPINOCOPT lpInoc)
{
    auto    char        szFilename [ SYM_MAX_PATH+1 ];


    if (*lpInoc->szNetInocPath && lpInoc->szNetInocPath[1] == ':')
        {
        STRCPY (lpPath, lpInoc->szNetInocPath);
        *lpPath = 'C';
        }
    else
        {
        STRCPY(lpPath, "C:");
        NameAppendFile(lpPath, lpInoc->szNetInocPath);
        }

    NameAppendFile(lpPath, GetSysIntegFilename(szFilename));
}

// ************************************************************************ //
// ************************************************************************ //
VOID SYM_EXPORT WINAPI GetNavBin(HINSTANCE hInstance, LPSTR lpPath, UINT uSize)
{
    auto        char            szFilename [ SYM_MAX_PATH+1 ];
    auto        char            szTmpPath  [ SYM_MAX_PATH+1 ];


    GetStartDir (hInstance, lpPath, uSize);
    strcpy(szTmpPath, lpPath);

    if ( FileExistsInDirPath(szTmpPath, GetNavBinFilename(szFilename) ))
        NameAppendFile(lpPath, GetNavBinFilename(szFilename));
     else
        NameAppendFile(lpPath, GetNavcBinFilename(szFilename));
}

// ************************************************************************ //
// ************************************************************************ //
LPSTR SYM_EXPORT WINAPI GetNavBinFilename(LPSTR lpFilename)
{
    return(STRCPY(lpFilename, "NAVOPTS.DAT"));
}

LPSTR SYM_EXPORT WINAPI GetNavcBinFilename(LPSTR lpFilename)
{
    return(STRCPY(lpFilename, "NAVCOPTS.DAT"));
}


/*@API:**********************************************************************
@Declaration:
VOID SYM_EXPORT WINAPI GetNavStart(HINSTANCE hInstance,
                                   LPSTR lpPath,
                                   UINT uSize)

@Description:
This function returns the full path to NAVSTART.DAT located in the program
directory.

@Parameters:
$hInstance$     Instance of .EXE for program directory.
$lpPath$        Buffer to place path.
$uSize$         size of buffer.

@Returns:       Nothing.
@See:
@Include:       navutil.h
@Compatibility:
****************************************************************************/
// 12/16/94 DALLEE, created.

VOID SYM_EXPORT WINAPI GetNavStart(HINSTANCE hInstance, LPSTR lpPath, UINT uSize)
{
    auto        char            szFilename [SYM_MAX_PATH+1];

    GetStartDir (hInstance, lpPath, uSize);
    NameAppendFile(lpPath, GetNavStartFilename(szFilename));
}

/*@API:**********************************************************************
@Declaration:
VOID SYM_EXPORT WINAPI GetNavexInfFile(HINSTANCE hInstance,
                                       LPSTR lpPath,
                                       UINT uSize)

@Description:
This function returns the full path to NAVEX15.INF located in the program
directory.

@Parameters:
$hInstance$     Instance of .EXE for program directory.
$lpPath$        Buffer to place path.
$uSize$         size of buffer.

@Returns:       Nothing.
@See:
@Include:       navutil.h
@Compatibility:
****************************************************************************/
// 12/16/94 DALLEE, created.

VOID SYM_EXPORT WINAPI GetNavexInfFile(HINSTANCE hInstance, LPSTR lpPath, UINT uSize)
{
    GetStartDir (hInstance, lpPath, uSize);
    NameAppendFile(lpPath, "NAVEX15.INF");
}

/*@API:**********************************************************************
@Declaration:
LPSTR SYM_EXPORT WINAPI GetNavStartFilename (LPSTR lpFilename)

@Description:
This function returns the name of the NAVSTART.DAT startup options file.

@Parameters:
$lpFilename$    Buffer to place name.

@Returns:   Address of filename (lpFilename)
@See:
@Include:   navutil.h
@Compatibility:
****************************************************************************/
// 12/16/94 DALLEE, created.

LPSTR SYM_EXPORT WINAPI GetNavStartFilename(LPSTR lpFilename)
{
    return(STRCPY(lpFilename, "NAVSTART.DAT"));
}


LPSTR SYM_EXPORT WINAPI GetSysIntegFilename(LPSTR lpFilename)
{
    return(STRCPY(lpFilename, "NAVSYSR.DAT"));
}

//************************************************************************
// NavSelfTest()
//
// This routine verifies the program has not changed by checking
// the self-test CRC.
//
// Parameters:
//      HINSTANCE   hInstance           WIN - program instance
//                                      DOS - garbage
//
// Returns:
//      SELF_TEST_SUCCESS    (0)        Passed self-test
//      SELF_TEST_FAIL       (2)        Bad CRC
//      SELF_TEST_FILE_ERROR (3)        Could not open/read executable.
//      1                               Program never CRC stamped.
//      ERR (-1)                        Memory allocation error.
//************************************************************************
// 6/14/93 DALLEE, Function created.
//10/16/94 DALLEE, call VirusScanSaveMemoryInfo() on fail.
//************************************************************************

WORD SYM_EXPORT WINAPI NavSelfTest (HINSTANCE hInstance)
{
    auto    LPVOID      lpBuffer;
    auto    WORD        wResult = ERR;
    auto    char        szProgramName [SYM_MAX_PATH + 1];

#ifdef SYM_WIN
    GetModuleFileName(hInstance, szProgramName, SYM_MAX_PATH);
#else
    DOSGetProgramName(szProgramName);
#endif


    lpBuffer = MemAllocPtr (GHND, SIZE_WORK_BUFFER );

    if (lpBuffer != NULL )
        {
        wResult = FileSelfTest(szProgramName, lpBuffer);

        if (SELF_TEST_FAIL == wResult)
            {
            GetInfectionData ((LPBYTE) lpBuffer,NULL, 0);
            }

        MemFreePtr(lpBuffer);
        }

    return (wResult);

} // End NavSelfTest()


//************************************************************************
// ParseZipFileName()
//
// Separates a zip file and path (C:\WORK\STUFF.ZIP#FILE.NAM) into
// zip path (C:\WORK\STUFF.ZIP) and file name inside zip (FILE.NAM).
//
// Parameters:
//      LPSTR lpFullPath                Full path to zip, plus name of
//                                          compressed file in zip.
//      LPSTR lpZipPath                 Return buffer for full path to zip.
//      LPSTR lpFileName                Return buffer for compressed filename.
//
// Returns:
//      Nothing
//************************************************************************
// 7/12/93 DALLEE, This header and moved out of #ifdef SYM_WIN section.
//************************************************************************

VOID SYM_EXPORT WINAPI ParseZipFileName (LPSTR lpFullPath, LPSTR lpZipPath,
                                     LPSTR lpFileName)
{
    auto    UINT    i;

    for (i = STRLEN(lpFullPath); i; i--)
        if (lpFullPath[i] == ZIP_SEPARATOR)
            break;

    if (lpFullPath[i] == ZIP_SEPARATOR)
        {
        STRNCPY (lpZipPath, lpFullPath, i);
        lpZipPath[i] = EOS;
        NOemToAnsi( &lpFullPath[i+1], lpFileName);
        }
    else
        {
        *lpZipPath  = EOS;
        *lpFileName = EOS;
        }
}

#ifdef SYM_WIN16

// ---------------------------------------------------------------------------
//  We don't want this function for W32, The quack NameShortenFilename() should
//  be used.
// ---------------------------------------------------------------------------

VOID SYM_EXPORT WINAPI MakePathShorter (LPSTR lpPath, UINT uLevels)
{
    auto    char    szFullPath[SYM_MAX_PATH+1];
    auto    UINT    uSlash = 0;
    auto    UINT    i;

    MEMSET (szFullPath, EOS, sizeof(szFullPath));

    for (i=STRLEN(lpPath); i; i--)
        if (lpPath[i] == '\\')
            uSlash++;

    if (uSlash > uLevels)
        {
        for (i=0, uSlash = 0; uSlash != (uLevels-1); i++)
            {
            if (lpPath[i] == '\\')
                uSlash++;
          
            szFullPath[i] = lpPath[i];
            }

        STRCAT (szFullPath, "...");

        for (i=STRLEN(lpPath), uSlash=0; uSlash != 1; i--)
            {
            if (lpPath[i] == '\\')
                uSlash++;
            }
        STRCAT (szFullPath, &lpPath[i+1]);

        STRCPY (lpPath, szFullPath);
        }
}

#endif // #ifdef SYM_WIN16

//************************************************************************
// FileIsDOSExecutable()
//
// Returns whether the file extension matches the list of standard DOS
// executables.
//
// Parameters:
//      LPCSTR  lpszFile                Filename to check
//
// Returns:
//      TRUE                            Extension matches std DOS executable
//      FALSE                           Does not match.
//************************************************************************
// 10/19/93 DALLEE, Function created.
//************************************************************************

BOOL SYM_EXPORT WINAPI FileIsDOSExecutable (
    LPSTR   lpszFile )
{
    static  LPSTR   lpExeList[] =
        {
        "COM",
        "EXE",
        NULL
        };

    auto    BOOL    bReturn = FALSE;
    auto    char    szExt [SYM_MAX_PATH+1];

    for ( UINT uIndex = 0; lpExeList[uIndex] && bReturn == FALSE; uIndex++ )
        {
        NameReturnExtensionEx(lpszFile, szExt, sizeof(szExt));

        bReturn = (0 == STRICMP(szExt,  lpExeList[uIndex]));
        }

    return ( bReturn );
}

/* **************************************************************************
@Name:
VOID SYM_EXPORT WINAPI TalkToVxD (      // Communicate with VXD
    VXDACTION   wVxDAction              // [in] action to perform
    );

@Description:
This function communicates with the NAV AP VXD.

@Parameters:
$wVxDAction$  Action indicator specifying to message we wana get across.
              At present time it can any of the following:

              VXDACTION_ENABLE:     To enable the VXD for this thread
              VXDACTION_DISABLE:    To disable the VXD for this thread

@Include: navapcom.h
************************************************************************** */

VOID SYM_EXPORT WINAPI TalkToVxD (      // Communicate with VxD
    VXDACTION   wVxDAction              // [in] Action to tell VxD to do
    )
{
                                        // Do this at runtime, so we
                                        // don't have to split the DLL:
if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
    {
                                        // If this is Gargoyle, don't
                                        // try to talk to the VxD. Nobody's
                                        // home.
    return;
    } 

#ifdef SYM_WIN32
    auto    HANDLE  hNAVAP;

                                        // Open up a communications channel
                                        // with the Vxd
    hNAVAP = CreateFile (NAVAP_VXD_COMM_NAME, 0, 0, 0, 0, 0, 0);
    if ( hNAVAP != INVALID_HANDLE_VALUE )
        {

                                        // Tell the VxD what we want it
                                        // to do.
        switch ( wVxDAction )
            {
            case VXDACTION_ENABLE:
                                        // Enable the VxD for the
                                        // current thread.
                DeviceIoControl ( hNAVAP, ID_COMMAND_DELETE_PROCESS_FROM_PEL,
                                  NULL, 0, NULL, 0, NULL, NULL );
            break;

            case VXDACTION_DISABLE:
                                        // Disable the VxD for the
                                        // current thread.
                DeviceIoControl ( hNAVAP, ID_COMMAND_ADD_PROCESS_TO_PEL,
                                  NULL, 0, NULL, 0, NULL, NULL );
                break;

            case VXDACTION_RELOAD_CONFIG:
                                        // Tell VxD to reload its config
                                        // file.
                DeviceIoControl ( hNAVAP, ID_COMMAND_NAVAP_LOAD_CONFIG,
                                  NULL, 0, NULL, 0, NULL, NULL );
                break;
            
            default:
                SYM_ASSERT ( 0 );
            }

                                        // Close the communications handle.
        CloseHandle ( hNAVAP );
        }
#endif
}



/* **************************************************************************
@Name:
VOID SYM_EXPORT WINAPI NavOptsLoadVxDChanged (
    HWND        hWnd,                   // [in] Handle of parent window
    BOOL        bNewVxDLoad);           // [in] New state of VxD load with WIN

@Description:
This function handles the case where the user has changed whether or not
the VxD is automatically loaded with Windows.

@Parameters:
$hWnd$          [in] Handle of parent window.
$bNewVxDLoad$   [in] The new setting for whether or not the VxD should be
                     automatically loaded with windows.

@Include: navutil.h
************************************************************************** */

VOID SYM_EXPORT WINAPI NavOptsLoadVxDChanged (
    HWND        hWnd,                   // [in] Handle of parent window
    BOOL        bNewVxDLoad)            // [in] New state of VxD load with WIN
{
#ifdef SYM_WIN32
    auto   BOOL   bVxDCurrentlyRunning  = FALSE;
    auto   WORD   wLoadVxD              = IDNO;
    auto   char   szCaption [80];
    extern HINSTANCE hInstance;

                                        // See if the VxD is currently 
                                        // running.
    if (FindWindow ( NAVAP_WND_CLASS, NULL ) != NULL)
        {
        bVxDCurrentlyRunning = TRUE;
        }

                                        // Load our caption.
    LoadString (hInstance, IDS_VXDAUTOLOAD_CAPTION, 
                szCaption, sizeof(szCaption));

                                        // Prompt the user appropriately.
    if ( bNewVxDLoad )
        {
                                        // If the user now wants to have the
                                        // VxD automatically loaded with
                                        // Windows, give the appropriate
                                        // prompt.
        if ( bVxDCurrentlyRunning )
            {
   	        NMsgBox (hInstance, 
                     hWnd, 
                     ERR_SEV_NUM (ERR_SEV_WARNING, IDS_VXDAUTOLOAD_YES),
                     TRACE_ID, 
                     szCaption, 
                     -1);
            }
        else
            {
   	        wLoadVxD = NMsgBox (hInstance, 
                                hWnd, 
                                ERR_SEV_NUM (ERR_SEV_WARNING, IDS_VXDAUTOLOAD_YES_ASKLOADNOW),
                                TRACE_ID, 
                                szCaption, 
                                MB_YESNO | MB_ICONQUESTION, 
                                NULL);
            if ( wLoadVxD == IDYES )
                {
                                        // Compute the path to auto-protect
                                        // and launch it!
                auto    char    szFullPath [SYM_MAX_PATH];
                auto    char    szAutoProtect [SYM_MAX_PATH];

                GetStartDir (hInstance, szFullPath, sizeof(szFullPath));
                LoadString (hInstance, IDS_AUTOPROTECT, szAutoProtect, sizeof(szAutoProtect));
                NameAppendFile (szFullPath, szAutoProtect);
                SetCursor (LoadCursor(NULL, IDC_WAIT));
                WinExec (szFullPath, SW_SHOW);
                SetCursor (LoadCursor(NULL, IDC_ARROW));
                }
            }
        }
    else
        {
                                        // If the user now doesn't want the
                                        // VxD automatically loaded with
                                        // Windows, give the appropriate
                                        // prompt.
        if ( bVxDCurrentlyRunning )
            {
            
   	        wLoadVxD = NMsgBox (hInstance, 
                                hWnd, 
                                ERR_SEV_NUM (ERR_SEV_WARNING, IDS_VXDAUTOLOAD_NO_NOWLOADED),
                                TRACE_ID, 
                                szCaption, 
                                MB_YESNO | MB_ICONQUESTION, 
                                NULL);

            if ( wLoadVxD == IDYES )
                {
                                        // Unload Autoprotect and VxD.
                HWND    hNavapW        = FindWindow ( NAVAP_WND_CLASS, NULL );
                UINT    uNavApWMessage = 0L;

                uNavApWMessage = RegisterWindowMessage (NAVW_NAVAPW_COMM);

                if ( uNavApWMessage != NULL )
                    PostMessage ( hNavapW, 
                                  uNavApWMessage, 
                                  NAVW_CMD_UNLOADNAVAPW,
                                  0L);
                }
            }
        else
            {
   	        NMsgBox (hInstance, 
                     hWnd, 
                     ERR_SEV_NUM (ERR_SEV_WARNING, IDS_VXDAUTOLOAD_NO),
                     TRACE_ID, 
                     szCaption, 
                     -1);
            }
        }

#endif
}

#ifdef SYM_WIN

/* **************************************************************************
@Name: BOOL SYM_EXPORT WINAPI SetCustomVersion ( VOID )

@Description:
This function checks the Registry to determine if this is a special custom version
of the AntiVirus. The static uCustomVersion is set as a result.

@Parameters:
    NONE
************************************************************************** */
BOOL SYM_EXPORT WINAPI SetCustomVersion ( VOID )
{
										// By default :
	uCustomVersion = VERSION_STANDARD;

										// Check for the Netscape custom version :
	char szRegValue [SYM_MAX_PATH];

	CFGREGKEY cfg;
	cfg.hkey = HKEY_LOCAL_MACHINE;
	cfg.lpszProduct = NAV_INSTALLKEY_PRODUCT;
	cfg.lpszVersion = NAV_INSTALLKEY_FUNC;
	cfg.lpszProject = NAV_INSTALLKEY_VER;
	cfg.lpszSection = NULL;
	cfg.lpszEntry = NULL;
	cfg.lpszName = NAV_INSTALLKEY_CUSTOMVERSION;
	cfg.lpszSwitch = NULL;
        
	auto LONG  lRegResult;
	auto DWORD dwType, dwValueSize;
	dwType = REG_SZ;
	dwValueSize = sizeof(szRegValue);
            
	lRegResult = 
		ConfigRegQueryValue ( &cfg, NULL, &dwType, (LPBYTE)szRegValue, &dwValueSize );

	if ((lRegResult == ERROR_SUCCESS) && (dwType == REG_SZ))
		{
		if (STRICMP ( szRegValue , NAV_INSTALLKEY_NETSCAPE ) == 0)
			{ 
										// Netscape value was found:
			uCustomVersion = VERSION_NETSCAPE; 
			}
		}

return TRUE;
}                                       // End SetCustomVersion()



/* **************************************************************************
@Name: UINT SYM_EXPORT WINAPI GetCustomVersion ( VOID )

@Description:
Any module can call this function to find out at runtime whether this 
application is a custom version of the AntiVirus.

@Parameters:
    NONE

@Return:
	VERSION_STANDARD					// Standard Norton version
	VERSION_NETSCAPE					// Netscape version

************************************************************************** */
UINT SYM_EXPORT WINAPI GetCustomVersion ( VOID )
{
	return uCustomVersion;
}                                       // End GetCustomVersion()



/* **************************************************************************
@Name: BOOL SYM_EXPORT WINAPI SchedulerIsInstalled ( VOID )

@Description:
This function returns TRUE if the Scheduler is installed. Under W95,
it returns TRUE if NSCHED32.EXE is present in the install directory;
under NT, it returns TRUE if NAVNTSCH.DLL is present.

@Parameters:
    NONE
************************************************************************** */
BOOL SYM_EXPORT WINAPI SchedulerIsInstalled ( VOID )
{
    extern          HINSTANCE           hInstance;
    auto            char                szWork [ SYM_MAX_PATH+1 ];
    auto            char                szFile [ SYM_MAX_PATH+1 ];

    GetStartDir (hInstance, szFile, sizeof(szFile));

    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
        { LoadString (hInstance, IDS_SCHEDULER, szWork, sizeof(szWork)); }
        else                            // Running under NT
        { LoadString (hInstance, IDS_SCHEDULEDLLNAME, szWork, sizeof(szWork)); }

    NameAppendFile(szFile, szWork);

                                        // Determine if szFile present:

    FINDDATA UnusedRec;                 // Required for Quake call
    return ((FileFindFirst(szFile, &UnusedRec)) != INVALID_HANDLE_VALUE);

}                                       // End SchedulerIsInstalled()

#endif

