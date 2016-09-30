/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/name_win.c_v   1.1   20 Aug 1996 18:54:56   MZAREMB  $ *
 *                                                                      *
 * Description:                                                         *
 *      Mostly Windows-specific Name functions.                         *
 *                                                                      *
 * Contains:                                                            *
 *      NameToFull                                                      *
 *      NameIsExecutable                                                *
 *      NameIsExecutableInitDir                                         *
 *      NameReturnAssociate                                             *
 *      NameLooksLikeExecutable                                         *
 *      NameFindExecutable                                              *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/name_win.c_v  $ *
// 
//    Rev 1.1   20 Aug 1996 18:54:56   MZAREMB
// Made some STRCPY() to STRNCPY() changes as we were stepping on the stack 
// trying to copy a string that was larger than SYM_MAX_PATH.  
// 
//    Rev 1.0   26 Jan 1996 20:21:24   JREARDON
// Initial revision.
// 
//    Rev 1.31   23 Apr 1995 13:35:48   DLEVITON
// Remove redundant source in NameFindExecutable.
//
//    Rev 1.30   03 Jan 1995 17:43:34   MARKL
// Fixed for VxD platform.
//
//    Rev 1.29   21 Dec 1994 04:24:34   BRUCE
// Made NUMEROUS changes to support UNCs
//
//    Rev 1.28   18 Nov 1994 14:08:08   BRAD
// Made DBCS stuff a little more efficient
//
//    Rev 1.27   09 Nov 1994 15:00:24   BRAD
// DBCS enabled a few routines
//
//    Rev 1.26   02 Nov 1994 14:42:16   BRAD
// Made changes so DBCS
//
//    Rev 1.25   18 May 1994 15:52:32   MARKK
// Added disk.h
//
//    Rev 1.24   04 May 1994 00:11:02   BRAD
// Don't use NFileOpen()
//
//    Rev 1.23   26 Apr 1994 17:47:40   BRAD
// Removed SYM_MAX_DIR and SYM_MAX_EXT.  Use SYM_MAX_PATH
//
//    Rev 1.22   08 Apr 1994 14:38:56   BRAD
// Fixed bug
//
//    Rev 1.21   21 Mar 1994 00:20:22   BRAD
// Cleaned up for WIN32
//
//    Rev 1.20   15 Mar 1994 12:33:46   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.19   25 Feb 1994 12:22:52   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.18   09 Feb 1994 18:55:54   PGRAVES
// Win32.
//
//    Rev 1.16   29 Sep 1993 12:04:50   DWHITE
// Add Double byte support to many functions. All changes are between
// #IFDEF DBYTECHARS
//
//
//    Rev 1.15   02 Jul 1993 08:52:06   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//
//    Rev 1.14   17 Jun 1993 19:56:04   MARKL
// SHELL.DLL is now dynamically loaded by SymKrnl.  The rouitines RegOpenKey(),
// RegCloseKey(), RegQueryValue(), and DoEnvironmentSubst() are now called
// through function pointers.  In the case of DOS, a function pointer has
// been established for DoEnvironmentSubst() to allow consistant code in both
// the Windows and DOS platforms.
//
//    Rev 1.13   26 Feb 1993 00:17:56   BRAD
// Fixed problem with NameToFull().
//
//    Rev 1.12   24 Feb 1993 23:58:00   BRAD
// Removed ASM code for DiskGet().
//
//    Rev 1.11   24 Feb 1993 22:01:00   BRAD
// Removed warning.
//
//    Rev 1.10   24 Feb 1993 21:11:00   BRAD
// NameToFull() would not work correctly, unless the path
// was a full path.  The problem was that it was checking
// for ERROR from DirGet(), rather than ERR.
//
//    Rev 1.9   15 Feb 1993 21:19:04   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.8   15 Feb 1993 08:35:56   ED
// Removed StringSkipWhite, which used to be a local copy of the original.
//
//    Rev 1.7   15 Feb 1993 08:25:42   ED
// Removed local copies of functions that used to be in other DLLs.  Now
// that we are one big happy family, everything is "local".
//
//    Rev 1.6   12 Feb 1993 04:29:04   ENRIQUE
// No change.
//
//    Rev 1.5   05 Jan 1993 14:20:48   BILL
// Changed NameIsExecutable to take a LPCSTR first argument by copying
// the input into a temp buffer (stack based)
//
//    Rev 1.4   10 Sep 1992 17:54:26   BRUCE
// Removed unnecessary includes
//
//    Rev 1.3   08 Sep 1992 13:48:22   ED
// Added 'A' to drive letter in NameToFull
//
//    Rev 1.2   08 Sep 1992 12:55:32   HENRI
// Removed use of __DirGet for the DOS platform because it
// duplicates DirGet.
//
//    Rev 1.1   03 Sep 1992 08:57:04   ED
// Fixed parameter lists
//
//    Rev 1.0   27 Aug 1992 15:19:58   ED
// Initial revision.
 ************************************************************************/

#include <dos.h>
#include "platform.h"
#include "file.h"
#include "disk.h"
#include "xapi.h"
#include "symkrnl.h"
#include "dbcs.h"



#ifdef SYM_WIN
/************************************************************************/
/*  LOCAL DATA                                                          */
/************************************************************************/

static  BYTE    szWindows[]     = "Windows";
static  BYTE    szPrograms[]    = "Programs";
static  BYTE    szExtensions[]  = "Extensions";
static  BYTE    szDefaultExts[] = "exe com pif bat";
static  PBYTE   pProgExts;


/************************************************************************/
/*  LOCAL PROTOS                                                        */
/************************************************************************/

BOOL LOCAL PASCAL NameGetProgExts(VOID);
BOOL LOCAL PASCAL NExecutableInThisDir(LPSTR lpPath);


/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      Evaluate a filename to see if it will execute under Windows and */
/*      (if requested) return the fully qualified path to that file     */
/*                                                                      */
/*                                                                      */
/*     INPUT:                                                           */
/*          lpPath      -   filename to check                           */
/*          lpFullPath  -   Ptr to buffer that proc fills with fully    */
/*                          qualified path to executable file. (Or the  */
/*                          caller can pass NULL if he doesn't care     */
/*                          about that                                  */
/*                                                                      */
/*     OUTPUT:                                                          */
/*        NOERR   lpPath is executable                                  */
/*        ERR     lpPath is not executable                              */
/*                                                                      */
/* See Also:                                                            */
/*      NWinExec()                                                      */
/************************************************************************/
/* 3/26/91 VS  Function created.                                       */
/************************************************************************/

UINT SYM_EXPORT WINAPI NameIsExecutable(LPCSTR lpPath, LPSTR lpFullPath)
{
    auto    BYTE            byTemp[128];
    auto    BYTE            byWork[SYM_MAX_PATH];
    auto    UINT            wErr;
    auto    PBYTE           p;
    auto    OFSTRUCT        of;
    auto    int             nPathLen;


    if(!pProgExts)
        NameGetProgExts();

    wErr = (UINT) ERR;
    STRNCPY(byWork,lpPath, SYM_MAX_PATH);
    NameReturnExtension(byWork,byTemp);

                                        // This kludge was added to
                                        // accomodate the 'a:setup' case
                                        // where no extension is entered
                                        // and we need to imitate Progman's
                                        // way of cd'ing to the directory
                                        // and looping through the exectuable
                                        // extensions
    if(!*byTemp)
                                        // This call with TACK ON the
                                        // extension if it finds one
        if(NExecutableInThisDir(byWork))
            {
                                        // From now on we act as if the
                                        // caller had an extension on the
                                        // path the whole time
            NameReturnExtension(byWork,byTemp);
            }

                                        // Does it already have an extension?
    if(*byTemp)
        {
        AnsiUpper(byTemp);

        p = pProgExts;
        while(*p)
            {
                                        // Does it match one of win.ini's
                                        // programs extensions?
            if(!STRCMP(byTemp,p+1))
                {
                wErr = (UINT) ERR;
                                        // Let Windows API find the actual
                                        // path to the file
                if(OpenFile (byWork, &of,OF_EXIST) != (HFILE)-1)
                    {
                                        // of.struct returns OEM filename
                    if(lpFullPath)
                        OemToAnsi(of.szPathName,lpFullPath);

                    wErr = NOERR;
                    }
                                        // We're done, don't loop thru any
                                        // more extensions
                break;
                }
            p += SYM_MAX_PATH;
            }

        }
    else
        {
                                        // File passed in does not
                                        // have any extension put on it.
                                        // We'll need to paste on the
                                        // extensions found in the programs
                                        // line of win.ini to see if we
                                        // get a hit.

        nPathLen = STRLEN(byWork);

        if ((nPathLen + SYM_MAX_PATH) < (SYM_MAX_PATH - 3))
                {

            if(wErr != NOERR)
                {

                p = pProgExts;
                while(*p)
                    {
                    STRNCPY(byTemp,byWork, sizeof(byTemp) - 1);
                    STRCAT(byTemp,p);
                    if(OpenFile(byTemp,&of,OF_EXIST) != (HFILE)-1)
                        {
                        if(lpFullPath)
                            OemToAnsi(of.szPathName,lpFullPath);
                        wErr = NOERR;
                        break;
                        }
                    p += SYM_MAX_PATH;
                        }
                }
            }
        }

    return(wErr);

}

//------------------------------------------------------------------------
//
// Description:
//      Evaluate a command line to see if it will execute under Windows
//      and return the fully qualified path to the file or its associated
//      executable.
//
//     INPUT:
//          lpCmdLine       Command line to check.
//          lpInitDir       Startup directory.
//          lpFullExePath   Ptr to buffer that we fill with fully
//                          qualified path to executable file.  (If
//                          this is NULL, we don't try to fill it.)
//
//     Returns:
//        NOERR   lpPath is executable
//        ERR     lpPath is not executable
//
// See Also:
//------------------------------------------------------------------------
// 4/23/92 PG  Function created.
//------------------------------------------------------------------------

int SYM_EXPORT WINAPI NameIsExecutableInitDir (LPCSTR lpCmdLine, LPCSTR lpInitDir, LPSTR lpFullExePath)
{
    char szCmdLine[256];
    char szExeName[256];
    char szPath[SYM_MAX_PATH];
    LPSTR lp;
    OFSTRUCT of;

                                        // Initialize output buffer.
    if (lpFullExePath != NULL)
        *lpFullExePath = EOS;

                                        // Copy command line and get first token.
    STRNCPY(szCmdLine, lpCmdLine, SYM_MAX_PATH);
    lp = StringSkipToWhite(szCmdLine);
    if (lp)
        *lp = EOS;

                                        // If the token itself is executable,
                                        // we're done.
    if (NameIsExecutable(szCmdLine, lpFullExePath) == NOERR)
        return NOERR;

                                        // The token itself is not executable.
                                        // Try appending the startup directory.
    if (!(NameGetType (szCmdLine) & NGT_IS_FULL) &&
                        lpInitDir != NULL && *lpInitDir != EOS)
        {
        STRNCPY(szPath, lpInitDir, SYM_MAX_PATH);
        NameAppendFile(szPath, szCmdLine);
        if (NameIsExecutable(szPath, lpFullExePath) == NOERR)
            return NOERR;
        }

                                        // Not an executable file.  Does it exist
                                        // at all?
    if (OpenFile(szCmdLine, &of, OF_EXIST) != HFILE_ERROR ||
        OpenFile(szPath, &of, OF_EXIST) != HFILE_ERROR)
        {
                                        // File does exist.  See if it has an
                                        // association and dereference it.
        char szExt[SYM_MAX_PATH];
        NameReturnExtension(szCmdLine, szExt);
        if (NameFindExecutable(szExt, szExeName))
            {
                                        // If the associated executable checks out,
                                        // we're done.
            if (NameIsExecutable(szExeName, lpFullExePath) == NOERR)
                return NOERR;

                                        // Didn't work.  Maybe we can append the
                                        // startup directory...
            if (!(NameGetType(szExeName) & NGT_IS_FULL) &&
                lpInitDir != NULL &&
                *lpInitDir != EOS)
                {
                STRNCPY(szPath, lpInitDir, SYM_MAX_PATH);
                NameAppendFile(szPath, szExeName);
                if (NameIsExecutable(szPath, lpFullExePath) == NOERR)
                    return NOERR;
                }
            }
        }

                                        // Nothing worked.  Give up and go home.
    if (lpFullExePath != NULL)
        STRNCPY(lpFullExePath, szCmdLine, SYM_MAX_PATH);

    return ERR;
}

//------------------------------------------------------------------------
//
// Description:
//      Evaluate a filename to see if it will execute under Windows and
//      return the fully qualified path to that file or it's associated
//      executable if the file's extension lives in the [Extensions]
//      section of WIN.INI
//
//
//     INPUT:
//          lpFileName    - filename to check
//          lpFullExePath - Ptr to buffer that proc fills with fully
//                          qualified path to executable file.
//
//     OUTPUT:
//        NOERR   lpPath is executable
//        ERR     lpPath is not executable
//
// See Also:
//      NWinExec()
//------------------------------------------------------------------------
// 3/26/91 VS  Function created.
// 02-14-92 2:47pm PG Modified to use NameFindExecutable, which looks in
//                    the registration database as well as win.ini.
//------------------------------------------------------------------------
UINT SYM_EXPORT WINAPI NameReturnAssociate(LPCSTR lpFileName, LPSTR lpFullExePath)
{
    auto    BYTE        byExt[SYM_MAX_PATH];
    auto    BYTE        byExeName[SYM_MAX_PATH];
    auto    BYTE        szTemp[SYM_MAX_PATH];
    auto    OFSTRUCT    of;

    STRNCPY (szTemp, lpFileName, SYM_MAX_PATH);

    *lpFullExePath = EOS;

    if (NameIsExecutable(szTemp, lpFullExePath) == ERR)
        {
        NameReturnExtension(szTemp, byExt);
        if (NameFindExecutable(byExt, byExeName) &&
            OpenFile(byExeName, &of, OF_EXIST) != HFILE_ERROR)
            {
            STRCPY(lpFullExePath, (LPSTR) of.szPathName);
            }
        }

    return (*lpFullExePath ? NOERR : ERR);
}

UINT SYM_EXPORT WINAPI NameLooksLikeExecutable(LPCSTR lpFilePath)
{
    auto    BYTE    byExt[SYM_MAX_PATH];
    auto    UINT    wErr;
    auto    PBYTE   p;


    if(!pProgExts)
        NameGetProgExts();

    NameReturnExtension(lpFilePath,byExt);
    AnsiUpper(byExt);

    wErr = (UINT) ERR;
    p = pProgExts;
    while(*p)
        {
        if(!STRCMP(byExt,p+1))
            {
            wErr = NOERR;
            break;
            }
        p += SYM_MAX_PATH;
        }

    return(wErr);
}

//------------------------------------------------------------------------
//
//      NameFindExecutable
//
// Description:
//      Finds the executable file associated with a given extension.
//      Looks first in the registration database, then in win.ini.  The
//      string returned in lpExeName is the name of the executable only
//      (no switches or command line arguments).  The passed-in extension
//      can contain a leading '.' or not, makes no never mind.
//
//------------------------------------------------------------------------
// 02-14-92 12:18pm PG Function created.
//------------------------------------------------------------------------
BOOL SYM_EXPORT WINAPI NameFindExecutable(LPCSTR lpExt, LPSTR lpExeName)
{
    char szExt[6];
    HKEY hk;
    char szExeName[SYM_MAX_PATH];
    const static char szShellOpenCommand[] = "\\shell\\open\\command";
    LPSTR lp;

                                        // Make sure the extension includes the
                                        // leading '.' so we can look it up in the
                                        // registration database.
    if (*lpExt == '.')
        STRCPY(szExt, lpExt);
    else
        {
        szExt[0] = '.';
        STRCPY(szExt+1, lpExt);
        }

    szExeName[0] = EOS;

    if (RegOpenKey(HKEY_CLASSES_ROOT, szExt, &hk) == ERROR_SUCCESS)
        {
                                        // The extension is indeed in the
                                        // registration database.
        char szClass[128];
        LONG cb = sizeof(szClass);

        if (RegQueryValue(hk, NULL, (LPSTR) szClass, &cb) == ERROR_SUCCESS &&
            szClass[0] != EOS)
            {
                                        // The extension is associated with a
                                        // class name.  Get the name of the
                                        // program associated with the class.
            cb = sizeof(szExeName);
            STRCAT(szClass, szShellOpenCommand);
            RegQueryValue(HKEY_CLASSES_ROOT, (LPSTR) szClass, (LPSTR) szExeName, &cb);
            }
        else
            {
                                        // The extension is NOT associated with a class
                                        // name.  Look for ...\shell\open\command.
            char szSubKey[64];
            STRCPY(szSubKey, szExt);
            STRCAT(szSubKey, szShellOpenCommand);
            cb = sizeof(szExeName);
            RegQueryValue(HKEY_CLASSES_ROOT, (LPSTR) szSubKey, (LPSTR) szExeName, &cb);
            }

        RegCloseKey(hk);
        }
    else
        {
                                        // The extension is NOT in the registration
                                        // database.  Try win.ini.
        GetProfileString(szExtensions,
                         &szExt[1],
                         "",
                         szExeName,
                         sizeof(szExeName));
        }

                                        // Chop off switches, command line arguments,
                                        // etc.
    lp = STRRCHR(szExeName, ' ');
    if (lp)
        *lp = EOS;

                                        // Copy results into the supplied buffer.
    STRCPY(lpExeName, (LPSTR) szExeName);

                                        // Return a flag indicating whether we
                                        // found an association.
    return (*lpExeName == EOS ? FALSE : TRUE);
}


/************************************************************************/
/*  LOCAL FUNCTIONS                                                     */
/************************************************************************/

BOOL LOCAL PASCAL NameGetProgExts(VOID)
{
    auto    BYTE        byTemp[128];
    auto    PBYTE       p,pLastExt;
    auto    LPSTR      lp;
    auto    UINT        iLen;



    iLen = GetProfileString(szWindows,szPrograms,szDefaultExts,
                                                byTemp,sizeof(byTemp));
    AnsiUpper(byTemp);

    pProgExts = (PBYTE)LocalAlloc(LPTR,(iLen/3) * SYM_MAX_PATH);

    lp = byTemp;
    p  = pLastExt = pProgExts;
    *p++ = '.';
    while(*lp)
        {
        if(*lp == ' ')
            {
            p = (pLastExt += SYM_MAX_PATH);
            lp++;
            *p++ = '.';
            }
        else
            {
            if (DBCSIsLeadByte(*lp))
                {
                *p++ = *lp++;
                }
            *p++ = *lp++;
            }
        }

    return(TRUE);

}


/************************************************************************/
/*  NExecutableInThisDir is 'helper' function for NameIsExecutable    */
/*  and is NOT meant to be called from anywhere else. It will loop thru */
/*  the executable extensions in win.ini programs line and try to find  */
/*  a file in the current directory (or the one attatched to the name   */
/*  passed in.                                                          */
/************************************************************************/

BOOL LOCAL PASCAL NExecutableInThisDir(LPSTR lpPath)
{
    auto    BYTE            fn[SYM_MAX_PATH];
    auto    BYTE            fullpath[SYM_MAX_PATH];
    auto    PBYTE           p;
    auto    HFILE           hTemp;

                                        // Convert 'a:setup' to 'a:\setup'
    if(!DBCSIsLeadByte(*lpPath) &&
                (*(lpPath+1) == ':') && (*(lpPath+2) != PATH_CHAR)
                && (*(lpPath+2) != '.'))
        {
        DirGet(*lpPath,fullpath);
        NameAppendFile(fullpath,lpPath+2);
        }
    else
        STRNCPY(fullpath,lpPath, SYM_MAX_PATH);

                                        // Assuming 'pProgExts' has already
                                        // been setup by caller
    p = pProgExts;

    while(*p)
        {
                                        // Assuming that lpPath has no
                                        // extension, as determined by
                                        // caller
        STRCPY(fn,fullpath);
        STRCAT(fn,p);
                                        // Can't use FindFirst() because
                                        // of a bug in windows: FindFirst
                                        // fails (even if the file is there)
                                        // when called from the A: drive
                                        // looking for a file on the A: drive
        if((hTemp = FileOpen(fn, OF_READ)) != (HFILE_ERROR))
            {
            FileClose(hTemp);
                                        // Tack this extension onto the
                                        // the caller's path
            STRCAT(lpPath,p);
            return(TRUE);
            }

        p += SYM_MAX_PATH;
        }

    return(FALSE);
}

#endif                                  // #ifdef SYM_WIN

////////////////////////////////////////////////////////////////////////
//
//   NameToFull()
//
// Description:
//
//   This procedure takes a file name and converts it into it's full,
//   global path name.
//
//   If the spec is UNC and is a full spec, just copy it to the output buffer.
//   If the UNC spec is NOT full, then if Win32, returns the output
//   from the Win32 API GetFullPathName().
//   If NOT a full UNC spec and NOT Win32, then return an error.
//
//   Returns a path name with the form:
//        A:\WRITING\UTIL\name
//
//   NOTE: Dest and Src can NOT be the same buffer.
//
//   The lpDest buffer MUST be at least SYM_MAX_PATH in length.
//
//   NOTE: This function used to call NameCompact() on the input - it
//         now operates on a local copy and does not modify the input.
//         If there was code depending on this side affect, it is now
//         broken!!
//
//   Returns:     ERR     If there was an error
//                NOERR   If there was not an error
//
// See Also:
//   Win32 API: GetFullPathName()
//
////////////////////////////////////////////////////////////////////////
// 12/20/94 BEM Function Header updated.
// 12/20/94 BEM Made to work on local copy of input.
////////////////////////////////////////////////////////////////////////

#pragma optimize("", off)
UINT SYM_EXPORT WINAPI NameToFull (LPSTR lpDest, LPCSTR lpSrc)
{
    BYTE        byDrive;                // Disk drive letter
    UINT        uType;
                                        // Our copy of the source to
                                        // party on so that we don't trash
                                        // the original.
    char        szSource[SYM_MAX_PATH+1];

    STRNCPY(szSource, lpSrc, sizeof(szSource) - 1);
    szSource[sizeof(szSource)-1] = EOS;

    NameCompact(szSource);              // Remove "name\.." and ".\"

    uType = NameGetType(szSource);
    if (uType & NGT_IS_UNC)
        {
        if (uType & NGT_IS_FULL)
            {
            STRNCPY(lpDest, szSource, SYM_MAX_PATH - 1);
            return NOERR;
            }
#if defined(SYM_WIN32)
                                        // For Win32, let Windows do the
                                        // work for non-complete UNCs
        if (!GetFullPathName(szSource, SYM_MAX_PATH, lpDest, NULL))
            return (UINT)ERR;
        else
            return(NOERR);
#else                                   // #ifdef SYM_WIN32
                                        // non Win32 environments
                                        // return an error on imcomplete UNCs
                                        // it would be too expensive otherwise
                                        // (we'd have to see if
                                        // uType & UNC_IS_SERVER and then
                                        // figure out the current volume
                                        // and directory)  If not
                                        // uType & UNC_IS_SERVER, then it's
                                        // definitely an error.
        return(ERR);
#endif                                  // #ifdef SYM_WIN32/#else
        }                               // if (uType & NGT_IS_UNC)

                                        // Non-UNC names:
    byDrive = (NameHasDriveSpec(szSource)) ? *szSource : DiskGet();

    byDrive = (BYTE) CharToUpper (byDrive);

    FileInitPath (lpDest);              // Put drive and '\' in front
    *lpDest = byDrive;                  // Set the drive letter

                                        // Is this already a full name?
    if (uType & NGT_IS_FULL)
        {
        LPSTR lpSource = szSource;

        if (lpSource[1] == ':')         // If string contained drive
            lpSource += 3;              // then skip over "d:\" part
        else
            lpSource++;                 // else skip over "\" part
                                        // Copy full path over
        STRNCAT (lpDest, lpSource, SYM_MAX_PATH - 1);
        return(NOERR);
        }

    if (DirGet (byDrive, lpDest) == ERR)
        return((UINT) ERR);             // We couldn't read the path

    NameConvertPath(lpDest, szSource);  // Convert to full path name

    return(NOERR);                      // There was no error
}

#pragma optimize("", on)

