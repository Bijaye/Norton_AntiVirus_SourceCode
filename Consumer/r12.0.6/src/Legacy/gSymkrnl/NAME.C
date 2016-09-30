/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/name.c_v   1.19   12 Sep 1997 15:25:32   KSACKIN  $ *
 *                                                                      *
 * Description:                                                         *
 *      Name functions                                                  *
 *                                                                      *
 * Contains:                                                            *
 *      NameStripFile                                                   *
 *      NameStripExtension                                              *
 *      NameReturnExtension                                             *
 *      NameReturnFile                                                  *
 *      NameReturnPath                                                  *
 *      NameReturnLongName                                              *
 *      NameReturnShortName                                             *
 *      NameReturnStartDir                                              *
 *      NameAppendFile                                                  *
 *      NameAppendExtension                                             *
 *      NameToUnpacked                                                  *
 *      NameToPacked                                                    *
 *      NameShortenFileName                                             *
 *      NameIsValidFileChar                                             *
 *      NameConvertPath                                                 *
 *      NameCompact                                                     *
 *      NameIsDevice                                                    *
 *      NameReplacePathChar                                             *
 *      NameTitleFromCmdLine                                            *
 *      NameTruncate                                                    *
 *      NamePathToEnvironment                                           *
 *      NameShortenFileName                                             *
 *      NameParsePath                                                   *
 *      NameSeparatePath                                                *
 *      NameConvertFwdSlashes                                           *
 *                                                                      *
 *                                                                      *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/name.c_v  $ *
// 
//    Rev 1.19   12 Sep 1997 15:25:32   KSACKIN
// Ported changes from QAKG.
// 
//    Rev 1.18   23 Aug 1997 00:01:24   DSACKIN
// Restored changes that I broke.
// 
//    Rev 1.16   22 Aug 1997 11:00:00   DDREW
// Even more things uncovered for NLM platform
// 
//    Rev 1.15   05 Aug 1997 16:46:40   DDREW
// Moved NameReturnFile() out from under #ifndef SYM_NLM
// 
//    Rev 1.14   19 Mar 1997 21:47:20   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.13   03 Jan 1997 18:47:22   RSTANEV
// Closing an orphaned handle in NameReturnShortName().
//
//    Rev 1.12   18 Oct 1996 22:07:34   RSTANEV
// Limited the usage of root handles in SYM_NTK.
//
//    Rev 1.11   21 Aug 1996 09:33:26   MZAREMB
// Destination string was not being created properly for SFNs in NameTruncate().
//
//    Rev 1.10   20 Aug 1996 18:52:22   MZAREMB
// Made some STRCPY() to STRNCPY() changes as we were stepping on the stack
// trying to copy a string that was larger than SYM_MAX_PATH.
//
//    Rev 1.9   13 Aug 1996 22:11:18   RSTANEV
// SYM_NTK: Using FILE_SHARE_DELETE flag in certain cases when calling
// ZvOpenFile().
//
//    Rev 1.8   08 Aug 1996 11:26:04   TIVANOV
// fix a problem with NamereturnShortName and high ASCII chars under NT
//
//    Rev 1.7   22 Jul 1996 18:36:16   RSTANEV
// Fixed a typo.
//
//    Rev 1.6   22 Jul 1996 17:51:28   RSTANEV
// SYM_NTK: Extended the usage of NT_SUCCESS().
//
//    Rev 1.5   24 Jun 1996 19:02:16   RSTANEV
// Now using the Zv...() wraps instead of the Zw...() APIs.
//
//    Rev 1.4   18 Jun 1996 10:57:16   RSTANEV
// Fixed some of the flags passed to ZwOpenFile() in SYM_NTK.
//
//    Rev 1.3   30 Apr 1996 18:59:32   BMCCORK
// Fixed what appears to be an infinite loop in NameGetIllegalChars().
// This needs further examination, since it would appear that the code was
// never executed.  See BEM comments in NameGetIllegalChars() for more
// details.
//
//    Rev 1.2   14 Feb 1996 16:42:22   JWORDEN
// Add NameReturnShortNameHardWay () function to handle Win32 situations
// where the file name contains non-ANSI translatable characters
//
//    Rev 1.1   09 Feb 1996 18:46:56   RSTANEV
// Restored back to use Zw... functions.
//
//    Rev 1.0   26 Jan 1996 20:21:50   JREARDON
// Initial revision.
//
//    Rev 1.141   25 Jan 1996 14:35:50   MARKK
// Fixed bug in NamedToPacked where dst was not being initialized
//
//    Rev 1.140   25 Jan 1996 13:10:14   MARKK
// Commented out ASSERT(FALSE)
//
//    Rev 1.139   24 Jan 1996 16:19:12   RSTANEV
// Implemented NameReturnLongName() and NameReturnShortName() for SYM_NTK.
//
//    Rev 1.138   21 Jan 1996 13:31:40   RSTANEV
// Added Unicode and SYM_NTK support.
//
//    Rev 1.137   10 Nov 1995 10:54:22   GDZIECI
// Cleaned up NLM platform.
//
//    Rev 1.136   06 Nov 1995 12:50:16   DBUCHES
// Fixed NameIsValidFileCharEx().  This function was returning TRUE for
// a NULL character on DBCS platforms, which is incorrect.  Per Shay Ryan,
// NULL is neither a valid lead or trail byte.
//
//    Rev 1.135   02 Aug 1995 01:00:00   AWELCH
// Merge changes from Quake 7.
//
//    Rev 1.134   27 Jul 1995 14:41:20   DBUCHES
// Merged quake 8 changes.
//
//    Rev 1.133   27 Jun 1995 17:46:26   BRUCE
// Modified NameGetIllegalChars so it can take a null buff ptr and calc req size
//
//    Rev 1.132   14 Jun 1995 17:53:00   DBUCHES
// Fixed NameGetIllegalChars() for Windows 95.
//
//    Rev 1.131   06 Jun 1995 10:03:04   BARRY
// Fixed PackedNameIsDevice for DOS and W16 platforms (broken in 1.128)
//
//    Rev 1.130   01 Jun 1995 20:10:24   HENRI
//
//    Rev 1.129   01 Jun 1995 10:40:44   MARKK
// Optimized compare of device names
//
//    Rev 1.128   01 Jun 1995 10:04:06   MARKK
// Broke NameIsDevice up into two functions, adding PackedNameIsDevice
//
//    Rev 1.127   30 May 1995 13:57:30   DBUCHES
// Changed NameIsDevice() for Windows 95.  Now traverses DOS device chain looking
// for match.
//
//    Rev 1.126   21 Apr 1995 14:01:16   TBAHR
// Added check to TrueNameGet to check the allocation of memory
//
//    Rev 1.125   08 Apr 1995 20:36:52   TORSTEN
// Modified TrueNameGet() to use a temporary buffer for the canonicalized
// filename; MS-DOS handled the case of source == destination fine, DR-DOS
// didn't.
//
//    Rev 1.124   14 Mar 1995 02:53:30   BRUCE
// Fixed thunks - DON'T CALL FILESETRETURNCODE BEFORE RESTORING DS
//
//    Rev 1.123   06 Mar 1995 21:35:24   BRUCE
//
// Fixed NameIsValidFileCharEx() so that ' ' and '.' are accepted if bAnyChar.
// Also made lowercase letters valid for SFNs (as it was before).
//
//    Rev 1.122   21 Feb 1995 16:06:30   BRUCE
// Castrated for OS/2
//
//    Rev 1.121   21 Feb 1995 11:07:40   BRUCE
// Fixed NameValidatePathEx() to allow periods in paths (as it did before)
//
//    Rev 1.120   20 Feb 1995 19:16:44   BRUCE
// Updated to match branch fixes for NameCompact() and invalid file char stuff
//
//    Rev 1.119   17 Feb 1995 14:45:20   RSTANEV
// Fixed NameAppendFile() so that it appends correctly "\\..." to "...\\"
// and put only one backslash at the place of concatenation.
//
//    Rev 1.118   09 Feb 1995 12:24:06   BILL
// Removed Symkrnl.h
//
//    Rev 1.117   01 Feb 1995 13:40:04   BRIANF
// Fixed NameGetType to not call DIRGET under OS2.
//
//    Rev 1.116   27 Jan 1995 16:36:42   BRIANF
// Fixed Win16 implementations of NameGetType and
// NameValidatePathEx to properly support UNCs.
//
//    Rev 1.115   10 Jan 1995 18:38:24   MARKL
// Now supports NameReturnLongName()
//
//    Rev 1.114   10 Jan 1995 15:56:52   BRUCE
// Rewrote NameShortenFileNameRect() to just call DrawTextEx()
//
//    Rev 1.113   30 Dec 1994 16:14:30   BRUCE
// Made slight optimization to NameReturnLongName()
//
//    Rev 1.112   30 Dec 1994 14:12:18   JREARDON
// Removed NameReturnLong from NLM platform.  Some of the helper
// functions are not available for this platform.
//
//    Rev 1.111   30 Dec 1994 12:02:46   BRUCE
// Fixed NameIsRoot() for UNC roots
//
//    Rev 1.110   30 Dec 1994 11:25:20   BRUCE
// Fixed NameReturnLongName() on UNC roots
//
//    Rev 1.109   28 Dec 1994 17:54:30   BRUCE
// Added NameShortenFileNameRect()
//
//    Rev 1.108   27 Dec 1994 17:10:40   BRUCE
// Added bFmtForDisp parm to NameReturnLongName()
//
//    Rev 1.107   27 Dec 1994 16:25:30   BRUCE
// Rewrote NameReturnLongName() so that no thunking is needed
//
//    Rev 1.106   22 Dec 1994 03:09:58   JMILLARD
// provide NLM shell for NameGetType
//
//    Rev 1.105   21 Dec 1994 17:37:06   BRAD
// Use WIN32 for GetShortPathName()
//
//    Rev 1.104   21 Dec 1994 16:06:56   BRAD
// Make long/short name functions consistent with Quake APIs.
//
//    Rev 1.103   21 Dec 1994 04:24:24   BRUCE
// Made NUMEROUS changes to support UNCs
//
//    Rev 1.102   09 Dec 1994 19:11:20   BRAD
// Cleaned up for SYM_VXD
//
//    Rev 1.101   09 Dec 1994 17:09:44   BRIANF
// Fixed NameConvertPath to work with UNCs.
//
//    Rev 1.100   09 Dec 1994 17:04:06   BRAD
// Added SYM_VXD support
//
//    Rev 1.99   08 Dec 1994 17:12:56   BRAD
// Added NameIsUNC
//
//    Rev 1.98   08 Dec 1994 14:26:06   BRAD
// AddedNameAppendExtension
//
//    Rev 1.97   07 Dec 1994 15:31:30   JMILLARD
// ifndef SYM_NLM NameTitleFromCmdLine
//
//    Rev 1.96   07 Dec 1994 08:34:12   JMILLARD
//
// clean up some NLM issues
//
//    Rev 1.95   06 Dec 1994 16:40:46   BRAD
// Added NameReturnRoot
//
//    Rev 1.94   29 Nov 1994 18:50:50   JMILLARD
// nuke inline assembler for SYM_NLM
//
//    Rev 1.93   23 Nov 1994 14:07:32   BRUCE
// Disabled namegetlong/short name for os/2
//
//    Rev 1.92   22 Nov 1994 11:55:18   BRUCE
// Fixed namegetlong/short name
//
//    Rev 1.91   21 Nov 1994 19:05:16   BRUCE
// Added NameGetLong/ShortName() functions and thunks to get to them
//
//    Rev 1.90   18 Nov 1994 14:08:04   BRAD
// Made DBCS stuff a little more efficient
//
//    Rev 1.89   09 Nov 1994 14:21:56   BRAD
// DBCS-enabled more routines
//
//    Rev 1.88   04 Nov 1994 15:08:30   BRAD
// Added DBCS support
//
//    Rev 1.87   01 Nov 1994 13:29:12   JMILLARD
// ifdef in some functions for NLM platform, some in a dummy way
//
//    Rev 1.86   26 Sep 1994 18:37:16   MFALLEN
// NameValidatePath(). Incorrect path validations were being done for LFN's.
// A long filename can have several periods between '\' This check is not being
// performed for the WIN32 platform.
//
//    Rev 1.85   23 Sep 1994 16:23:24   TONY
// Quake 4 to Quake 6 changes for OS/2
//
//    Rev 1.84   31 Aug 1994 12:12:24   MFALLEN
// NameGetIllegalChars() was returning 0x20 as an illegal character under Chicago.
//
//
//    Rev 1.83   26 Jul 1994 11:30:20   BRAD
// Moved wildcard routines to wildcard.c
 ************************************************************************/

#ifdef SYM_OS2
#   pragma inline
#endif

#include <dos.h>
#include "platform.h"
#include "xapi.h"
#include "file.h"
#include "disk.h"
//#include "symkrnl.h"
#ifdef SYM_OS2
#include <dir.h>
#endif
#include "dbcs.h"

#ifdef SYM_NLM
#include <ctype.h>
#endif

#ifndef SYM_NLM
static  TCHAR   szDefChars[] =          // default illegal file characters
                _T("\x22\x2B\x2C\x2E\x2F\x3A\x3B\x3C\x3D\x5B\x5C\x5D\x7C");


typedef DeviceDriverRec far *LPDRIVER;

typedef struct  tagFILECHARTABLE
    {
    WORD        fctLength;              // table length, except this field
    BYTE        x1;
    BYTE        fctFirst;               // lowest permissable character value
    BYTE        fctLast;                // highest permissable character value
    BYTE        x2;
    BYTE        fctExcludeFirst;        // first in range of excluded characters
    BYTE        fctExcludeLast;         // last in range of excluded characters
    BYTE        x3;
    BYTE        fctIllegals;            // number of illegal characters in array
    BYTE        fctTable;               // start of array of illegal characters
    } FILECHARTABLE, far *LPFILECHARTABLE;

/************************************************************************/
/*  LOCAL PROTOS                                                        */
/************************************************************************/
LPFILECHARTABLE LOCAL PASCAL GetCharTable (void);
LPDRIVER LOCAL PASCAL DOSGetFirstDriverAddress (void);

#endif  // #ifndef SYM_NLM

LPTSTR   LOCAL PASCAL InsertChar (TCHAR ch, LPTSTR pBuffer, UINT FAR *pwCount, BOOL bPathChars);

LPTSTR   LOCAL PASCAL FindFileName (LPCTSTR lpName, BOOL bSkipSlash);
UINT     LOCAL PASCAL IncDigit (LPTSTR lpLimit, LPTSTR lpPos);
STATUS         WINAPI NameReturnShortNameHardWay (LPTSTR lpszSpec, LPTSTR lpszShortSpec);

/************************************************************************/
/*  Misc externals                                                      */
/************************************************************************/
#if defined(SYM_VXD)
EXTERNC UINT WINAPI VxDNameReturnLongName ( LPCSTR, LPSTR );
EXTERNC UINT WINAPI VxDNameReturnShortName ( LPCSTR, LPSTR );
#endif

#ifndef SYM_NLM             // following routines are not used by SYM_NLM

/*********************************************************************
** NameConvertFwdSlashes()
**
** Description:
**    Changes forward slashes to backslashes (these can come from
**    intercepting int calls where the person generating them uses
**    fwd slashes (like Brief)).
**
** See Also:
**    NameReplacePathChar()
**
**********************************************************************/

UINT SYM_EXPORT WINAPI NameConvertFwdSlashes(LPTSTR lpPath)
{
    while (lpPath && *lpPath)
        {
        if (*lpPath == '/')
            *lpPath = '\\';
        lpPath = AnsiNext(lpPath);
        }

    return(NOERR);
}


////////////////////////////////////////////////////////////////////////
//
//   NameReturnFileEx()
//
// Description:
//      This function gets the file name part of lpszFileSpec and
//      returns the file name in the buffer lpszFileName.
//
//      This routine was taken from the one created by Brad (04/14/89) as
//      part of the DOS Norton Libraries.
//
//      Note that the UNC names: "\\SERVER", and "\\SERVER\VOLUME" do not
//      contain a file and so this function will return an empty buffer for
//      lpFileName in these cases.
//
//
//      The only difference between this function and NameReturnFile()
//      is that this one finds the maximum legal length for the file name
//      component instead of using the NAME_FILENAMESIZE limit.
//
// Return values:
//    NOERR   -- Function completed
//    ERR     -- File name is longer than the maximum file name length
// See Also:
//
// NameReturnFile(), FindFileName()
////////////////////////////////////////////////////////////////////////
// 12/16/94 BEM Header comment updated.
// 12/19/94 BEM Modified to use FindFileName()
////////////////////////////////////////////////////////////////////////
#define SYM_MAX_FILE_SYSTEM_NAME        257


UINT SYM_EXPORT WINAPI NameReturnFileEx(// Return file name
    LPCTSTR     lpszFileSpec,           // File spec containing ext.
    LPTSTR      lpszFileName)           // Buffer to return file name
{
    int         wRet;                   // Component return code
    LPTSTR      lpszFile;               // Ptr to file name portion

                                        // Place to hold null terminated
                                        // name (just to make sure).
    TCHAR       szFileSpec[SYM_MAX_PATH+1];

    DWORD       dwMaximumComponentLength;

    SYM_ASSERT ( lpszFileSpec );
    SYM_VERIFY_STRING ( lpszFileSpec );
    SYM_ASSERT ( lpszFileName );
    SYM_VERIFY_BUFFER ( lpszFileName, NAME_FILENAMESIZE * sizeof(TCHAR) );

    dwMaximumComponentLength = NAME_FILENAMESIZE;
#ifdef SYM_WIN
    {
    DWORD       dwFileSystemFlags;
    DWORD       dwVolumeSerialNumber;
    TCHAR       szVolumeNameBuffer[SYM_MAX_PATH];
    TCHAR       szFileSystemNameBuffer[SYM_MAX_FILE_SYSTEM_NAME];
    TCHAR       szRootName[SYM_MAX_PATH];
    NameReturnRoot(lpszFileSpec, szRootName);

    if ( VolumeInformationGet (
                szRootName,
                szVolumeNameBuffer,
                sizeof(szVolumeNameBuffer)/sizeof(TCHAR),
                &dwVolumeSerialNumber,
                &dwMaximumComponentLength,
                &dwFileSystemFlags,
                szFileSystemNameBuffer,
                sizeof(szFileSystemNameBuffer)/sizeof(TCHAR)) == FALSE)
        {
        return ((UINT)ERR);
        }
    }
#endif
                                        // Make sure we're working on a
                                        // NULL terminated copy of the name.
    STRNCPY(szFileSpec, lpszFileSpec, sizeof(szFileSpec)/sizeof(*szFileSpec));
    szFileSpec[sizeof(szFileSpec)/sizeof(*szFileSpec)-1] = EOS;

    lpszFile = FindFileName(szFileSpec, TRUE);

    if ((STRLEN (lpszFile) + 1) > dwMaximumComponentLength)
        {                                // File name too long!
        *lpszFileName = EOS;
        wRet = (UINT) ERR;
        }
    else
        {                                // We have a valid File name
        STRCPY (lpszFileName, lpszFile);
        wRet = NOERR;
        }

    return (wRet);

} // NameReturnFileEx()

#endif              // #ifndef SYM_NLM

////////////////////////////////////////////////////////////////////////
//
//   NameReturnFile()
//
// Description:
//      This function gets the file name part of lpszFileSpec and
//      returns the file name in the buffer lpszFileName.
//
//      This routine was taken from the one created by Brad (04/14/89) as
//      part of the DOS Norton Libraries.
//
//      Note that the UNC names: "\\SERVER", and "\\SERVER\VOLUME" do not
//      contain a file and so this function will return an empty buffer for
//      lpFileName in these cases.
//
// Return values:
//    NOERR   -- Function completed
//    ERR     -- File name is longer than NAME_FILENAMESIZE (255)
// See Also:
//
// NameReturnFileEx(), FindFileName()
////////////////////////////////////////////////////////////////////////
// 12/16/94 BEM Header comment updated.
// 12/19/94 BEM Modified to use FindFileName()
////////////////////////////////////////////////////////////////////////

UINT SYM_EXPORT WINAPI NameReturnFile ( // Return file name
    LPCTSTR     lpszFileSpec,           // File spec containing ext.
    LPTSTR      lpszFileName)           // Buffer to return file name
{
    int         wRet;                   // Component return code
    LPTSTR      lpszFile;               // Ptr to file name portion

                                        // Place to hold null terminated
                                        // name (just to make sure).
    TCHAR       szFileSpec[SYM_MAX_PATH+1];

    SYM_ASSERT ( lpszFileSpec );
    SYM_VERIFY_STRING ( lpszFileSpec );
    SYM_ASSERT ( lpszFileName );
    SYM_VERIFY_BUFFER ( lpszFileName, NAME_FILENAMESIZE * sizeof(TCHAR) );

                                        // Make sure we're working on a
                                        // NULL terminated copy of the name.
    STRNCPY(szFileSpec, lpszFileSpec, sizeof(szFileSpec)/sizeof(*szFileSpec));
    szFileSpec[sizeof(szFileSpec)/sizeof(*szFileSpec)-1] = EOS;

    lpszFile = FindFileName(szFileSpec, TRUE);

    if ((STRLEN (lpszFile) + 1) > NAME_FILENAMESIZE)
        {                                // File name too long!
        *lpszFileName = EOS;
        wRet = (UINT) ERR;
        }
    else
        {                                // We have a valid File name
        STRCPY (lpszFileName, lpszFile);
        wRet = NOERR;
        }

    return (wRet);
} // NameReturnFile()

/*----------------------------------------------------------------------*
 * This procedure strips the file name off the end of a path to leave   *
 * just the path name.  We have to make sure we catch several special   *
 * cases:                                                               *
 *    '\', 'x:', 'x:\'                                                  *
 *----------------------------------------------------------------------*/

LPTSTR SYM_EXPORT WINAPI NameStripFile (LPTSTR lpName)    // The full name
{
    LPTSTR      p;

    SYM_ASSERT ( lpName );
    SYM_VERIFY_STRING ( lpName );

    p = FindFileName (lpName, FALSE);

    *p = '\0';                          // Leave behind just path name

    return (lpName);
}

////////////////////////////////////////////////////////////////////////
//
//   NameReturnExtension()
//
// Description:
//     This function gets the extension part of lpszFileSpec and
//     returns the extension in the buffer lpszExtension.
//
//     An extension is now defined as the text following the last period
//     in a file spec.  Since so much legacy code exists and assumes
//     that an extension buffer is only 4 bytes long, this function will
//     just call NameReturnExtensionEx() with 4 as the length of the
//     extention buffer.
//
//     This routine was taken from the one created by David (06/13/89) as
//     part of the DOS Norton Libraries.
//
//Return values:
//   NOERR                             -- Function completed
//
// See Also:
//   NameReturnExtensionEx()
//
////////////////////////////////////////////////////////////////////////
// 12/16/94 BEM Comment Header Updated.
////////////////////////////////////////////////////////////////////////

UINT SYM_EXPORT WINAPI NameReturnExtension (
    LPCTSTR     lpszFileSpec,
    LPTSTR      lpszExtension)
{
    return(NameReturnExtensionEx(lpszFileSpec, lpszExtension, 4));
}  // NameReturnExtension()

////////////////////////////////////////////////////////////////////////
//
//   NameReturnExtensionEx()
//
// Description:
//     This function gets the extension part of lpszFileSpec and
//     returns the extension in the buffer lpszExtension.
//
//     An extension is now defined as the text following the last period
//     in a file spec.
//
//      The cbExtBuffSize parameter contains the size of the buffer in
//      number of characters, not number of bytes!!!
//
//Return values:
//   NOERR                             -- Function completed
//
// See Also:
//   NameReturnExtension()
//
////////////////////////////////////////////////////////////////////////
// 12/16/94 BEM Function created by copying NameReturnExtension() and
//              changing the 3s to cbExtBuffSize-1
////////////////////////////////////////////////////////////////////////

UINT SYM_EXPORT WINAPI NameReturnExtensionEx(
    LPCTSTR     lpszFileSpec,
    LPTSTR      lpszExtension,
    UINT        cbExtBuffSize)
{
    auto        UINT    wRet;           // Component return code
    auto        LPCTSTR lpszFileIndex;  // Index into lpszFileSpec
    register    TCHAR   ch;

    SYM_ASSERT ( lpszFileSpec );
    SYM_VERIFY_STRING ( lpszFileSpec );
    SYM_ASSERT ( lpszExtension );
    SYM_VERIFY_BUFFER ( lpszExtension, cbExtBuffSize * sizeof(TCHAR) );

                                        // Initialize index to point to
                                        // '\0' in string.

    lpszFileIndex = lpszFileSpec + STRLEN(lpszFileSpec);

                                        // Backup to '\', ':', or '.'
    while ((lpszFileIndex = AnsiPrev (lpszFileSpec, lpszFileIndex))
           > lpszFileSpec)
       {
       ch = *lpszFileIndex;

       if (ch == ':' || ch == '\\' || ch == '.')
            break;                      // We found character
       }

    if (*lpszFileIndex == '.')          // Was there an extension?
       {
        lpszFileIndex = AnsiNext (lpszFileIndex);

        STRNCPY(lpszExtension, lpszFileIndex, cbExtBuffSize-1);
        lpszExtension[cbExtBuffSize-1] = EOS;
        wRet = NOERR;
       }
    else
       {                                // No extension found...
        *lpszExtension = EOS;
        wRet = NOERR;
       }

    return (wRet);
} // NameReturnExtensionEx()


#ifndef SYM_NLM

////////////////////////////////////////////////////////////////////////
//
//   NameIsRoot()
//
// Description:
//
//   Returns TRUE if lpName refers to a root directory, FALSE otherwise.
//
//   if name is of the form "x:", then return NameGetType() & NGT_IS_ROOT
//   if name is of the form "x:\", then return TRUE
//   if name is of the form "\", then return TRUE
//   if name is of the form "\\server\volume", then return TRUE
//   if name is of the form "\\server\volume\", then return TRUE
//   Otherwise return FALSE.
//
// See Also:
//
//
////////////////////////////////////////////////////////////////////////
// 12/19/94 BEM Function header updated.
////////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI NameIsRoot(LPCTSTR lpName)
{
    SYM_ASSERT ( lpName );
    SYM_VERIFY_STRING ( lpName );

                                        // Check if it's 'x:' or 'x:\' or '\\'
    if (lpName[0] != '\0' && lpName[1] != '\0')
        {
                                        // if (lpName[1] == ':' &&
                                        //     lpName[2] == '\0')
        if (lpName[1] == ':' && lpName[2] == '\0')
            {
                                        // It's "x:".  The current directory
                                        // on x: may or may not be the root,
                                        // so we'll investigate further.
            return (NameGetType(lpName) & NGT_IS_ROOT) ? TRUE : FALSE;
            }

                                        // if (lpName[1] == ':' &&
                                        //     lpName[2] == '\\' &&
                                        //     lpName[3] == '\0')
        if (lpName[1] == ':' && lpName[2] == '\\' && lpName[3] == '\0')
            return TRUE;                // It's "x:\".

                                        // if (lpName[0] == '\\' &&
                                        //     lpName[1] == '\\')
        if (lpName[0] == '\\' && lpName[1] == '\\')
            {
                                        // It's a UNC, so find out if it
                                        // has server and volume.
            LPCTSTR lpSlashPos = NULL;  // Last slash found
            LPCTSTR p = lpName+2;       // Point past "\\"
            int     nSlashCount = 0;    // We can tell if it had at least
                                        // '\\server\volume' by couning
                                        // at least 1 slash beyond the '\\'

            while (*p)
                {
                if (*p == '\\')
                    {
                    lpSlashPos = p;
                    nSlashCount++;
                    if (nSlashCount > 2)
                        break;
                    }
                p = AnsiNext(p);
                }
                                        // p now points to the terminating
                                        // NULL or to the 3rd '\'
                                        // past the '\\'.

                                        // If there wasn't a volume or if
                                        // there was a path beyond the root
                                        // then it's not the root.
            if (nSlashCount == 0 || nSlashCount > 2)
                return FALSE;
                                        // At this point, nSlashCount is
                                        // either 1 or 2.
                                        // p points to the null byte.
                                        // If the last character was a
                                        // backslash ...
                                        // This is DBCS safe because we
                                        // know that lpSlashPos was correctly
                                        // found and that it is a single byte
                                        // character.

            if (lpSlashPos == (p-1))
                if (nSlashCount == 1)
                                        // If only 1 slash
                                        // was found then the name was:
                                        // "\\server\", which is not the root.
                    return FALSE;
                else                    // else 2 slashes were found and
                                        // the name was: "\\server\volume\"
                return TRUE;
                                        // At this point, nSlashCount is
                                        // either 1 or 2 and we know that
                                        // the last character is not '\\'.
                                        // So, we know that the name must
                                        // be either "\\server\volume" or
                                        // "\\server\volume\name".  So,
                                        // if nSlashCount is 1, it's the root
                                        // otherwise it's not.

            if (nSlashCount == 1)
                return TRUE;
            else
                return FALSE;

            }
        }
                                        // else if (lpName[0] == '\\' &&
                                        //          lpName[1] == '\0')
    else if (lpName[0] == '\\' && lpName[1] == '\0')
        return TRUE;                    // It's "\".


    return FALSE;                       // It's not a root directory
}


////////////////////////////////////////////////////////////////////////
//
//   NameReturnRoot
//
// Description:
//
//   Returns the root of the file specified by 'lpszFile'.
//   The spec in lpszFile must be fully qualified.
//   This function returns the root portion of the specified pathname.
//   A UNC spec of \\server\volume is considered to be a "root".
//
//   NOTE:
//   The specified path must be fully qualified (call NameToFull()).
//   This function used to call NameConvertPath() on the input, but it is now
//   the responsibility of the caller.
//
// See Also:
//
//
////////////////////////////////////////////////////////////////////////
// 12/20/94 BEM Function header updated.
// 12/20/94 BEM Removed call to NameConvertPath() - the caller is
//              now responsible for calling NameConvertPath() before
//              calling this funcion.
////////////////////////////////////////////////////////////////////////

VOID SYM_EXPORT WINAPI NameReturnRoot(LPCTSTR szFile, LPTSTR lpszRoot)
{
    UINT    uSlashes;

    SYM_ASSERT ( szFile );
    SYM_VERIFY_STRING ( szFile );
    SYM_ASSERT ( lpszRoot );

                                        // Is this a mapped drive?
    if (szFile[0] && szFile[1] == ':' && szFile[2] == '\\')
        {                               // mapped drive roots are "x:\"
        SYM_VERIFY_BUFFER ( lpszRoot, 3 * sizeof(TCHAR) );
        STRNCPY(lpszRoot, szFile, 3);
        lpszRoot[3] = EOS;
        }
    else // Is UNC
        {
        LPCTSTR  lpszFile = szFile;
                                        // UNC roots are "\\a\b\"
        uSlashes = 4;
        while (*lpszFile && uSlashes)
            {
            if (*lpszFile == '\\')
                uSlashes--;
            else
                {
                if (DBCSIsLeadByte(*lpszFile))
                    *lpszRoot++ = *lpszFile++;
                }
            *lpszRoot++ = *lpszFile++;
            }
        *lpszRoot = EOS;
        }
}

////////////////////////////////////////////////////////////////////////
//
//   NameParseUNC()
//
// Description:
//
//   This function will parse a UNC spec of "\\SERVER\VOLUME\PATH" into
//   its constituent pieces, returning "SERVER", "VOLUME", and "\PATH".
//   The pieces are returned into the lpszServer, lpszVolume, and lpszPath
//   buffers passed in.  These buffers must each be at least
//   SYM_MAX_PATH in length.
//
//   The path portion of the name is returned with a leading backslash.
//
//
// Returns:
//    NOERR     Success
//    ERR       Failed because the spec was invalid/non-complete.
//
//
// See Also:
//   NameGetType(), NameToFull()
//
////////////////////////////////////////////////////////////////////////
// 12/21/94 BEM Function created.
////////////////////////////////////////////////////////////////////////
UINT SYM_EXPORT WINAPI NameParseUNC(
   LPCTSTR lpszUNC,                     // UNC spec of at least "\\server"
   LPTSTR  lpszServer,                  // Place to return SERVER
   LPTSTR  lpszVolume,                  // Place to return VOLUME
   LPTSTR  lpszPath)                    // Place to return the rest
{
    LPCTSTR p;
    LPCTSTR pVolume;
    UINT uLen;
    UINT uType = NameGetType(lpszUNC);

    SYM_ASSERT ( lpszUNC );
    SYM_VERIFY_STRING ( lpszUNC );
    SYM_ASSERT ( lpszServer );
    SYM_VERIFY_BUFFER ( lpszServer, SYM_MAX_PATH * sizeof(TCHAR) );
    SYM_ASSERT ( lpszVolume );
    SYM_VERIFY_BUFFER ( lpszVolume, SYM_MAX_PATH * sizeof(TCHAR) );
    SYM_ASSERT ( lpszPath );
    SYM_VERIFY_BUFFER ( lpszPath, SYM_MAX_PATH * sizeof(TCHAR) );

    *lpszServer = EOS;
    *lpszVolume = EOS;
    *lpszPath = EOS;

    if (!(uType & NGT_IS_UNC))
        return ERR;

    if (uType & NGT_IS_SERVER)
        {                               // Input is: "\\SERVER"
        STRCPY(lpszServer, lpszUNC+2);
        uLen = STRLEN(lpszServer);
        if (lpszServer[uLen-1] == '\\')
            lpszServer[uLen-1] = EOS;
        }

    if (!(uType & NGT_IS_FULL))
        return ERR;

                                        // After here we know we
                                        // have at least "\\server\volume"
    p = lpszUNC + 2;                    // Point passed '\\' (DBCS safe)
    while (*p && *p != '\\')            // Find first backslash
        p = AnsiNext(p);

    uLen = p - lpszUNC - 2;             // Calc length of server portion
    STRNCPY(lpszServer, lpszUNC+2, uLen);
    lpszServer[uLen] = EOS;
    p++;                                // DBCS safe because we're on SBC
                                        // Now p points passed "\\server\"
    pVolume = p;
    while(*p && *p != '\\')             // Find end of volume spec
        p = AnsiNext(p);

                                        // p points passed "\\server\volume"
    uLen = p - pVolume;
    STRNCPY(lpszVolume, pVolume, uLen);
    lpszVolume[uLen] = EOS;

    if (*p)
        {
        STRCPY(lpszPath, p);
        }

    SYM_ASSERT ( STRLEN ( lpszServer ) < SYM_MAX_PATH );
    SYM_ASSERT ( STRLEN ( lpszVolume ) < SYM_MAX_PATH );
    SYM_ASSERT ( STRLEN ( lpszPath ) < SYM_MAX_PATH );

    return(NOERR);
}

#endif                                  // #ifndef SYM_NLM


/*------------------------------------------------------------------------
    This procedure appends a file name to a path name.  We first append
    a '\' to the path name unless it already ends in '\' or ':' or if
    the first character of the filename isn't '\'.
------------------------------------------------------------------------*/

void SYM_EXPORT WINAPI NameAppendFile(LPTSTR lpPathName, LPCTSTR lpFileName)
{
    WCHAR       wChar;

    SYM_ASSERT ( lpPathName );
    SYM_VERIFY_STRING ( lpPathName );
    SYM_ASSERT ( lpFileName );
    SYM_VERIFY_STRING ( lpFileName );

    if (*lpPathName == '\0')
        {
        SYM_VERIFY_BUFFER ( lpPathName, (STRLEN(lpFileName)+1) * sizeof(TCHAR) );
        STRCPY(lpPathName, lpFileName);
        }
    else
        {
                                        // Append a slash if the last character
                                        // is neither '\' nor ':'.
        wChar = StringGetLastChar(lpPathName);
        lpPathName = StringGetEnd(lpPathName);
        if (*lpFileName != '\\' && wChar != '\\' && wChar != ':')
            {
            *lpPathName++ = '\\';
            }

        if ( wChar == '\\' && *lpFileName == '\\' )
            {
            lpFileName++;
            }

        SYM_VERIFY_BUFFER ( lpPathName, (STRLEN(lpFileName)+1) * sizeof(TCHAR) );
        STRCPY(lpPathName, lpFileName);
        }
}

/*------------------------------------------------------------------------
    This procedure appends a file extension to a path name.  We first append
    a '.' to the path name unless it already ends in '.' or the extension
    portion already ends in '.'.
------------------------------------------------------------------------*/
void SYM_EXPORT WINAPI NameAppendExtension(LPTSTR lpPathName, LPCTSTR lpszExtension)
{
    WCHAR       wChar;

    SYM_ASSERT ( lpPathName );
    SYM_VERIFY_STRING ( lpPathName );
    SYM_ASSERT ( lpszExtension );
    SYM_VERIFY_STRING ( lpszExtension );

                                        // See if we need to add a period
    if (lpszExtension[0] != '.')
        {
                                        // Append a '.' if the last character
                                        // is not a '.'.
        wChar = StringGetLastChar(lpPathName);
        if (wChar != '.')
            StringAppendChar(lpPathName, '.');
        }
    SYM_VERIFY_BUFFER ( &lpPathName[STRLEN(lpPathName)], (STRLEN(lpszExtension)+1)*sizeof(TCHAR) );
    STRCAT(lpPathName, lpszExtension);
}


#ifndef SYM_NLM

/*----------------------------------------------------------------------*
 * This procedure copies just the path part of NAME into PATH_NAME.     *
 *----------------------------------------------------------------------*/
void SYM_EXPORT WINAPI NameReturnPath (LPCTSTR name, LPTSTR path_name)
{
    SYM_ASSERT ( name );
    SYM_VERIFY_STRING ( name );
    SYM_ASSERT ( path_name );
    SYM_VERIFY_BUFFER ( path_name, (STRLEN(name)+1)*sizeof(TCHAR) );

    STRCPY(path_name, name);           // Get a copy of this name
    NameStripFile(path_name);          // Remove name from end
}

//-----------------------------------------------------------------------
// This function parses a passed-in command line and copies the start-up
// directory and the rest of the command line to the passed-in buffers.
//
// Example:
//
//    Input:
//            lpszString = "c:\bin\ude.exe wname.c -g330"
//    Output:
//       lpszStartDir = "c:\bin"
//       lpszCmdLine = "ude.exe wname.c -g330"
//
//-----------------------------------------------------------------------

VOID SYM_EXPORT WINAPI NameReturnStartDir (LPCTSTR lpszString, LPTSTR lpszStartDir, LPTSTR lpszCmdLine)
{
        TCHAR   szPath[SYM_MAX_PATH+1];
        LPTSTR  lp;

                                        // Isolate the first token of the passed-in
                                        // string.
        STRNCPY(szPath, lpszString, sizeof(szPath)/sizeof(*szPath)-1);
        szPath[sizeof(szPath)/sizeof(*szPath)-1] = EOS;
        lp = STRRCHR(szPath, ' ');
        if (lp)
            *lp = EOS;
                                        // The path part of this token is the
                                        // start-up directory.
        NameReturnPath(szPath, lpszStartDir);

                                        // Start fresh.
        STRNCPY(szPath, lpszString, sizeof(szPath)/sizeof(*szPath)-1);
        szPath[sizeof(szPath)/sizeof(*szPath)-1] = EOS;

                                        // Advance past start-up directory.
        lp = (LPTSTR) szPath + STRLEN(lpszStartDir);
        if (*lp == '\\' || *lp == '/')
            lp++;
                                        // What's left is the command line.
        STRCPY(lpszCmdLine, lp);
}

/*----------------------------------------------------------------------*/
/* NameToUnpacked                                                       */
/*    This routine is used to copy a name to an unpacked name.          */
/*    This function was ported from the Norton DOS library.             */
/*                                                                      */
/*    Converts a struture of 8 bytes name and 3 bytes                   */
/*    extension (space padded) to string.                               */
/*                                                                      */
/*    NOTE: Rewrote to be "DBCS-friendly".  11/9/94. BK.                */
/*                                                                      */
/* INPUTS                                                               */
/*    LPSTR    dst    the destination string                            */
/*    LPSTR     src    the source string                                */
/*                                                                      */
/* RETURNS                                                              */
/*    none                                                              */
/*----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI NameToUnpacked(LPTSTR lpszDest, LPCTSTR lpszSrc)
{
    register    int     i;
    LPCTSTR     lpszStart = lpszSrc;

    SYM_ASSERT ( lpszDest );
    SYM_ASSERT ( lpszSrc );
    SYM_VERIFY_STRING ( lpszSrc );

                                        // Copy first 8 characters (name)

    for (i = 0; *lpszSrc != ' ' && i < 8; )
        {
        if (DBCSIsLeadByte(*lpszSrc))
            {
            *lpszDest++ = *lpszSrc++;
            i++;
            }
        *lpszDest++ = *lpszSrc++;
        i++;
        }

    *lpszDest++ = '.';                  // add '.'

                                        // Copy extension
    lpszSrc = lpszStart + 8;
    for (i = 8; *lpszSrc != ' ' && i < 11; )
        {
        if (DBCSIsLeadByte(*lpszSrc))
            {
            *lpszDest++ = *lpszSrc++;
            i++;
            }
        *lpszDest++ = *lpszSrc++;
        i++;
        }
                                        // If no extension, remove '.'
    if (i == 8)
        lpszDest--;

    *lpszDest = EOS;                    // null terminate
}


/*----------------------------------------------------------------------*/
/* NameToPacked                                                        */
/*    This function removes the NULL at the end of a string the period  */
/*    before the extension and pads the filename with blanks.           */
/*                                                                      */
/*          Takes a null terminated string and converts it              */
/*          to 11 bytes (8 name and 3 extention space padded)           */
/*                                                                      */
/* INPUTS                                                               */
/*    LPSTR  dst        destination                                     */
/*    LPSTR  src        source                                          */
/*                                                                      */
/* RETURNS                                                              */
/*    standard                                                          */
/*----------------------------------------------------------------------*/

void SYM_EXPORT WINAPI NameToPacked (LPTSTR dst, LPCTSTR src)
{
    register     UINT    i;
    register     TCHAR   ch;
    LPTSTR       lpStart = (LPTSTR)src;

    SYM_ASSERT ( dst );
    SYM_ASSERT ( src );
    SYM_VERIFY_STRING ( src );

    for ( i=0; i<11; dst[i++] = ' ' );

    if (NameIsDot(src))
        {
        *dst = '.';
        return;
        }

    if (NameIsDoubleDot(src))
        {
        dst[0] = dst[1] = '.';
        return;
        }

    i = 0;
    ch = *src;

    while (i < 11)
        {
        if( DBCSGetByteType( lpStart, (LPSTR)src ) != _MBC_SINGLE )
            {
            src++;
            dst[i++] = ch;
            ch = *src;
            continue;
            }

        if( ch == EOS || ch == '\\')
            break;

        if (ch == '.')
            i = 8;
        else
            dst[i++] = ch;
        src++;
        ch = *src;
        }
                                        // Convert "\" to "\          "
    if ( ch == '\\' && i == 0 )
        *dst = '\\';
}


/*----------------------------------------------------------------------*
 * This procedure converts a a relative file name into a full path      *
 * name.                                                                *
 *----------------------------------------------------------------------*/

void SYM_EXPORT WINAPI NameConvertPath(LPTSTR lpPath,LPCTSTR lpSrc)
{
    auto        LPTSTR          lpDst = lpPath;
    auto        TCHAR           szWorkBuffer[SYM_MAX_PATH+1];
    auto        LPTSTR          lpNewSrc = szWorkBuffer;
    auto        UINT            uType, uSize;


                                        // We shouldn't be changing the
                                        // source!  We now work on a local
                                        // string copy, rather than the
                                        // original.  BK
    STRNCPY(lpNewSrc, lpSrc, sizeof(szWorkBuffer) - 1);

    NameConvertFwdSlashes(lpPath);
    NameConvertFwdSlashes(lpNewSrc);
    NameCompact(lpNewSrc);              // Remove "name\.." and ".\"
    NameCompact(lpDst);                 // Remove "name\.." and ".\"

    uType = NameGetType(lpNewSrc);
                                        // Is this already a full name?
    if (uType & NGT_IS_FULL)
        {
        if (!(uType & NGT_IS_UNC) && lpNewSrc[1] != ':')
            {
            MEMMOVE(lpNewSrc+2, lpNewSrc, (STRLEN(lpNewSrc) + 1)*sizeof(TCHAR));
            lpNewSrc[0] = *lpDst;
            lpNewSrc[1] = ':';
            }
        STRNCPY(lpDst,lpNewSrc, SYM_MAX_PATH - 1);         // Copy full path over
        }
    else
        {
        if (!(uType & NGT_IS_UNC) && lpNewSrc[1] != ':')
            {
            MEMMOVE(lpNewSrc+2,lpNewSrc,(STRLEN(lpNewSrc) + 1)*sizeof(TCHAR));
            lpNewSrc[0] = *lpDst;
            lpNewSrc[1] = ':';
            }

                                        // Is there a drive letter or '\\'?
        if ((uType & NGT_IS_UNC) || NameHasDriveSpec (lpNewSrc))
            lpNewSrc += 2;              // Yes, skip over it

                                        // Is there a drive letter or '\\'?
        if (NameIsUNC(lpDst) || NameHasDriveSpec (lpDst))
            lpDst += 2;                 // Yes, skip over it

        if (DBCSInstalled())
            {
            while (*lpNewSrc != EOS)
                {
                if (*lpNewSrc == '.')   // Could be either '.' or '..'
                    {
                                        // Is it '..'?

                    if (*(++lpNewSrc) == '.')
                        {               // Yes.
                                        // Remove one level of path
                        NameStripFile(lpPath);
                        }
                    while (*lpNewSrc != '\0' &&
                                (*lpNewSrc != '\\' || DBCSGetByteType(szWorkBuffer, lpNewSrc)))
                        lpNewSrc++;     // Skip over '\'
                    }
                else                    // Copy over next name
                    {
                                        // Move to '\0' at end of name
                    uSize = STRLEN(lpPath);
                    lpDst = lpPath + uSize;
                    if (lpDst > lpPath &&
                                    (*(lpDst - 1) != '\\' || DBCSGetByteType ( lpPath, lpDst-1)))
                        {
                        *lpDst++ = '\\';// Put '\' before next name
                        uSize++;
                        }

                    while (*lpNewSrc != '\0' &&
                                   (*lpNewSrc != '\\' || DBCSGetByteType( szWorkBuffer, lpNewSrc)))
                        {
                        if (uSize >= SYM_MAX_PATH)
                            {
                            *lpNewSrc = EOS;
                            break;
                            }
                        uSize++;
                                        // Copy over name
                        *lpDst++ = *lpNewSrc++;
                        }

                    if (*lpNewSrc == '\\' )
                        lpNewSrc++;     // Skip over '\'
                    *lpDst++ = '\0';    // Write end of string mark
                    }
                }
            }
        else
            {
            while (*lpNewSrc != EOS)
                {
                if (*lpNewSrc == '.')   // Could be either '.' or '..'
                    {
                                        // Is it '..'?
                    if (*(++lpNewSrc) == '.')
                        {               // Yes.
                                        // Remove one level of path
                        NameStripFile(lpPath);
                        }
                    while (*lpNewSrc != '\0' && *lpNewSrc++ != '\\')
                        continue;       // Skip over '\'
                    }
                else                    // Copy over next name
                    {
                                        // Move to '\0' at end of name
                    uSize = STRLEN(lpPath);
                    lpDst = lpPath + uSize;
                    if (lpDst > lpPath && *(lpDst - 1) != '\\')
                        {
                        *lpDst++ = '\\';// Put '\' before next name
                        uSize++;
                        }
                    while (*lpNewSrc != '\0' && *lpNewSrc != '\\')
                        {
                        if (uSize >= SYM_MAX_PATH)
                            {
                            *lpNewSrc = EOS;
                            break;
                            }
                        uSize++;
                                        // Copy over name
                        *lpDst++ = *lpNewSrc++;
                        }
                    if (*lpNewSrc == '\\')
                        lpNewSrc++;     // Skip over '\'
                    *lpDst++ = '\0';    // Write end of string mark
                    }
                }
            }
        }
}


/*----------------------------------------------------------------------*
 * This procedure removes sequences like "\name\.." from a path or file *
 * name to make sure that the name is as short as possible.             *
 *                                                                      *
 * Here are the possibilities:                                          *
 *                                                                      *
 *    name\..   -->        ""                                           *
 *    .\        -->        ""                                           *
 *                                                                      *
 * Here are some boundary conditions that are also useful examples:     *
 *                                                                      *
 *    x:.\          -->    x:                                           *
 *    x:.           -->    x:.                                          *
 *    x:..          -->    x:..                                         *
 *    x:\..         -->    x:\..    (this isn't a leagal path)          *
 *    one\name\..   -->    one                                          *
 *    ..\           -->    ..\                                          *
 *    x:\.          -->    x:\                                          *
 *    x:\one\..     -->    x:\      (old bug, use to return x: instead) *
 *----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI NameCompact(LPTSTR name)
{
    auto    LPTSTR   start;             // Start of name we're scanning
    auto    LPTSTR   s;                 // Floating pointer to string
    auto    LPTSTR   anchor;
    auto    BOOL     bStartOfPiece;     // TRUE on very first char or
                                        // on char after PATH_CHAR

    SYM_ASSERT ( name );
    SYM_VERIFY_BUFFER ( name, STRLEN(name)*sizeof(TCHAR) );


    if (*name == EOS)                   // Do we have a name?
        return;                         // No, then we're all done

    start = name;                       // Point the the name
                                        // Is there a drive letter or '\\'?
    if (start[1] == ':' || NameIsUNC(start))
        start += 2;                     // Yes, then skip over it

    s = start;                          // Move to start of the string
    bStartOfPiece = TRUE;
    while (*s != EOS)                   // Scan forward through string
        {

                                        // Look for '\.' entry at the
                                        // end.
        if (s[0] == '\\' && s[1] == '.' && s[2] == EOS)
            {
            s[1] = EOS;
            break;
            }

        if (!bStartOfPiece)
            {
            bStartOfPiece = (*s == PATH_CHAR);
            s++;                        //&? DBCS SAFE?
            continue;
            }

        if (s[0]=='.' && s[1]=='\\')    // Is this ".\" sequence
            {                           // Yes, remove from string
            STRCPY(s, s + 2);           // Remove the ".\" string
            continue;                   // Look for the next '.'
            }

                                        // This is "..\" sequence
                                        // or ".." at end of string
        if (s[0] == '.' && s[1] == '.' &&
            (s[2] == PATH_CHAR || s[2] == EOS))
            {
            if (s == start && s[2] == PATH_CHAR)
                {                       // This is "..\" at start
                start += 3;             // Skip over "..\"
                s = start;
                continue;               // Look for next '.'
                }

            anchor = s + 2;             // First BYTE after ".."
                                        // This is DBCS safe since we
                                        // know that the bytes we are
                                        // passing are single byte chars.
            if (s != start)
                {
                                        //&? DBCS SAFE?
                s--;                    // Move to '\' of "\.."
                while (s > start && (*(--s) != PATH_CHAR ||
                       DBCSGetByteType ( name, s ) ) )
                    continue;           // Skip back over this BYTE
                STRCPY(s, anchor);      // Remove "name\.."
                if (NameHasDriveSpec (name) && (name[2] == EOS))
                    {
                                        // Case of "x:\one\.." being turned
                                        // into "x:" instead of "x:\"
                    STRCAT(name, _T("\\"));
                    }
                }
            else
                {
                s = anchor;
                }

            continue;                   // Look for the next '.'
            }

        bStartOfPiece = (*s == PATH_CHAR);
        s++;                            // Move to the next character
                                        //&? DBCS Safe?
        }
}

////////////////////////////////////////////////////////////////////////
//
//   NameReplacePathChar()
//
// Description:
//   Changes forward slashes to backslashes (these can come from
//   intercepting int calls where the program generating them uses
//   fwd slashes (like Brief)).
//
//   Returns TRUE if a replacement happened, otherwise FALSE.
//
// See Also:
//   NameConvertFwdSlashes()
//
////////////////////////////////////////////////////////////////////////
// 12/16/94 BEM Header comment created.
////////////////////////////////////////////////////////////////////////
BOOL SYM_EXPORT WINAPI NameReplacePathChar(LPTSTR lp)
{
    auto    BOOL    bDidReplace;

    SYM_ASSERT ( lp );

    bDidReplace = FALSE;

    while(lp && *lp)
        {
        if(*lp == '/')
            {
            *lp = '\\';
            bDidReplace = TRUE;
            }
        lp = AnsiNext(lp);
        }

    return(bDidReplace);
}

#endif          // #ifndef SYM_NLM


/*-----------------------------------------------------------------------*/
/* TrueNameGet                                                           */
/*    This function uses DPMI issue int 21 function 60h.  It is an       */
/*    undocumented function.  So only use this functio when absolutely   */
/*    necessary.                                                         */
/*                                                                       */
/* INPUTS                                                                */
/*    LPSTR    lpRelativePath  the relative path to pass to the         */
/*                              true name function                       */
/*    LPSTR    lpTrueName      contains the result of the call to       */
/*                              to the true name function.               */
/*                                                                       */
/* RETURNS                                                               */
/*    -1 if error 0 if successful.                                       */
/*-----------------------------------------------------------------------*/

#if !defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NLM) && !defined(SYM_NTK)
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI TrueNameGet (LPCTSTR lpRelativePath, LPTSTR lpTrueName)
{
#if defined(SYM_OS2)
    STRCPY(lpRelativePath, lpTrueName);
    return -1;
#else   //  *** Not OS/2    ***
    auto        UINT    wRet = (UINT) ERR;
#ifdef  SYM_PROTMODE
    auto union  REGS    regs;
    auto struct SREGS   segregs;
    auto        DWORD   dwRelativeSegSel;
    auto        DWORD   dwTrueNameSegSel;
    auto        LPSTR  lpRelativeSel;
    auto        LPSTR  lpRelativeSeg;
    auto        LPSTR  lpTrueNameSel;
    auto        LPSTR  lpTrueNameSeg;

	if ( lpRelativePath[0] == '\\' && lpRelativePath[1] == '\\' )
		{
										// UNC path. No resolution of
										// the path is required.
		if ( lpRelativePath != lpTrueName )
			STRCPY ( lpTrueName, lpRelativePath );

		return NOERR;
		}

    dwRelativeSegSel = GlobalDosAlloc(SYM_MAX_PATH);
    dwTrueNameSegSel = GlobalDosAlloc(SYM_MAX_PATH);

    if (dwRelativeSegSel == 0L || dwTrueNameSegSel == 0L)
        return ERR;

                                            // Check if GlobalDosAlloc Fails
    lpRelativeSel = (LPSTR)MAKELP(LOWORD(dwRelativeSegSel), 0);
    lpRelativeSeg = (LPSTR)MAKELP(HIWORD(dwRelativeSegSel), 0);

    lpTrueNameSel = (LPSTR)MAKELP(LOWORD(dwTrueNameSegSel), 0);
    lpTrueNameSeg = (LPSTR)MAKELP(HIWORD(dwTrueNameSegSel), 0);

                                        // Convert the path to OEM while
                                        // copying to the low-dos memory
    NAnsiToOem (lpRelativePath, lpRelativeSel);

    regs.h.ah = 0x60;
    segregs.ds = FP_SEG(lpRelativeSeg);
    regs.x.si = FP_OFF(lpRelativeSeg);
    segregs.es = FP_SEG(lpTrueNameSeg);
    regs.x.di = FP_OFF(lpTrueNameSeg);


    IntWin((BYTE)0x21,&regs,&segregs);  // Real Mode Interrupt

                                            // Check for errors
    if (regs.x.cflag == FALSE)
        {
        wRet = NOERR;

                                        // Convert the path to Ansi while
                                        // copying from low-dos memory
        NOemToAnsi (lpTrueNameSel, lpTrueName);
        }

    GlobalDosFree(LOWORD(dwRelativeSegSel));
    GlobalDosFree(LOWORD(dwTrueNameSegSel));

#else
    auto        char    szTempBuffer[SYM_MAX_PATH];

    _asm                                // some things are simpler in DOS!
        {
        push    si
        push    di
        push    es
        push    ds
        push    bp

        lds     si, lpRelativePath
        mov     ax, ss
        mov     es, ax
        lea     di, szTempBuffer

        mov     ah, 60h

        DOS
        jc      Is_Error
        mov     wRet, NOERR
Is_Error:
        pop     bp
        pop     ds
        pop     es
        pop     di
        pop     si
        }

    if (wRet == NOERR)
        STRCPY(lpTrueName, szTempBuffer);
#endif

    return (wRet);
#endif  //  #ifdef SYM_OS2
}
#pragma optimize("", on)
#endif                                  // #if !defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NLM) && !defined(SYM_NTK)


#if !defined(SYM_NLM)

#if !defined(SYM_NTK)
/*----------------------------------------------------------------------*/
/* NameIsValidFileChar                                                  */
//  NOTE: THIS FUNCTION IS OBSOLETE - USE NameIsValidFileCharEx()
//        IF YOU USE THIS FUNCTION, YOU WILL ONLY CONSIDER ILLEGAL
//        SHORT NAME CHARACTERS, WHICH IS WRONG IF YOU'RE DEALING WITH
//        LFNs.
/*    Tests whether ch is a valid filename character                    */
/*    If bAnyChar == TRUE, we must accept '.' and SPACE as legal        */
/*        file name characters.                                         */
/*                                                                      */
/*      NOTE: Converted from ASM to C on 3/31/94 by BRAD.               */
/*----------------------------------------------------------------------*/
BOOL SYM_EXPORT WINAPI NameIsValidFileChar(TCHAR character, BOOL bAnyChar)
{
    return(NameIsValidFileCharEx(character, bAnyChar, TRUE));
}

////////////////////////////////////////////////////////////////////////
//
//   NameIsValidFileCharEx()
//
// Description:
//      This function will check the given character to see if it
//      is one of the illegal characters for the given context (LFN
//      or SFN).
//
//      If bAnyChar == TRUE, we must accept '.' and SPACE as legal
//      file name characters (this is a legacy parameter - pass FALSE
//      unless you remember what this parm was for).
//
//   NOTE: Converted from ASM to C on 3/31/94 by BRAD.
//   NOTE: This code can't be called "multithreaded", even with the
//         initialization being done only upon the first call.
//
// See Also:
//
//   NameReturnIllegalChars(), VolumeSupportsLFN()
////////////////////////////////////////////////////////////////////////
//  2/17/95 MARTIN/BEM Function created.
////////////////////////////////////////////////////////////////////////

BOOL SYM_EXPORT WINAPI NameIsValidFileCharEx (
    TCHAR   character,                  // character to validate.
    BOOL    bAnyChar,                   // TRUE to accept '.' and ' ' without question
                                        // (normally pass FALSE for bAnyChar)
    BOOL    bValidateForSFN             // TRUE to validate as SFN, FALSE for LFN
    )
{
#ifdef SYM_OS2
    return (character != '*' && character != '?');
#else
                                        // Cache of SFN and LFN illegals
    static      TCHAR   szSFNIllegals[ SYM_MAX_PATH ];
    static      TCHAR   szLFNIllegals[ SYM_MAX_PATH ];
    static      BOOL    bBufferFull = FALSE;
    static      UINT    uSFNCount;
    static      UINT    uLFNCount;
    auto        UINT    uCount;
    auto        UINT    i;
    auto        LPTSTR  lpChars = (bValidateForSFN ? szSFNIllegals : szLFNIllegals);
    auto        WORD    wIllegalFlags;


    if (DBCSInstalled())
        {
        if (character < ' ' && character != 0x05)
            return (FALSE);
        }

    if (!bAnyChar && character == '.')
        return (FALSE);

    if (bAnyChar && character == ' ' || character == '.')
        return (TRUE);

                                        // Check if we have cached info
                                        // to satisfy the caller's request.
    if (!bBufferFull)
        {
                                        // -----------------------------------
                                        // For all cases wildcard characters
                                        // and path characters are considered
                                        // invalid.
                                        // IMPORTANT: For backward compatibility
                                        // lower case letters are considered
                                        // valid for SFN character validation.
                                        // This is technically incorrect.
                                        // -----------------------------------

        wIllegalFlags = SYM_ILLEGAL_WILDCARDCHARS |
                        SYM_ILLEGAL_PATHCHARS |
                        SYM_ILLEGAL_DOS;

        uSFNCount = NameReturnIllegalChars (
                                        wIllegalFlags,
                                        NULL,
                                        szSFNIllegals,
                                        sizeof(szSFNIllegals)/sizeof(*szSFNIllegals)
                                        );

        SYM_ASSERT(uSFNCount);

        wIllegalFlags = SYM_ILLEGAL_WILDCARDCHARS |
                        SYM_ILLEGAL_PATHCHARS |
                        SYM_ILLEGAL_WIN95;

        uLFNCount = NameReturnIllegalChars (
                                        wIllegalFlags,
                                        NULL,
                                        szLFNIllegals,
                                        sizeof(szLFNIllegals)/sizeof(*szLFNIllegals)
                                        );

        SYM_ASSERT(uLFNCount);

        bBufferFull = TRUE;
        }

    uCount = (bValidateForSFN ? uSFNCount : uLFNCount);
    for (i = 0; i < uCount; i++)
        if ( *lpChars++ == character )
            return(FALSE);

    return(TRUE);
#endif                                  // #ifdef SYM_OS2 / #else
}
#endif                                  // #if !defined(SYM_NTK)

//***************************************************************************************
//
// InsertChar()
//
// If pBuffer is NULL, then only *pwCount is updated with how many chars WOULD have been
// inserted.
//***************************************************************************************
// 06/27/95 BEM - Header comment added, changed to tolerate NULL pBuffer
//***************************************************************************************
LPTSTR LOCAL PASCAL InsertChar (TCHAR ch, LPTSTR pBuffer, UINT FAR *pwCount, BOOL bPathChars)
{
    if (ch != '.' && ch != 0)
        {
        if ((ch == '\\' || ch == '/' || ch == ':') && !bPathChars)
            return (pBuffer);

        if (pBuffer)
            *pBuffer++ = ch;
        (*pwCount)++;
        }

    return (pBuffer);
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Fills a buffer with characters that are not allowed to be used  *
 *      in filenames.  For DOS versions 3.3 or greater, a DOS call is   *
 *      made that returns a country-dependent list of characters.  For  *
 *      any other DOS version, a default set of characters is returned  *
 *                                                                      *
 * Parameters:                                                          *
 *      pBuffer         The buffer to be filled with characters         *
 *                      or NULL to compute number of chars required     *
 *                                                                      *
 *      bPathChars      Include characters used in pathnames (\, /, :)  *
 *                                                                      *
 * Return Value:                                                        *
 *      The number of characters in the buffer, or zero if not          *
 *      supported or an error.                                          *
 *                                                                      *
 *      If pBuffer was NULL, then the return value is the number of     *
 *      characters that would be copied into the buffer (not including  *
 *      the final EOS.                                                  *
 *                                                                      *
 ************************************************************************
 * 02/18/1992 ED Function Created.                                      *
 ************************************************************************/

UINT SYM_EXPORT WINAPI NameGetIllegalChars (LPTSTR pBuffer, BOOL bPathChars)
{
    auto        BYTE        x;
    auto        UINT        wCount = 0;
    auto        BOOL        bGotTable = FALSE;

                                        // the illegal characters function
                                        // is only available in DOS 3.3+
#if !defined(SYM_OS2) && !defined(SYM_NLM) && !defined(SYM_VXD) && !defined(SYM_NTK)
    {
    auto        LPFILECHARTABLE         lpCharTable;
    auto        LPSTR                   lpChars;
#ifdef  SYM_PROTMODE
    auto        UINT                    wSelector;
#endif

    if (DOSGetVersion() >= DOS_3_30)
        {
                                // get the DOS character table pointer
                                // (which is always a real-mode address)
        lpCharTable = GetCharTable ();

        if (lpCharTable == NULL)
            goto Not_Supported;

#ifdef  SYM_PROTMODE            // create a protected mode pointer
        if (!SelectorAlloc(FP_SEG(lpCharTable), 0xFFFF, &wSelector))
            FP_SEG(lpCharTable) = wSelector;
        else
            goto Not_Supported;
#endif
                                // -------------------------------------
                                // copy the leading end of the range
                                // of non-permissable characters
                                // -------------------------------------
        if (lpCharTable->fctFirst > 0)
            {
            for (x = 1; x <= lpCharTable->fctFirst; x++)
                pBuffer = InsertChar (x, pBuffer, &wCount, bPathChars);
            }
                                // -------------------------------------
                                // copy the trailing end of the range
                                // of non-permissable characters
                                // -------------------------------------
        if (lpCharTable->fctLast < 255)
            {
            // BEM
            // The following used to be coded as:
            //            for (x = lpCharTable->fctLast; x <= 255; x++)
            //                pBuffer = InsertChar (x, pBuffer, &wCount, bPathChars);
            // But x is a BYTE, and this would be an endless loop.
            // This must mean that this code was never executed, and so lpCharTable->fctLast is
            // ALWAYS 255 since fctLast is also a BYTE?
            // BEM
            x = lpCharTable->fctLast;
            while (TRUE)
                {
                pBuffer = InsertChar (x, pBuffer, &wCount, bPathChars);
                if (x == 255)
                    break;
                x++;
                }
            }
                                // -------------------------------------
                                // copy the range of excluded characters
                                // -------------------------------------
        if (lpCharTable->fctExcludeFirst != lpCharTable->fctExcludeLast)
            {
            for (x = lpCharTable->fctExcludeFirst; x <= lpCharTable->fctExcludeLast; x++)
                pBuffer = InsertChar (x, pBuffer, &wCount, bPathChars);
            }
                                // -------------------------------------
                                // copy the explicit illegal characters
                                // -------------------------------------
        lpChars = (LPSTR) &lpCharTable->fctTable;
        for (x = 0; x < lpCharTable->fctIllegals; x++)
            pBuffer = InsertChar (lpChars[x], pBuffer, &wCount, bPathChars);

#ifdef  SYM_PROTMODE
        SelectorFree(FP_SEG(lpCharTable));
#endif
        bGotTable = TRUE;
        }
    }
Not_Supported:

#endif      //  #if !defined(SYM_OS2) && !defined(SYM_NLM) && !defined(SYM_VXD)

    if ( !bGotTable )
        {
#ifdef SYM_WIN32
        auto    BYTE    byEndInsertChar = 0x1F;
#else
        auto    BYTE    byEndInsertChar = 0x20;
#endif
                                        // -----------------------------------
                                        // Note that 0x20 is a valid character
                                        // under Chicago. Brad told that
                                        // we shouldn't wory about Win32s.
                                        // -----------------------------------
        for (x = 0; x <= byEndInsertChar; x++)
            pBuffer = InsertChar (x, pBuffer, &wCount, bPathChars);

        x = 0;                          // add default characters
        while (szDefChars[x])
            pBuffer = InsertChar (szDefChars[x++], pBuffer, &wCount, bPathChars);

        }

    if (pBuffer)
        *pBuffer = '\0';                // null-terminate the string
    return (wCount);
}

#endif      // #ifndef SYM_NLM


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Gets the filename character table, which is country-dependent.  *
 *      The INT 21 call is supported by Windows, but all pointers in    *
 *      the returned structure are untranslated.                        *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 02/18/1992 ED Created.                                               *
 ************************************************************************/

#if !defined(SYM_OS2) && !defined(SYM_NLM) && !defined(SYM_NTK)
#pragma optimize("", off)
LPFILECHARTABLE LOCAL PASCAL GetCharTable (void)
{
#define FCT_POINTER_SIZE        5

    auto        LPFILECHARTABLE lpCharTable = NULL;
    auto        UINT            wResult = 0;

#ifdef SYM_WIN32
#ifndef _M_ALPHA
    if (SystemGetWindowsType() != SYM_SYSTEM_WIN_NT)
        {
        typedef struct
            {
            BYTE byInfoID;
            LPFILECHARTABLE lpTable;
            } COUNTRY_INFO;

        auto  COUNTRY_INFO    countryInfo;
        auto  SYM_REGS        regs;

        MEMSET(&countryInfo, 0, sizeof(countryInfo));
        regs.EAX = 0x00006505;
        regs.EBX = 0x0000FFFF;          // default code page
        regs.ECX = sizeof(countryInfo);
        regs.EDX = 0x0000FFFF;          // default country ID
        regs.EDI = (DWORD) &countryInfo;
        VxDIntBasedCall( 0x21, &regs );

#define CARRY_FLAG     0x00000001

        wResult = regs.EFlags;
        if ( (regs.EFlags & CARRY_FLAG) == 0)
            {
            static  char    buffer[256 + sizeof(FILECHARTABLE)];
            auto    WORD    wSize;

            MEMSET(&buffer, 0, sizeof(buffer));
            MemCopyPhysical(buffer, countryInfo.lpTable, sizeof(WORD), FALSE);
            wSize = * ((WORD *) buffer);
            MemCopyPhysical(buffer, countryInfo.lpTable, wSize, FALSE);
            lpCharTable = (LPFILECHARTABLE) buffer;
            wResult = 0;
            }
        }
#endif // _M_ALPHA
#elif defined(SYM_PROTMODE)

    auto union  REGS    regs;
    auto struct SREGS   sRegs;
    auto        BYTE    FAR *lpTable;
    auto        DWORD   dwSegSel;

    dwSegSel = GlobalDosAlloc(FCT_POINTER_SIZE);

    lpTable = MAKELP(LOWORD(dwSegSel), 0);

    sRegs.es  = HIWORD(dwSegSel);
    regs.x.di = 0x0000; //FP_OFF(dwSegSel);
    regs.x.ax = 0x6505;
    regs.x.bx = 0xFFFF;
    regs.x.dx = 0xFFFF;
    regs.x.cx = FCT_POINTER_SIZE;
    IntWin(0x21, &regs, &sRegs);

    wResult = regs.x.cflag;

    lpCharTable =  * ((LPFILECHARTABLE FAR *) (lpTable + 1));

    GlobalDosFree(LOWORD(dwSegSel));

#else
    auto        BYTE            fctBuffer[FCT_POINTER_SIZE];

    _asm
        {
        mov     bx, -1                  ; use current code page
        mov     dx, -1                  ; use current country code

        lea     di,word ptr fctBuffer   ; return buffer
        mov     ax,ss
        mov     es,ax
        mov     cx,5                    ; size of return buffer

        mov     ax,6505h                ; get the filename table
        int     21h
        rcl     wResult,1               ; save the carry flag

        mov     ax, word ptr fctBuffer + 1
        mov     dx, word ptr fctBuffer + 3
        mov     word ptr lpCharTable, ax
        mov     word ptr lpCharTable + 2, dx
        }

#endif

    if (wResult)
        lpCharTable = NULL;

    return (lpCharTable);
}
#pragma optimize("", on)
#endif  //  #if !defined(SYM_OS2) && !defined(SYM_NLM) && !defined(SYM_NTK)



#if !defined(SYM_NLM) && !defined(SYM_NTK)

/************************************************************************/
/*                                              NameTitleFromCmdLine   */
/*                                                                      */
/* Description:                                                         */
/*      This function does its best at extracting a document name from  */
/*      a given command line string. This is used to create a Quick     */
/*      Access program item title from a command line.                  */
/*                                                                      */
/*      Basically, we're looking for the token nearest to the end of    */
/*      the string that looks like it might be a file name or path.     */
/*      The token is reduced to a file name (if a path was there, it's  */
/*      stripped off), and the extension (if there is one) is removed   */
/*      from the token. This is what's returned as the "document"       */
/*      string.                                                         */
/*                                                                      */
/* Return Values:                                                       */
/*      NOERR                           - Success.                      */
/*                                                                      */
/* See Also:                                                            */
/*                                                                      */
/************************************************************************/
/* 04/30/91     Keith Created.                                          */
/************************************************************************/

UINT SYM_EXPORT WINAPI NameTitleFromCmdLine(LPCSTR lpOrigStr, LPSTR lpDocStr)
{
    auto    LPSTR       lpPtr;          // Ptr to string
    auto    BYTE        szTemp [SYM_MAX_PATH+1];      // Working string
    auto    BYTE        szFile [SYM_MAX_PATH+1];      // File name

                                        // Make copy of original to work
                                        // with
    STRCPY (szTemp, lpOrigStr);
                                        // Go through each of the tokens
                                        // on the command line passed in.
    while (TRUE)
        {
                                        // Find last occurrance of space.
        lpPtr = STRRCHR (szTemp, ' ');
        if (lpPtr)
            {
                                        // Is this a file name?
                                        //
            if (NameIsValidFileChar (*(lpPtr + 1), FALSE))
                {
                                        // Yes, strip the file name from
                                        // the path (if there is one).
                                        //
                NameReturnFile (lpPtr + 1, szTemp);

                break;                  // We're done.
                }
            else
                {
                                        // This might be a switch or
                                        // something. Nullify the rest
                                        // of the string and go on to
                                        // next token.
                *lpPtr = '\0';
                }
            }
        else
            {
                                        // No more spaces found.
                                        // We're done.
            break;
            }
        }

    NameReturnFile (szTemp, szFile);

    if (*szFile)
        {
                                        // Strip the extension off of
                                        // the file name.
                                        //
        lpPtr = STRRCHR (szFile, '.');
        if (lpPtr)
            *lpPtr = '\0';
        }
    else
        {
                                        // Couldn't find a file. Punt
                                        // by setting it to a single
                                        // space character.
        szFile [0] = ' ';
        szFile [1] = '\0';
        }
                                        // Copy the result into the
                                        // destination.
    STRCPY (lpDocStr, szFile);

                                        // Capitalize first letter
    lpDocStr[0] = (BYTE) CharToUpper (lpDocStr[0]);

    return (NOERR);
}



int SYM_EXPORT WINAPI NamePathToEnvironment(LPCSTR lpInput, LPSTR lpOutput)
{
    (void) lpInput;
    (void) lpOutput;

    return ERR;
}

#endif  // if !defined(SYM_NLM) && !defined(SYM_NTK)


#ifdef SYM_WIN32                        // This function uses the Win32
                                        // API GetTextExtentExPoint()

////////////////////////////////////////////////////////////////////////
//
//   NameShortenFileNameRect()
//
// Description:
//
//   This function will shorten a path spec such that it fits into
//   the width of the given rectangle.  The passed in hDC is used so
//   that the currently selected font and text attributes are used to
//   calculate what will fit.
//
//   This function just uses the Win95 DrawTextEx() fucntion with all
//   the right magic flags to get the work done.  Using DT_CALCRECT
//   prevents DrawTextEx() from actually drawing the text.  Using
//   DT_MODIFYSTRING | DT_PATH_ELLIPSIS causes DrawTextEx() to return
//   the modified string.
//
// See Also:
//   NameShortenFileName(), WIN32 APIs: DrawTextEx(), GetTextExtentExPoint()
//
////////////////////////////////////////////////////////////////////////
// 12/28/94 BEM Function created.
////////////////////////////////////////////////////////////////////////

UINT SYM_EXPORT WINAPI NameShortenFileNameRect(
   LPCSTR lpPathName,                   // Original path spec
   LPSTR lpShortName,                   // Place to return shortened version
   HDC hDc,                             // DC to paint text into
   LPCRECT lpRect)                      // Rectangle to fit text into
{

    STRCPY(lpShortName, lpPathName);
    if (!DrawTextEx(hDc, lpShortName, -1, (LPRECT)lpRect,
               DT_PATH_ELLIPSIS | DT_MODIFYSTRING |
               DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT, NULL))
        return ERR;
    return NOERR;
}

#endif                                  // ifdef SYM_WIN32

/*----------------------------------------------------------------------*/
/* NameShortenFileName                                                 */
/* This procedure writes a file name to stderr such that it won't be    */
/* longer than WIDTH characters.  If the name is too long, it will      */
/* print '\...' followed by the last WIDTH - 4 characters.              */
/*                                                                      */
/*    \writing\util\shell    This name fits                             */
/*    a:\...ting\util\shell    Too long to fit, but with drive          */
/*----------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI NameShortenFileName(LPCTSTR lpPathName, LPTSTR lpShortName, int nWidth)
{
    auto    int         nLen;
    auto    int         nLenTemp;

    LPTSTR  lpszServVol;
    int     nPrefixSize;            // length of prefix
    int     nFileNameSize = 0;      // length of simple file name

    SYM_ASSERT ( lpPathName );
    SYM_VERIFY_STRING ( lpPathName );
    SYM_ASSERT ( lpShortName );
    SYM_VERIFY_BUFFER ( lpShortName, nWidth * sizeof(TCHAR) );

#define ELLIPSIS_LEN    3


    *lpShortName = 0;                   // Initialize to zero length

    nLen = STRLEN(lpPathName);

    if (nLen > nWidth)                  // Is name too long to fit?
        {                               // Too long to fit in field

        // look for the : marking the end of the drive or volume prefix

        lpszServVol = STRCHR( lpPathName, ':' );

        if (lpszServVol != NULL)
            {
            nPrefixSize = lpszServVol - lpPathName + 1 + 1; // prefix + ':\'

            // find the last \ to see how long the file name is - I will
            // give priority to displaying the file name

            nLenTemp = nLen - 1;
            while ( ( lpPathName[nLenTemp] != '\\' || DBCSGetByteType ( (LPSTR)lpPathName, (LPSTR)&lpPathName[nLenTemp] ) )
                    && nLenTemp > 0)
                {
                nLenTemp--;
                }

            nFileNameSize = nLen - nLenTemp - 1;

            // shorten prefix to leave room for ellipsis and file name

            if (nFileNameSize + nPrefixSize + ELLIPSIS_LEN > nWidth)
                {
                nPrefixSize = nWidth - nFileNameSize - ELLIPSIS_LEN;
                }

            // copy whatever prefix will fit

            if (nPrefixSize > 0)
                {
                STRNCPY(lpShortName,lpPathName, nPrefixSize);
                lpShortName[nPrefixSize] = EOS;

                nWidth -= nPrefixSize;  // Remove these chars from width
                }
            }
        else
            {

            // no drive or volume - indicate root at beginning as the old
            // routine used to do. This was taken out of the ellipsis string
            // because if I break in the middle of the volume name inserting
            // a root slash will be incorrect.

            lpShortName[0] = '\\';
            lpShortName[1] = 0;
            nWidth -=1 ;

            }

        STRCAT(lpShortName, _T("...")); // insert ellipsis

        nWidth -= ELLIPSIS_LEN;

                                        // Avoid leaving the second
                                        // of a DBCS character
        if ( DBCSGetByteType ( (LPSTR)lpPathName, (LPSTR)&lpPathName[nLen - nWidth] ) == _MBC_TRAIL)
            nWidth--;

         if (nWidth > 0)
            {
            STRCAT(lpShortName, &lpPathName[nLen - nWidth] );
            }
        }
    else
        {
        STRCPY(lpShortName,lpPathName); // it all fits
        }

    return (NOERR);
}

/*----------------------------------------------------------------------*
 * This procedure removes the extension from a file name if it's        *
 * present.                                                             *
 *----------------------------------------------------------------------*/

LPTSTR SYM_EXPORT WINAPI NameStripExtension (LPTSTR lpName)
{
    LPTSTR      lpPtr = lpName;

    SYM_ASSERT ( lpName );
    SYM_VERIFY_STRING ( lpName );

    lpPtr += STRLEN (lpPtr);            // Point to '\0' in string

    while (lpPtr-- > lpName)            // Backup to '\', ':', or '.'
        {

#ifndef SYM_NLM

        if ( DBCSGetByteType ( lpName, lpPtr ) != _MBC_SINGLE )
            continue;                  // Ignore DBCS characters
#endif
        if (*lpPtr == ':' || *lpPtr == '\\' || *lpPtr == '.')
            break;                      // We found terminal char
        }
    if (*lpPtr == '.')                  // Was there an extension?
        *lpPtr = '\0';                  // Terminate lpName here

    return (lpName);
}

#ifndef SYM_NLM

/*-----------------------------------------------------------------------*/
/*This procedure truncates the various parts of a file name so each      */
/* part of the path is not too long.                                     */
/*                                                                       */
/*      file name       No more than 8 characters long                   */
/*      extension       No more than 3 characters long.                  */
/*                                                                       */
/*      NOTE: Converted from ASM to C on 3/31/94 by BRAD.                */
/*-----------------------------------------------------------------------*/
#ifndef SYM_OS2
UINT SYM_EXPORT WINAPI NameTruncate (LPTSTR lpSrc)
{
    auto        LPTSTR  lpDest = lpSrc;
    auto        UINT    uMaxCount, uMaxLength, uCount;
    auto        DWORD   dwComponentLength, dwSystemFlags;
    auto        BOOL    bOK;
    auto        BOOL    bSupportsLFN;

    SYM_ASSERT ( lpSrc );
    SYM_VERIFY_STRING ( lpSrc );

#if defined(SYM_VXD)
    bOK = FALSE;
#else
    bOK = VolumeInformationGet(lpSrc, NULL, 0, NULL, &dwComponentLength,
                               &dwSystemFlags, NULL, 0);
#endif
    uMaxLength = (bOK) ? (WORD) dwComponentLength : 12;
    bSupportsLFN = (BOOL)(bOK && (dwSystemFlags & FS_LFN_APIS));

    uMaxCount = uMaxLength;
    uCount = 8;
    while (*lpSrc)
        {
        if (*lpSrc == ':' || *lpSrc == '\\')
            {
                                        // Skip double slashes
            if ( !(*lpSrc == '\\' && *(lpSrc + 1) == '\\') )
                *lpDest++ = *lpSrc;     // Save this character (':' or '\')
            lpSrc++;
            uMaxCount = uMaxLength;     // Reset count
            uCount = 8;
            continue;                   // Process next character
            }

        if (*lpSrc == '.' && uMaxCount)
            {
            *lpDest++ = *lpSrc++;       // Save this character ('.')
            uMaxCount--;
            uCount = 3;                 // Reset count
            continue;                   // Process next character
            }

        if ((bSupportsLFN || uCount)
            && uMaxCount)               // Have we gone past '.'?
            {
            uCount--;                   // Another character within component
            uMaxCount--;
            if (DBCSIsLeadByte(*lpSrc))
                {
                if ((bSupportsLFN || uCount) && uMaxCount)
                    {
                    uCount--;
                    uMaxCount--;
                    *lpDest++ = *lpSrc++;       // Save this character
                    *lpDest++ = *lpSrc++;       // Save this character
                    }
                else
                    {
                                                // Skip this character
                                                // Not enough space
                    lpSrc += sizeof(WCHAR);
                    }
                }
            else
                {
                *lpDest++ = *lpSrc++;       // Save this character
                }
            }
        else
            {
            *lpDest++ = *lpSrc;         // Save this character
                                        // Skip this character
            lpSrc = AnsiNext(lpSrc);
            }
        }
    *lpDest = EOS;

    return (FALSE);
}
#endif // #ifndef SYM_OS2


////////////////////////////////////////////////////////////////////////
//
//   NameGetType()
//
// Description:
//      Returns type information for the specified path.
//
//
//
//
// Parameters:
//
// Return Value:
//      The result is one or more of the following flags:
//
//      NGT_IS_DIR              The path is a directory name
//      NGT_IS_FILE             The path is a filename
//      NGT_IS_ROOT             The path is a root directory (which can
//                              be a UNC spec like: "\\SERVER\VOLUME" or
//                              "\\SERVER\VOLUME\")
//      NGT_IS_FULL             The path has a drive, path, and file
//                              Or it's a UNC spec that contains at least
//                              "\\SERVER\VOLUME"
//      NGT_IS_UNC              The path starts with '\\'
//      NGT_IS_SERVER           The path is "\\SERVER" or "\\SERVER\"
//                              (NGT_IS_SERVER is NOT NGT_IS_FULL).
//
// See Also:
//
//
////////////////////////////////////////////////////////////////////////
// 07/14/1992 ED Created.
// 12/20/94 BEM Function header updated and code updated for UNC.
////////////////////////////////////////////////////////////////////////

#if defined(SYM_WIN32) || defined(SYM_NTK)
UINT SYM_EXPORT WINAPI NameGetType (LPCTSTR lpPath)
{
    DWORD               dwAttr;
    UINT                wFlags = 0;

    SYM_ASSERT ( lpPath );

    if (*lpPath == 0)                   // This is a null directory.
        return NGT_IS_DIR;              // Don't bother with other tests.

                                        // "." and ".." are directories.
    else if (NameIsDot(lpPath) || NameIsDoubleDot(lpPath))
        {
        wFlags = NGT_IS_DIR;
        goto LOCRET;
        }
                                        // Is it "\", another valid path?
    else if ( lpPath[0] == '\\' && lpPath[1] == '\0' )
        {
        wFlags = (NGT_IS_ROOT | NGT_IS_DIR);
        goto LOCRET;
        }
                                        // Check if it's 'x:' or 'x:\' or '\\'
    else if (lpPath[1] != '\0')
        {
                                        // Is it "x:", a valid path?
        if ( lpPath[1] == ':' && lpPath[2] == '\0')
            {
#if defined(SYM_NTK)
            SYM_ASSERT ( FALSE );       // no concept of current directory
            return 0;
#else
            TCHAR szCurrentDir[SYM_MAX_PATH];

                                        // Check here for the current
                                        // directory before saying that
                                        // this is the root.
                                        //&? THIS WILL CAUSE A DISK HIT, WHICH
                                        //&? WILL CAUSE A HARD ERROR ON FLOPPY
                                        //&? SPECS WHEN NO DISK IS THERE.
            if (DirGet(*lpPath, szCurrentDir) == ERR)
                return 0;               // ERROR

            wFlags = NGT_IS_DIR;
                                        // Check if the current dir is
                                        // the root (i.e. "x:\")
            if ( szCurrentDir[2] == '\\' && szCurrentDir[3] == '\0' )
                wFlags |= NGT_IS_ROOT;

            goto LOCRET;
#endif
            }
                                        // Is it "x:\", another valid path?
        else if (lpPath[1] == ':' && lpPath[2] == '\\' && lpPath[3] == '\0')
            {
            wFlags = (NGT_IS_ROOT | NGT_IS_DIR);
            goto LOCRET;
            }
        else if (NameIsUNC(lpPath))
            {
            LPCTSTR p = lpPath + 2;     // Skip initial '\\'
                                        // DBCS safe because we know the
                                        // first two bytes were single byte
                                        // chars.
            int    nSlashCount = 0;     // Count of '\'s beyond initial '\\'

            wFlags = NGT_IS_UNC;
            while (*p)
                {
                if (*p == '\\')
                    {
                    nSlashCount++;
                    p++;                // DBCS safe because we know we
                                        // are on a single byte char.

                                        // NOTE: p now points one char
                                        // passed the backslash.

                                        // If spec is: "\\server\", then
                                        // return that info.
                    if (nSlashCount == 1 && *p == EOS)
                        {
                                        // If it's "\\\", then return nothing.
                        if (p == lpPath + 3)
                            return 0;
                        wFlags |= NGT_IS_SERVER;
                        goto LOCRET;
                        }
                    else if (nSlashCount == 2)
                        {
                                        // p-1 is DBCS Safe since we know
                                        // we're on single byte char and have
                                        // just advanced past it.
                        if (*AnsiPrev(lpPath, p-1) == '\\')
                            return 0;   // If "\\server\\", return nothing

                        if (*p == EOS)  // If spec is: "\\server\volume\"
                            {           // then return that info.
                            wFlags |= NGT_IS_ROOT | NGT_IS_DIR | NGT_IS_FULL;
                            goto LOCRET;
                            }
                        else            // If spec is: "\\server\volume\name"
                            {           // then break to see if it's a file
                                        // or a path.
                            wFlags |= NGT_IS_FULL;
                            break;
                            }
                        }
                    }
                else
                    p = AnsiNext(p);
                }

            if (!nSlashCount)
                {
                if (p != (lpPath + 2))  // Must be a "\\SERVER" spec
                    wFlags = NGT_IS_UNC | NGT_IS_SERVER;
                                        // else it was "\\" which isn't
                                        // anything except UNC, I guess.
                goto LOCRET;
                }

            if (nSlashCount == 1)       // If spec is: \\server\volume
               {                        // then return that info
               wFlags |= NGT_IS_ROOT | NGT_IS_DIR | NGT_IS_FULL;
               goto LOCRET;
               }
            }
        }

#if defined(SYM_NTK)
    if ( FileGetAttr(lpPath,&dwAttr) == ERR )
        {
        dwAttr = 0xffffffff;
        }
#else
    dwAttr = GetFileAttributes(lpPath);
#endif

                                        // If the file exists, check the attributes.
    if (dwAttr != 0xffffffff)
        {
        if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
            wFlags |= NGT_IS_DIR;
        else
            wFlags |= NGT_IS_FILE;
        }

LOCRET:
    if (!(wFlags & NGT_IS_UNC))         // If it was UNC, we already handled
                                        // determination of NGT_IS_FULL.
                                        // Check for a full name.
        if (*lpPath == '\\' || (lpPath[1] == ':' && lpPath[2] == '\\'))
            wFlags |= NGT_IS_FULL;

    return wFlags;
}

#else                                   // !SYM_WIN32 && !SYM_NTK
UINT SYM_EXPORT WINAPI NameGetType (LPCTSTR lpPath)
{
    UINT                wError;
    UINT                uAttr;
    UINT                wFlags = 0;
#ifdef SYM_OS2
    struct ffblk               filefindblk;
#endif


    if (*lpPath == 0)                   // This is a null directory
        return NGT_IS_DIR;
                                        // "." and ".." are directories
    else if (NameIsDot (lpPath) || NameIsDoubleDot (lpPath))
        {
        wFlags = NGT_IS_DIR;
        goto LOCRET;
        }
                                        // Is it '\', another valid path?
    else if (*(UINT FAR *)(lpPath) == 0x005C)
        {
        wFlags = (NGT_IS_ROOT | NGT_IS_DIR);
        goto LOCRET;
        }
                                        // Check if it's 'x:' or 'x:\'
    else if (lpPath[1] != '\0')
        {
                                        // Is it 'x:', a valid path?
#ifdef SYM_OS2
        if (*(UINT FAR *)(lpPath + 1) == 0x003A)
            {
            wFlags = (NGT_IS_ROOT | NGT_IS_DIR);
            goto LOCRET;
            }
#else
        if (*(UINT FAR *)(lpPath + 1) == 0x003A)
            {
            TCHAR szCurrentDir[SYM_MAX_PATH];

            if ( DirGet(*lpPath, szCurrentDir) == ERR )
                return 0;

            wFlags = NGT_IS_DIR;

            if ( *(UINT FAR *)(szCurrentDir+2) == 0x005C)
                wFlags |= NGT_IS_ROOT;

            goto LOCRET;
            }
#endif
        else if ((*(UINT FAR *)(lpPath + 1) == 0x5C3A) && lpPath[3] == '\0')
            {
            wFlags = (NGT_IS_ROOT | NGT_IS_DIR);
            goto LOCRET;
            }

        else if (NameIsUNC(lpPath))
            {
            LPCTSTR p = lpPath + 2;     // Skip initial '\\'
                                        // DBCS safe because we know the
                                        // first two bytes were single byte
                                        // chars.
            int    nSlashCount = 0;     // Count of '\'s beyond initial '\\'

            wFlags = NGT_IS_UNC;
            while (*p)
                {
                if (*p == '\\')
                    {
                    nSlashCount++;
                    p++;                // DBCS safe because we know we
                                        // are on a single byte char.

                                        // NOTE: p now points one char
                                        // passed the backslash.

                                        // If spec is: "\\server\", then
                                        // return that info.
                    if (nSlashCount == 1 && *p == EOS)
                        {
                                        // If it's "\\\", then return nothing.
                        if (p == lpPath + 3)
                            return 0;
                        wFlags |= NGT_IS_SERVER;
                        goto LOCRET;
                        }
                    else if (nSlashCount == 2)
                        {
                                        // p-1 is DBCS Safe since we know
                                        // we're on single byte char and have
                                        // just advanced past it.
                        if (*AnsiPrev(lpPath, p-1) == '\\')
                            return 0;   // If "\\server\\", return nothing

                        if (*p == EOS)  // If spec is: "\\server\volume\"
                            {           // then return that info.
                            wFlags |= NGT_IS_ROOT | NGT_IS_DIR | NGT_IS_FULL;
                            goto LOCRET;
                            }
                        else            // If spec is: "\\server\volume\name"
                            {           // then break to see if it's a file
                                        // or a path.
                            wFlags |= NGT_IS_FULL;
                            break;
                            }
                        }
                    }
                else
                    p = AnsiNext(p);
                }

            if (!nSlashCount)
                {
                if (p != (lpPath + 2))  // Must be a "\\SERVER" spec
                    wFlags = NGT_IS_UNC | NGT_IS_SERVER;
                                        // else it was "\\" which isn't
                                        // anything except UNC, I guess.
                goto LOCRET;
                }

            if (nSlashCount == 1)       // If spec is: \\server\volume
               {                        // then return that info
               wFlags |= NGT_IS_ROOT | NGT_IS_DIR | NGT_IS_FULL;
               goto LOCRET;
               }
            }
        }
                                        // get the file attribute
    if ((wError = FileGetAttr (lpPath, &uAttr)) == ERR)
        {
#ifdef SYM_OS2
        wError = findfirst(lpPath, &filefindblk, FA_DIREC);
        uAttr = (UINT) filefindblk.ff_attrib;
#elif defined(SYM_VXD)
        // Not done yet!
        wError = ERR;
#else
        {
        HFIND       hFind;
        FINDDATA    findData;

        hFind = FileFindFirst(lpPath, &findData);
        wError = ( hFind == INVALID_HANDLE_VALUE ) ? ERR : NOERR;
        uAttr = (UINT) findData.dwFileAttributes;
        FileFindClose(hFind);
        }
#endif  //SYM_OS2
        }
                                        // if the file exists, check the attr
    if (wError != ERR && uAttr != ERR)
        {
        if (uAttr & FA_DIR)
            wFlags |= NGT_IS_DIR;
        else
            wFlags |= NGT_IS_FILE;
        }

LOCRET:
    if ( !(wFlags & NGT_IS_UNC) )
                                        // check for a full name
        if (*lpPath == '\\' || (lpPath[1] == ':' && lpPath[2] == '\\'))
            {
            wFlags |= NGT_IS_FULL;
            }

    return (wFlags);

}
#endif                                  // !SYM_WIN32


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Separates the directory and the filename from a pathname.       *
 *      The path is directly modified to get the directory, but the     *
 *      filename goes into a separate buffer                            *
 *                                                                      *
 *      C:\PATH\FILE.EXT     ->  C:\PATH,  FILE.EXT                     *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/16/1992 ED Created.                                               *
 ************************************************************************/

void SYM_EXPORT WINAPI NameParsePath (LPTSTR lpPath, LPTSTR lpFile)
{
    NameReturnFile (lpPath, lpFile);
    NameStripFile (lpPath);
}

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Separates the directory and the filename from a pathname.       *
 *      This differs from NameParsePath in that the original            *
 *      path is not directly modified.                                  *
 *                                                                      *
 *      C:\PATH\FILE.EXT     ->  C:\PATH,  FILE.EXT                     *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/16/1992 ED Created.                                               *
 ************************************************************************/

void SYM_EXPORT WINAPI NameSeparatePath(
    LPCTSTR lpPath,
    LPTSTR lpDir,
    LPTSTR lpFile)
{
    NameReturnPath(lpPath, lpDir);
    NameReturnFile(lpPath, lpFile);
}

////////////////////////////////////////////////////////////////////////
//
//   NameValidatePath()
//
// Description:
//
//   This function preserves the previous behavior of NameValidatePath()
//   by calling NameValidatePathEx() with the bAllowWild as FALSE.
//
// See Also:
//
//
////////////////////////////////////////////////////////////////////////
// 12/20/94 BEM Function created.
////////////////////////////////////////////////////////////////////////

BOOL SYM_EXPORT WINAPI NameValidatePath(LPTSTR lpPathName)
{
    return(NameValidatePathEx(lpPathName, FALSE));
}


//************************************************************************
// NameValidatePathEx()
//
// This function validates that a pathname has been constructed properly.
// IT DOES NOT VALIDATE THE ACTUAL EXISTENCE OF THE PATHNAME!!!!!!!!!!!!!
//
// If the bAllowWild parameter is FALSE, wildcard characters are
// considered illegal (otherwise they are allowed).
//
// The pathname must be fully qualified.
//
// A properly constructed pathname has the form  d:\path\file.ext
//
// This function checks for the following things:
//       Validates that a UNC name has at least \\SERVER\VOLUME
//
//       1) Illegal characters:  " [ ] | < > + = ; , ? * and <= space
//
//       2) Drive letter:  a) if it exists, there must be a colon after it
//                         b) if it exists, it must be in the range A-Z
//                         c) there can only be one colon in the pathname,
//                            since a colon is an illegal filename character
//
//       3) Slashes: Double backslashes are not allowed except for UNCs.
//          Forward slashes are converted to backslashes in the pathname.
//
//       4) All separate path components fit into the DOS 8.3 format
//          (i.e., c:\8.3\8.3)
//
//       5) The path length is greater than zero and less than the
//          value of SYM_MAX_PATH.
//          &? Could check for the max legal component length,
//          &? but thay may just be overkill - BEM.
//
// The following checks should only be done in non W32 (LFN) environments:
// 1.- Truncating a file and comparing it with it's original filename is
//     an incorrect validation method for long filenames.
//
// 2,- A long filename can have several periods between '\' so checking
//     for periods is also erroneous for long filenames.
//
// If all these tests pass, TRUE is returned.  Otherwise FALSE is returned.
// Note that this function does not check the following things:
//
//       1) The drive letter maps to an existing drive
//       2) The path maps to an existing path
//       3) The path contains a filename rather than just a path
//
//************************************************************************
// Some Helpful Examples:
//
//
//       \\SERVER\VOLUME\NAME                    PASS
//       \\SERVER\VOLUME                         PASS
//       \\SERVER                                FAIL
//       \\SERVER\                               FAIL
//       \\\ANYTHING                             FAIL
//       C:\WINDOWS\AUTOEXEC.BAT                 PASS
//       C:\WINDOWS                              PASS
//       C:/WINDOWS                              PASS (converted in place)
//       C:\WIN.COM\WIN.INI1                     FAIL (4 character extension)
//       C:\AUTOEXEC1.BAT\AUTOEXEC.BAT           FAIL (9 character filename)
//       C:\AUTOEXEC12.BAT\AUTOEXEC.BAT          FAIL (10 character filename)
//       C:..\AUTOEXEC.BAT                       PASS
//       C:..\../AUTOEXEC.BAT                    PASS (converted in place)
//       C:AUTOEXECA                             FAIL (9 character filename)
//       C:A                                     PASS
//       #:\WINDOWS                              FAIL (invalid drive letter)
//       ..\TEMP\ED                              PASS
//       C:\TEMP\*.*                             FAIL (wildcards)
//       C:\WIN\SYSTEM.TMP.A                     FAIL (illegal extension)
//       C:\WINDOWS\                             PASS (trailing backslash)
//
//      NOTE: Converted from ASM to C on 3/31/94 by BRAD.
//      NOTE: Converted to handle UNCs on 12/20/94 by BEM.
//************************************************************************
BOOL SYM_EXPORT WINAPI NameValidatePathEx(LPTSTR lpPathName, BOOL bAllowWild)
#ifdef  SYM_OS2
{
    if ( (!(strchr(lpPathName, '*'))) && (!(strchr(lpPathName, '?'))) )
        return (TRUE);                  // Some of these names are OS/2
    return (FALSE) ;                    //  legal.  For compat. only.
}
#else
{
    LPTSTR      lpTemp;
    LPTSTR      lpSlashPos = NULL;
    UINT        uLen;
    TCHAR       szIllegals[SYM_MAX_PATH];
    WORD        wIllegalFlags = SYM_ILLEGAL_USEVOLUME;
    UINT        uNumChars, i;
    UINT        uSlashCount = 0;        // Count of backslashes beyond the
                                        // initial '\\' in a UNC.
    BOOL        bColonFound = FALSE;    // No jokes, please ;)
    BOOL        bIsUNC = FALSE;         // True if name starts with two backslashes

#if !defined(SYM_WIN32) && !defined(SYM_NTK)
    BOOL        bCharsFound, bPeriodFound;
    TCHAR       szTemp[SYM_MAX_PATH];
#endif

    SYM_ASSERT ( lpPathName );
    SYM_VERIFY_STRING ( lpPathName );

                                        //-------------------------------
                                        // Illegal size?
                                        //-------------------------------
    uLen = STRLEN(lpPathName);
    if ( uLen == 0 || uLen > SYM_MAX_PATH )
        return(FALSE);
                                        // UNC starts with '\\'
    bIsUNC = lpPathName[0] == '\\' && lpPathName[1] == '\\';

#if 0
                                        //-------------------------------
                                        // This is old code invalid code
                                        // that should not be used again.
                                        // It was left here just in case
                                        // some unexpected situations are
                                        // discovered.
                                        //-------------------------------
    if (!bAllowWild)
        {
        szIllegals[0] = '*';            // add wildcards to the list
        szIllegals[1] = '?';            // of illegal characters
        uNumChars = NameGetIllegalChars(&szIllegals[2], FALSE) + 2;
        }
    else
        uNumChars = NameGetIllegalChars(szIllegals, FALSE);
#endif
                                        //-------------------------------
                                        // Illegal characters?
                                        //-------------------------------

    if ( !bAllowWild )
        {
        wIllegalFlags |= SYM_ILLEGAL_WILDCARDCHARS;
        }

    uNumChars = NameReturnIllegalChars(wIllegalFlags, lpPathName,
                                       szIllegals, sizeof(szIllegals)/sizeof(*szIllegals));

    lpTemp = lpPathName;

    if (bIsUNC)
      {
      lpTemp += 2;                      // Skip the initial '\\'
      if (*lpTemp == '\\')
         return FALSE;                  // A path of "\\\" is invalid.
      }

    while (*lpTemp)
        {
        if (*lpTemp == ':')
            {
            if (bColonFound)            // Multiple ':' is illegal
                return FALSE;
            bColonFound = TRUE;
            lpTemp++;                   // DBCS safe since we know we have
                                        // a single byte character.
            }
        else if (*lpTemp == '\\')
            {
                                        // DBCS safe since we know we're
                                        // on a single byte character etc.
            if (lpSlashPos && lpSlashPos == (lpTemp - 1))
                return FALSE;           // Double backslash in interior of
                                        // path is illegal.
            uSlashCount++;
            lpSlashPos = lpTemp;
            lpTemp++;                   // DBCS safe since we know we have
                                        // a single byte character.
            }
        else
            {
            for (i = 0; i < uNumChars; i++)
               if (*lpTemp != '.' && *lpTemp == szIllegals[i])
                   return(FALSE);
            lpTemp = AnsiNext(lpTemp);
            }
        }

    if (bIsUNC)
        {                               // A UNC name must have at least
                                        // a \\server\volume spec.
        if (!uSlashCount)
            return FALSE;
                                        // If name is "\\server\" then
                                        // it's invalid.
        if (uSlashCount == 1 && lpSlashPos == (lpPathName + uLen  - 1))
            return FALSE;
        }

                                        //-------------------------------
                                        // Make sure drive letter is legal
                                        // Can only have drives 'A'-'Z'
                                        //-------------------------------
    lpTemp = lpPathName;
    if (!DBCSIsLeadByte(*lpPathName) && *(lpPathName + 1) == ':')
        {
        TCHAR   ch;

        ch = *lpPathName = CharToUpper (*lpPathName);
        if ( ch < 'A' || ch > 'Z' )
            return(FALSE);
        }


#if !defined(SYM_WIN32) && !defined(SYM_NTK)
    // ######################################################################
    // These checks should only be done in non W32 (LFN) environments;
    //
    // 1.- Truncating a file and comparing it with it's original filename is
    //     an incorrect validation method for long filenames.
    //
    // 2,- A long filename can have several periods between '\' so checking
    //     for periods is also erroneous for long filenames.
    //
    // ######################################################################

/*-----------------01-27-95 04:26pm-----------------
 I added a cover around the following functions
 such that if the path is a UNC do not execute
 the following tests.  UNC's do not need to follow
 8.3 standard.              -brian
--------------------------------------------------*/
    if (!bIsUNC)
    {
                                        //-------------------------------
                                        // Convert all forward slashes to
                                        // backslashes
                                        //-------------------------------
        NameConvertFwdSlashes(lpPathName);

        STRCPY(szTemp, lpPathName);         // Make a copy
        NameTruncate (szTemp);              // Put it into 8.3 format

                                        // Will only occur if path components
                                        // are not 8.3
        if (STRCMP(szTemp, lpPathName) != 0)
            return(FALSE);

                                        //-------------------------------
                                        // Make sure we don't have extra
                                        // periods in the filename.
                                        //-------------------------------
        lpTemp = lpPathName;
        bPeriodFound = bCharsFound = FALSE;
        while (*lpTemp)
            {
            if ( *lpTemp == '\\' || *lpTemp == ':' )
                {
                                        // Reset states, since we are on a
                                        // new component
                bPeriodFound = bCharsFound = FALSE;
                }
            else if ( *lpTemp == '.')
                {
                                        // If we find a '.' and we have already
                                        // found characters and we already have
                                        // a period in this path component, then
                                        // the path is bogus.
                if (bPeriodFound && bCharsFound)
                    return(FALSE);
                bPeriodFound = TRUE;        // We found a period
                }
            else
                {
                bCharsFound = TRUE;
                }

            lpTemp = AnsiNext(lpTemp);      // Process next character
            }
    }
#endif

    return (TRUE);
}
#endif                                  //  #ifdef SYM_OS2

/*----------------------------------------------------------------------*
 * This procedure checks the full name to see if it has any DOS wild-   *
 * card characters ("*" or "?") in the last part of the name.           *
 *                                                                      *
 * Returns:     TRUE    If there are wild cards                         *
 *              FALSE   If there are no wild card characters            *
 *----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI NameIsWildcard (LPCTSTR lpPathName)
{
                                        // Point to end of name
    LPTSTR      lpEnd = (LPTSTR) &lpPathName [STRLEN (lpPathName)];

    while (lpEnd-- > lpPathName && ((*lpEnd != ':' && *lpEnd != '\\') ||
            DBCSGetByteType ( (LPSTR)lpPathName, lpEnd ) != _MBC_SINGLE ))
        {
        if (*lpEnd == '?' || *lpEnd == '*')
             return (TRUE);             // There's at least one wild card
        }

    return (FALSE);                     // There are no wild cards
}



/*----------------------------------------------------------------------*
 * NameIsDevice()                                                       *
 *                                                                      *
 * Determines if a filename represents a device                         *
 *                                                                      *
 * Returns:     TRUE    filename is a device                            *
 *              FALSE   filename is not a device                        *
 *----------------------------------------------------------------------*/
BOOL SYM_EXPORT WINAPI NameIsDevice (LPCTSTR lpName)
{
#if defined(SYM_VXD) || defined(SYM_NTK)
    return(FALSE);
#else

    auto        TCHAR           szDeviceName[SYM_MAX_PATH+1];
    auto        int             nLen;

#if defined(SYM_WIN32)
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        {
        auto        HFILE           hFile;
        auto        DWORD           dwType;
        auto        BOOL            bResult = FALSE;

        hFile = FileOpen(lpName, OF_READ);
        if (hFile != HFILE_ERROR)
            {
            dwType = GetFileType((HANDLE) hFile);
            bResult = (BOOL)((dwType & FILE_TYPE_DISK) ? FALSE : TRUE);
            FileClose(hFile);
            }
        return(bResult);
        }
#endif

    // Isolate just the filename portion of what may be a full path
    NameReturnFile(lpName, szDeviceName);
    NameStripExtension(szDeviceName);

    // Turn name into an upper case, 8 byte, space padded string
    szDeviceName[8] = 0;
    if ((nLen = STRLEN(szDeviceName)) < 8)
        {
        // Fill out end of string with spaces
        while (nLen < 8)
            szDeviceName[nLen++] = ' ';
        }

    STRUPR(szDeviceName);

    return (PackedNameIsDevice(szDeviceName));
#endif
}

#define DOS20_DEVICE_OFFSET     0x17    // Device driver offset DOS 2.0+
#define DOS30_DEVICE_OFFSET     0x28    // Device driver offset DOS 3.0+
#define DOS31_DEVICE_OFFSET     0x22    // Device driver offset DOS 3.1+

#if defined(SYM_WIN32)
typedef struct
    {
    DWORD       dwPackedName[2];
    } *PDEVICE;
#endif

BOOL SYM_EXPORT WINAPI PackedNameIsDevice(LPCTSTR lpDeviceName)
{
#if defined(SYM_VXD) || defined(SYM_NTK)
    return(FALSE);
#elif defined(SYM_WIN32)
#ifndef _M_ALPHA
    auto        SYM_REGS        regs;
    auto        BYTE            far *lpDosVars;
    auto        DeviceDriverRec DriverRec;
    auto        UINT            uCount;
    auto        LPDWORD         lpDWord = (LPDWORD) lpDeviceName;
    static      UINT            uDeviceCount;
    static      LPDRIVER        lpFirstDriver = NULL;
    static      PDEVICE         lpDevice = NULL;

    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        {
#endif        	
        auto        HFILE           hFile;
        auto        DWORD           dwType;
        auto        BOOL            bResult = FALSE;
        auto        BYTE            szDeviceName[SYM_MAX_PATH+1];

        NameToUnpacked(szDeviceName, lpDeviceName);
        hFile = FileOpen(szDeviceName, OF_READ);
        if (hFile != HFILE_ERROR)
            {
            dwType = GetFileType((HANDLE) hFile);
            bResult = (BOOL)((dwType & FILE_TYPE_DISK) ? FALSE : TRUE);
            FileClose(hFile);
            }
        return(bResult);
#ifndef _M_ALPHA
        }

    if (lpDevice == NULL)
        {
        MEMSET(&regs, 0, sizeof(regs));

        /*----------------05-30-95 09:40am------------------
        ** Use DOS 'Get List of Lists' function to get
        ** pointer to dos internal vars.  Only do this once.
        ** ------------------------------------------------*/
        if (lpFirstDriver == NULL)
            {
                                // Get list of lists.
            regs.EAX = 0x00005200;
            if( !VxDIntBasedCall( 0x21, &regs ) )
                return(FALSE);

            lpDosVars = (LPBYTE) regs.EBX;
            lpFirstDriver = (DeviceDriverRec far *) (lpDosVars + DOS31_DEVICE_OFFSET);
            }

        uDeviceCount = 0;
        /*----------------05-30-95 12:17pm------------------
        ** 1st DeviceDriver is NUL device.
        ** ------------------------------------------------*/
        MemCopyPhysical(&DriverRec, lpFirstDriver, sizeof(DeviceDriverRec), 0);

        /*----------------05-30-95 12:23pm------------------
        ** Now, walk list of drivers, looking for ours.
        ** ------------------------------------------------*/
        while (TRUE)
            {
            if((DWORD)DriverRec.next == -1 )
                break;
            else            // Get next driver entry...
                {
                MemCopyPhysical(&DriverRec, DriverRec.next, sizeof(DeviceDriverRec), 0);
                uDeviceCount++;
                }
            }

        lpDevice = MemAllocFixed(uDeviceCount * sizeof(*lpDevice));

        MemCopyPhysical(&DriverRec, lpFirstDriver, sizeof(DeviceDriverRec), 0);

        for (uCount = 0; uCount < uDeviceCount; uCount++)
            {
            MEMCPY(lpDevice[uCount].dwPackedName, DriverRec.name, sizeof(lpDevice[0].dwPackedName));
            MemCopyPhysical(&DriverRec, DriverRec.next, sizeof(DeviceDriverRec), 0);
            }
        }

    for (uCount = 0; uCount < uDeviceCount; uCount++)
        if (lpDWord[0] == lpDevice[uCount].dwPackedName[0] &&
            lpDWord[1] == lpDevice[uCount].dwPackedName[1])
            return (TRUE);
    return (FALSE);

#endif // _M_ALPHA
#else
    LPDRIVER    lpDriver;
    BOOL        bResult = FALSE;
#ifdef  SYM_PROTMODE
    UINT        uSelector;
#endif

    // Walk driver list
    lpDriver = DOSGetFirstDriverAddress();

#if defined(SYM_PROTMODE)
    lpDriver = GetProtModePtr(lpDriver);
    uSelector = FP_SEG(lpDriver);
#endif
    while (!bResult)
        {                               // offset of FFFF means the end
        if (FP_OFF (lpDriver) == 0xFFFF)
            break;
                                        // compare the driver name with ours.
                                        // block device drives probably will
                                        // not have names, just garbage, but
                                        // that's not a problem.
        if (MEMCMP(lpDeviceName, lpDriver->name, 8) == 0)
            bResult = TRUE;
        else
            {
            lpDriver = lpDriver->next;  // get the next driver in the chain
#if defined(SYM_PROTMODE)
            SelectorSetDosAddress(&uSelector, MAKELP(FP_SEG(lpDriver), 0));
            FP_SEG(lpDriver) = uSelector;
#endif
            }
        }

#if defined(SYM_PROTMODE)
    SelectorFree(uSelector);
#endif
    return (bResult);
#endif
}

/*----------------------------------------------------------------------*
 * DOSGetFirstDriverAddress()                                           *
 *                                                                      *
 * Returns the address of the first device driver.                      *
 * The whole driver chain can be walked by starting at this address.    *
 *                                                                      *
 * NOTE: This address is a REAL MODE address                            *
 *----------------------------------------------------------------------*/

#if !defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)


LPDRIVER LOCAL PASCAL DOSGetFirstDriverAddress (void)
{
#if defined(SYM_OS2)
    return NULL;
#else
#ifndef SYM_WIN
    static      LPDRIVER        lpFirstDriver = NULL;
#else
    auto        LPDRIVER        lpFirstDriver;
#endif
    auto        BYTE            far *lpDosVars;
    auto        UINT            wVersion;

    wVersion = DOSGetVersion();

#ifndef SYM_WIN
    // The address can't change out from under us, so only do the work once
    if (lpFirstDriver == NULL)
#endif
        {
        // Get pointer to DOS List Of Lists
        DOSGetInternalVariables(&lpDosVars);

        // Get pointer to NUL device (always start of driver chain)
        if (wVersion < DOS_3_00)
            {
            // DOS 2.0+
            lpFirstDriver = (DeviceDriverRec far *) (lpDosVars + DOS20_DEVICE_OFFSET);
            }
        else if (wVersion == DOS_3_00)
            {
            // DOS 3.0
            lpFirstDriver = (DeviceDriverRec far *) (lpDosVars + DOS30_DEVICE_OFFSET);
            }
        else
            {
            // DOS 3.1+
            lpFirstDriver = (DeviceDriverRec far *) (lpDosVars + DOS31_DEVICE_OFFSET);
            }
        }

    return(lpFirstDriver);
#endif  //  #ifdef SYM_OS2
}
#endif                                  // #if !defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      Given a pathname, this function will adjust the last component  *
 *      until the pathname is unique.  The pathname may be padded with  *
 *      alphanumeric characters to accomplish this.                     *
 *                                                                      *
 *      NOTE: This routine makes the assumption that the pathname can   *
 *      be expanded in size up to seven characters!  Make sure you      *
 *      take this into account, or simply pass in a buffer of at least  *
 *      SYM_MAX_PATH!!!                                                 *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 05/20/1993 ED Function Created.                                      *
 ************************************************************************/

UINT SYM_EXPORT WINAPI NameMakeUnique (LPTSTR lpPath)
{
    LPTSTR      lpFile;
    LPTSTR      lpExt;
    LPTSTR      lpLast;
    TCHAR       szExt [5];
    UINT        wLen;
    UINT        wRet = NOERR;

                                        // do a quick check before getting
                                        // too carried away
    if (!(NameGetType(lpPath) & (NGT_IS_DIR | NGT_IS_FILE)))
        return (NOERR);

                                        // find the filename portion
    lpFile = FindFileName (lpPath, TRUE);

    if (lpExt = STRCHR (lpFile, '.'))   // do we have an extension?
        {
        STRCPY (szExt, lpExt);          //      save it, then
        *lpExt = '\0';                  //      kill it for now
        }

    wLen = STRLEN (lpFile);             // size the filename

    lpLast = lpFile + wLen - 1;         // get the last character

    while (TRUE)
        {
        if (wLen < 8)                   // can we pad the name?
            {
            wLen++;
            *(++lpLast) = '0';          // pad with a zero character
            }
        else
            wRet = IncDigit (lpFile, lpLast);

        if (lpExt)                      // replace the extension
            STRCPY (lpLast + 1, szExt);
        else
            *(lpLast + 1) = '\0';       // end the string

        if (wRet == ERR)
            break;

        if (!(NameGetType(lpPath) & (NGT_IS_DIR | NGT_IS_FILE)))
            break;
        }

    return (wRet);
}

        // helper function, used by NameMakeUnique

UINT LOCAL PASCAL IncDigit (LPTSTR lpLimit, LPTSTR lpPos)
{
    UINT        wRet = NOERR;

    if (lpPos < lpLimit)        // oops! too far
        return (ERR);

    (*lpPos)++;

    if (*lpPos == ':')          // check for numerical overflow
        *lpPos = 'A';

    if (*lpPos == '[')          // check for alphabetic overflow
        {
        *lpPos = '0';           // reset the current character

                                // adjust the adjacent character
        wRet = IncDigit (lpLimit, lpPos - 1);
        }

    return (wRet);
}

#endif          // #ifndef SYM_NLM


////////////////////////////////////////////////////////////////////////
//
//   FindFileName()
//
// Description:
//   Helper function, used by NameStripFile() and NameMakeUnique().
//
//   If the bSkipSlash parameter is TRUE, then no returned values will
//   point at a backslash.
//   The following describes the behavior when bSkipSlash is FALSE:
//   If lpName does not contain '\' or ':', return lpName.
//   If lpName contains '\NAME', return pointer to 'NAME'.
//   If lpName contains 'A:\NAME', return pointer to 'NAME'.
//   If lpName contains 'A:\DIR\NAME', return pointer to '\NAME'
//   If lpName contains '\\SERVER', return pointer to '\0' (no filename)
//   If lpName contains '\\SERVER\VOLUME', return pointer to '\0' (no filename)
//   If lpName contains '\\SERVER\VOLUME\NAME', return pointer to '\NAME'
//
//   When the bSkipSlash parameter is FALSE, this function will return a
//   pointer to a '\' only when lpName contains a path beyond the root
//   (deeper than 1), otherwise it will not.
//
// See Also:
//   NameStripFile(), NameMakeUnique()
//
////////////////////////////////////////////////////////////////////////
// 12/15/94 BEM Comment header created.
////////////////////////////////////////////////////////////////////////

LPTSTR LOCAL PASCAL FindFileName (LPCTSTR lpName, BOOL bSkipSlash)
{
    LPCTSTR     p;
    LPCTSTR     lpszFound;
    LPCTSTR     pNullByte;
    BOOL        bIsUNC = FALSE;
    int         nSlashCount = 0;        // Count of slashes beyond '\\' in UNC

    SYM_ASSERT ( lpName );
    SYM_VERIFY_STRING ( lpName );


    p = lpszFound = lpName;
                                        // Find the last '\' or ':' in lpName
                                        // Detect UNC while we go through
    while (*p)
        {
        if (*p == ':')
            lpszFound = p;
        else if (*p == '\\')
            {
            if (bIsUNC)
               nSlashCount++;
                                        // If the first and second characters
                                        // are '\', then its a UNC.
            if (lpszFound == lpName &&
                *lpszFound == '\\' && (p == lpszFound+1))
                bIsUNC = TRUE;
            lpszFound = p;
            }
        p = AnsiNext(p);
        }

    pNullByte = p;
    p = lpszFound;                      // Points to the last occurance of
                                        // '\' or ':' in lpName, or
                                        // points to the first char of
                                        // lpName if none was found.
                                        //

    if (*p == ':')                      // Is this a drive letter?
        p++;                            // Yes, then keep it.
                                        // This is DBCS safe because we
                                        // know we're not on a lead byte.

    if (p == lpName && *p == '\\')      // Is this file in root dir?
        p++;                            // Yes, keep the '\'
                                        // This is DBCS safe because we
                                        // know we're not on a lead byte.
    else
        {
        if (bIsUNC)                     // If it's a UNC, make sure that
                                        // there was a file name, which
                                        // must come after \\SERVER\VOLUME
                                        // so the number of slashes past
                                        // the initial '\\' must be 2 or more
                                        // otherwise there wasn't a name.
            {
            if (nSlashCount < 2)
                p = pNullByte;          // No file name, so return ptr to '\0'
            }
        else if (p > lpName && *p == '\\' && *AnsiPrev(lpName, p) == ':')
            p++;                        // 'A:\', keep the '\'
                                        // This is DBCS safe because we
                                        // know we're not on a lead byte.
        }

    if (bSkipSlash && *p == '\\')
        p++;                            // This is DBCS safe because we
                                        // know we're not on a lead byte.

    return ((LPTSTR)p);
}


#ifndef SYM_NLM

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      This function will create a path using the specified directory  *
 *      name and the filename from the specified path.  I got sick of   *
 *      doing this in lots of places in my own code, so here it is....  *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 06/29/1993 ED Function Created.                                      *
 ************************************************************************/

void SYM_EXPORT WINAPI NameDerivePath (LPCTSTR lpPath, LPCTSTR lpNewDir, LPTSTR lpReturnPath)
{
    TCHAR       szFileName [SYM_MAX_PATH + 1];

    STRCPY (lpReturnPath, lpNewDir);    // use the specified directory

                                        // get the filename from the original
                                        // path and tack it on
    NameReturnFile (lpPath, szFileName);
    NameAppendFile (lpReturnPath, szFileName);
}

#endif  // ifndef SYM_NLM


#if !defined(SYM_OS2) && !defined(SYM_NLM)

////////////////////////////////////////////////////////////////////////
//
//   NameReturnLongName()
//
// Description:
//   Retrieves the full path in its long filename form for the specified file
//   or path.  The function returns the long name for all directories in the
//   path.
//
//   Relative paths containing the characters "." and ".." in
//   lpszSpec are fully expanded.  Since this function
//   performs validation, the lpszSpec must contain either a valid
//   filename or path.
//
//   If bFmtForDisp is TRUE, then the output is formatted as Explorer
//   would display - that is, 8.3 names have the first letter capitalized,
//   and the rest lower case, while LFNs are as they appear on disk.
//
//   If bFmtForDisp is FALSE, then the output is formatted as it comes
//   from FileFindFirst() - that is, 8.3 names are all capital, while
//   LFNs are as they appear on disk.
//
//   SYM_NTK: lpszSpec should not contain relative path segments and
//            STRLEN ( lpszSpec ) < SYM_MAX_PATH.  The size in characters
//            of lpszLongSpec is assumed to be AT LEAST SYM_MAX_PATH!
//            Since there is no information regarding the size of the
//            buffer at lpszLongSpec, the memory following it will be
//            destroyed if the resulting filename is too long.  If this
//            function fails, lpszLongSpec will be undefined. bFmtForDisp
//            should be FALSE.
//
// See Also:
//   NameReturnShortName(), the Win32 SDK\DOCS\GUIDE_2.DOC
//
////////////////////////////////////////////////////////////////////////
// 11/21/94 BEM Function created.
////////////////////////////////////////////////////////////////////////
                                        // Get long version of every component
                                        // of lpszSpec
STATUS SYM_EXPORT WINAPI NameReturnLongName(
   LPCTSTR lpszSpec,                    // Existing name you want as long
   LPTSTR lpszLongSpec,                 // Place to return long name
   BOOL bFmtForDisp)                    // TRUE to format for display
{
#if defined(SYM_VXD)

    auto char szSource[SYM_MAX_PATH * sizeof(WCHAR)];
    auto char szDestination[SYM_MAX_PATH * sizeof(WCHAR)];
    auto UINT uCurCharSet;

    uCurCharSet = ((PTCB)Get_Cur_Thread_Handle())->TCB_Flags & THFLAG_CHARSET_MASK;

    if ( VxDGenericStringConvert ( (PSTR)lpszSpec,
                                   THFLAG_OEM,
                                   szSource,
                                   uCurCharSet ) == FALSE )
        {
        return ( ERR );
        }

    if ( VxDNameReturnLongName ( szSource, szDestination ) != 0 )
        {
        return ( ERR );
        }

    if ( VxDGenericStringConvert ( (PSTR)szDestination,
                                   THFLAG_OEM,
                                   lpszLongSpec,
                                   uCurCharSet ) == FALSE )
        {
        return ( ERR );
        }

   return NOERR;

#elif defined(SYM_NTK)

    auto NTSTATUS                     Stat;
    auto UINT                         uCharsCopied, uCount;
    auto PTSTR                        pszNTFileName;
    auto LPTSTR                       lpszRegion;
    auto UNICODE_STRING               FileName;
    auto OBJECT_ATTRIBUTES            ObjectAttributes;
    auto IO_STATUS_BLOCK              IoStatusBlock;
    auto HANDLE                       hPath;
    auto struct {
            FILE_BOTH_DIR_INFORMATION FileInfo;
            TCHAR                     szPadding[SYM_MAX_PATH];
            } FileDirInfo;

    SYM_ASSERT ( lpszSpec );
    SYM_VERIFY_STRING ( lpszSpec );
    SYM_ASSERT ( STRLEN ( lpszSpec ) < SYM_MAX_PATH );
    SYM_ASSERT ( lpszLongSpec );
    SYM_VERIFY_BUFFER ( lpszLongSpec, SYM_MAX_PATH * sizeof(TCHAR) );
    SYM_ASSERT ( !bFmtForDisp );

                                        // Copy the original root name into
                                        // the destination buffer, because it
                                        // does not change during the
                                        // conversion.  uCharsCopied from
                                        // here on will represent how deep
                                        // into lpszLongSpec we are.

    uCharsCopied = FileCopyRootName ( lpszSpec, lpszLongSpec );

    SYM_ASSERT ( uCharsCopied < SYM_MAX_PATH );

    if ( !uCharsCopied || uCharsCopied >= SYM_MAX_PATH )
        {
        return ( ERR );
        }

    uCharsCopied--;

                                        // Create a native NT name.

    if ( ! ( pszNTFileName = FileCreateNativeNTName ( lpszSpec ) ) )
        {
        return ( ERR );
        }

                                        // Find out where in the NT native
                                        // name is the location of what we
                                        // had already copied into the
                                        // destination buffer.

    lpszRegion = pszNTFileName +
                 STRLEN ( pszNTFileName ) -
                 STRLEN ( FileRelativeToRootName ( lpszSpec ) ) - 1;

    SYM_ASSERT ( lpszRegion > pszNTFileName );

                                        // Start a loop which converts each
                                        // part of pszNTFileName (pointed to
                                        // by lpszRegion.

    Stat = STATUS_SUCCESS;

    while ( NT_SUCCESS(Stat) && *lpszRegion )
        {
                                        // Open a handle to the directory
                                        // where the section at lpszRegion
                                        // to convert is located.

        FileName.Buffer = pszNTFileName;
        FileName.MaximumLength = \
        FileName.Length = ( lpszRegion - pszNTFileName + 1 ) * sizeof(WCHAR);

        InitializeObjectAttributes ( &ObjectAttributes,
                                     &FileName,
                                     OBJ_CASE_INSENSITIVE,
                                     NULL,
                                     NULL );

        Stat = ZvOpenFile ( &hPath,
                            SYNCHRONIZE | FILE_READ_ATTRIBUTES,
                            &ObjectAttributes,
                            &IoStatusBlock,
                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                            FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT );

        if ( !NT_SUCCESS(Stat) )
            {
            continue;
            }

                                        // Find the end of the region in
                                        // pszNTFileName pointed to by
                                        // pszSection so that we can
                                        // initialize FileName to point to
                                        // that region, and set lpszRegion
                                        // to point to the next region.

        for ( uCount = 0, lpszRegion++;
              lpszRegion[uCount] && lpszRegion[uCount] != '\\';
              uCount++ );

        FileName.Buffer = lpszRegion;
        lpszRegion = &lpszRegion[uCount];
        FileName.MaximumLength = \
        FileName.Length = uCount * sizeof(WCHAR);

                                        // Get information about FileName
                                        // located in hPath.

        Stat = ZvQueryDirectoryFile ( hPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &IoStatusBlock,
                                      &FileDirInfo,
                                      sizeof(FileDirInfo),
                                      FileBothDirectoryInformation,
                                      TRUE,
                                      &FileName,
                                      FALSE );

        if ( !NT_SUCCESS(Stat) )
            {
            ZvClose ( hPath );
            continue;
            }

        Stat = ZvClose ( hPath );

        if ( !NT_SUCCESS(Stat) )
            {
            SYM_ASSERT ( FALSE );
            continue;
            }

                                        // Now append the returned string to
                                        // the destination buffer.

        lpszLongSpec[uCharsCopied++] = '\\';

        FileDirInfo.FileInfo.FileNameLength /= sizeof(TCHAR);

        for ( uCount = 0;
              uCount < FileDirInfo.FileInfo.FileNameLength;
              uCount++ )
            {
            if ( uCharsCopied < SYM_MAX_PATH )
                {
                lpszLongSpec[uCharsCopied++] = FileDirInfo.FileInfo.FileName[uCount];
                }
            }

        if ( uCharsCopied == SYM_MAX_PATH )
            {
            Stat = STATUS_INVALID_BUFFER_SIZE;
            }
        else
            {
            lpszLongSpec[uCharsCopied] = '\0';
            }
        }

    FileDestroyNativeNTName ( pszNTFileName );

    if ( NT_SUCCESS ( Stat ) )
        {
        return ( NOERR );
        }

    // Set last error.

    return ( ERR );

#else
   UINT uRet;
   TOK_BLOCK TokBlock;
   TCHAR szFullSpec[SYM_MAX_PATH];      // Spec to be parsed
   TCHAR szPiece[SYM_MAX_PATH];         // Current piece of spec while parsing
   TCHAR szPath[SYM_MAX_PATH];          // Buffer to use in find first calls
                                        // (builds up path piece by piece)
   UINT uRootLen;
   UINT uPathSize;

   szPath[0] = lpszLongSpec[0] = EOS;

                                        // Removed relative references, add
                                        // current drive and dir if necessary
   if (NameToFull(szFullSpec, lpszSpec) != NOERR)
       return ERR;

                                        // Copy root into find first buffer
   NameReturnRoot(szFullSpec, szPath);
   uRootLen = STRLEN(szPath);
   uPathSize = STRLEN(szFullSpec) - uRootLen;
   STRCPY(lpszLongSpec, szPath);        // Start with the root

   if (NameIsRoot(lpszSpec))            // If it was the root, we're done.
       return NOERR;
                                        // Strip the root from the
                                        // spec to be parsed.
   MEMMOVE(szFullSpec, szFullSpec+uRootLen, uPathSize+1);

   for (uRet = PathTok(&TokBlock, szFullSpec, PATH_CHAR, szPiece, TRUE);
        uRet != ERR;
        uRet = PathTok(&TokBlock, szFullSpec, PATH_CHAR, szPiece, FALSE))
      {
      FINDDATA             findData;
      HFIND                hFindFile;

      NameAppendFile(szPath, szPiece);
      hFindFile = FileFindFirst (szPath, &findData);
      if (hFindFile == INVALID_HANDLE_VALUE)
         return ERR;
      FileFindClose(hFindFile);
                                        // If formatting as Explorer would
                                        // and we have only an 8.3 name,
                                        // then lower case everthing but
                                        // the first character.
      if (bFmtForDisp && !findData.szAlternateFileName[0])
         STRLWR(AnsiNext(findData.szFileName));
      NameAppendFile(lpszLongSpec, findData.szFileName);
      }

   return NOERR;
#endif
}
#endif  // ifndef SYM_NLM & SYM_OS2


#if !defined(SYM_OS2) && !defined(SYM_NLM)

////////////////////////////////////////////////////////////////////////
//
//   NameReturnShortName()
//
// Description:
//   This function returns the short (8.3) version for every component of
//   lpszSpec.
//
//   This function uses the new LFN int 21 function.
//
//   Relative paths containing the characters "." and ".." in
//   lpszSpec are fully expanded.  Since this function
//   performs validation, the lpszSpec must contain either a valid
//   filename or path.
//
//   SYM_NTK: lpszSpec should not contain relative path segments and
//            STRLEN ( lpszSpec ) < SYM_MAX_PATH.  The size in characters
//            of lpszShortSpec is assumed to be AT LEAST SYM_MAX_PATH!
//            Since there is no information regarding the size of the
//            buffer at lpszShortSpec, the memory following it will be
//            destroyed if the resulting filename is too long.  If this
//            function fails, lpszShortSpec will be undefined.
//
// See Also:
//   NameReturnLongName(), the Win32 SDK\DOCS\GUIDE_2.DOC
//
////////////////////////////////////////////////////////////////////////
// 11/21/94 BEM Function created.
////////////////////////////////////////////////////////////////////////

#pragma optimize("", off)
                                        // Get short (8.3) version of
                                        // every component of lpszSpec
STATUS SYM_EXPORT WINAPI NameReturnShortName(
   LPCTSTR lpszSpec,
   LPTSTR lpszShortSpec)                 // Place to return short version
{
#if defined(SYM_WIN32)

    DWORD dwReturn;
    BOOL bApisAreANSI = AREFILEAPISANSI;
    TCHAR szLongFilePath[SYM_MAX_PATH];

                                            // GetShortPathName will not translate
                                            // high ASCII chars in file or directory names
                                            // to fix this problem and keep everything as
                                            // before check if running under
                                            // NT and then use the fixed code.
    if (SystemGetWindowsType() != SYM_SYSTEM_WIN_NT)
        {
                                            //-------------------------------
                                            // Try the standard method first
                                            //-------------------------------
        dwReturn = GetShortPathName (lpszSpec, lpszShortSpec, SYM_MAX_PATH);

        if (dwReturn > SYM_MAX_PATH)
            return (ERR);

        if (dwReturn != 0)
            return (NOERR);

                                            //-------------------------------
                                            // If GetShortPathName() fails it
                                            // is most likely due to non-ANSI
                                            // characters in the input. If the
                                            // call doesn't work we'll try it
                                            // the hard way way.
                                            //-------------------------------
        if (bApisAreANSI)
            SET_FILE_APIS_TO_OEM;
                                            // Make a copy that we can ruin
        STRCPY (szLongFilePath, lpszSpec);

        dwReturn = NameReturnShortNameHardWay (szLongFilePath,
                                            lpszShortSpec);
        if (bApisAreANSI)
            SET_FILE_APIS_TO_ANSI;

        if (dwReturn == ERR
        ||  dwReturn > SYM_MAX_PATH)
            return (ERR);

        return (NOERR);
        }
    else
        {
                                            // if we used set api's to OEM
                                            // this will make
                                            // NameReturnShortNameHardWay() to fail.
                                            // with a high ASCII chars
                                            // the calling function should
                                            // Set api's to ANSI and try to
                                            // get a valid SFN. This should
                                            // contain only ANSI-OEM mappable
                                            // characters.
        if (!bApisAreANSI)
            SET_FILE_APIS_TO_ANSI;
                                            // Make a copy that we can ruin

        STRCPY (szLongFilePath, lpszSpec);

        dwReturn = NameReturnShortNameHardWay (szLongFilePath, lpszShortSpec);

        if (!bApisAreANSI)
            SET_FILE_APIS_TO_OEM;

        if (dwReturn == ERR || dwReturn > SYM_MAX_PATH)
            return (ERR);

        return (NOERR);
        }

#elif defined(SYM_VXD)
    auto char szSource[SYM_MAX_PATH * sizeof(WCHAR)];
    auto char szDestination[SYM_MAX_PATH * sizeof(WCHAR)];
    auto UINT uCurCharSet;

    uCurCharSet = ((PTCB)Get_Cur_Thread_Handle())->TCB_Flags & THFLAG_CHARSET_MASK;

    if ( VxDGenericStringConvert ( (PSTR)lpszSpec,
                                   THFLAG_OEM,
                                   szSource,
                                   uCurCharSet ) == FALSE )
        {
        return ( ERR );
        }

    if ( VxDNameReturnShortName ( szSource, szDestination ) != 0 )
        {
        return ( ERR );
        }

    if ( VxDGenericStringConvert ( (PSTR)szDestination,
                                   THFLAG_OEM,
                                   lpszShortSpec,
                                   uCurCharSet ) == FALSE )
        {
        return ( ERR );
        }

    return (NOERR);

#elif defined(SYM_NTK)

    auto NTSTATUS                 Stat;
    auto UINT                     uCharsCopied, uCount;
    auto PTSTR                    pszNTFileName;
    auto LPTSTR                   lpszRegion;
    auto UNICODE_STRING           FileName;
    auto OBJECT_ATTRIBUTES        ObjectAttributes;
    auto IO_STATUS_BLOCK          IoStatusBlock;
    auto HANDLE                   hPath;
    auto struct {
            FILE_NAME_INFORMATION FileInfo;
            TCHAR                 szPadding[8 + 3 + 1];
            } FileDirInfo;

    SYM_ASSERT ( lpszSpec );
    SYM_VERIFY_STRING ( lpszSpec );
    SYM_ASSERT ( STRLEN ( lpszSpec ) < SYM_MAX_PATH );
    SYM_ASSERT ( lpszShortSpec );
    SYM_VERIFY_BUFFER ( lpszShortSpec, SYM_MAX_PATH * sizeof(TCHAR) );

                                        // Copy the original root name into
                                        // the destination buffer, because it
                                        // does not change during the
                                        // conversion.  uCharsCopied from
                                        // here on will represent how deep
                                        // into lpszShortSpec we are.

    uCharsCopied = FileCopyRootName ( lpszSpec, lpszShortSpec );

    SYM_ASSERT ( uCharsCopied < SYM_MAX_PATH );

    if ( !uCharsCopied || uCharsCopied >= SYM_MAX_PATH )
        {
        return ( ERR );
        }

    uCharsCopied--;

                                        // Create a native NT name.

    if ( ! ( pszNTFileName = FileCreateNativeNTName ( lpszSpec ) ) )
        {
        return ( ERR );
        }

                                        // Find out where in the NT native
                                        // name is the location of what we
                                        // had already copied into the
                                        // destination buffer.

    lpszRegion = pszNTFileName +
                 STRLEN ( pszNTFileName ) -
                 STRLEN ( FileRelativeToRootName ( lpszSpec ) ) - 1;

    SYM_ASSERT ( lpszRegion > pszNTFileName );

                                        // Start a loop which converts each
                                        // part of pszNTFileName (pointed to
                                        // by lpszRegion.

    Stat = STATUS_SUCCESS;

    while ( NT_SUCCESS(Stat) && *lpszRegion )
        {
                                        // Find the end of the region in
                                        // pszNTFileName that is currently
                                        // pointed to by lpszSection.

        for ( uCount = 0, lpszRegion++;
              lpszRegion[uCount] && lpszRegion[uCount] != '\\';
              uCount++ );

        lpszRegion = &lpszRegion[uCount];

                                        // Open a handle to the region in
                                        // pszNTFileName that was pointed to
                                        // to by lpszSection before its
                                        // change above.

        FileName.Buffer = pszNTFileName;
        FileName.MaximumLength = \
        FileName.Length = ( lpszRegion - pszNTFileName ) * sizeof(WCHAR);

        InitializeObjectAttributes ( &ObjectAttributes,
                                     &FileName,
                                     OBJ_CASE_INSENSITIVE,
                                     NULL,
                                     NULL );

        Stat = ZvOpenFile ( &hPath,
                            SYNCHRONIZE | FILE_READ_ATTRIBUTES,
                            &ObjectAttributes,
                            &IoStatusBlock,
                            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                            FILE_SYNCHRONOUS_IO_NONALERT );

        if ( !NT_SUCCESS(Stat) )
            {
            continue;
            }

                                        // Get information about the opened
                                        // file/dir.

        Stat = ZvQueryInformationFile ( hPath,
                                        &IoStatusBlock,
                                        &FileDirInfo,
                                        sizeof(FileDirInfo),
                                        FileAlternateNameInformation );

        if ( !NT_SUCCESS(Stat) )
            {
            ZvClose ( hPath );
            continue;
            }

        Stat = ZvClose ( hPath );

        if ( !NT_SUCCESS(Stat) )
            {
            SYM_ASSERT ( FALSE );
            continue;
            }

                                        // Now append the returned string to
                                        // the destination buffer.

        lpszShortSpec[uCharsCopied++] = '\\';

        FileDirInfo.FileInfo.FileNameLength /= sizeof(TCHAR);

        for ( uCount = 0;
              uCount < FileDirInfo.FileInfo.FileNameLength;
              uCount++ )
            {
            if ( uCharsCopied < SYM_MAX_PATH )
                {
                lpszShortSpec[uCharsCopied++] = FileDirInfo.FileInfo.FileName[uCount];
                }
            }

        if ( uCharsCopied == SYM_MAX_PATH )
            {
            Stat = STATUS_INVALID_BUFFER_SIZE;
            }
        else
            {
            lpszShortSpec[uCharsCopied] = '\0';
            }
        }

    FileDestroyNativeNTName ( pszNTFileName );

    if ( NT_SUCCESS ( Stat ) )
        {
        return ( NOERR );
        }

    // Set last error.

    return ( ERR );

#elif !defined(SYM_DOSX)

    auto        WORD            wRetCode = NOERR;
    auto        BOOL            bSupported = TRUE;

                                        //-------------------------------
                                        // Get the short file name
                                        //-------------------------------
    _asm
        {
        push    ds
        push    es
        push    si
        push    di

        mov     ax,7160h                ; Get Name
        mov     cx,1                    ; Get Short Name
        lds     si,lpszSpec             ; Get the original name
        les     di,lpszShortSpec        ; Get the results buffer
        stc                             ; Assume not supported
        DOS                             ; Make the DOS call
        jnc     done
        cmp     ax, 7100h               ; Unsupported?
        mov     ax, -1                  ; Return error
        jne     done                    ; No, supported, but error
        mov     bSupported, 0           ; Not supported.

done:
        mov     wRetCode, ax
        pop     di
        pop     si
        pop     es
        pop     ds
        }

    if ( !bSupported )
        STRCPY(lpszShortSpec, lpszSpec);

    return(NOERR);

#else

    STRCPY(lpszShortSpec, lpszSpec);
    return(NOERR);

#endif                                  // if defined(SYM_WIN32) ..else..
}

#if defined(SYM_WIN32)
////////////////////////////////////////////////////////////////////////
//
//   NameReturnShortNameHardWay()
//
// Description:
//   This function returns the short (8.3) version for every component
//   of lpszSpec.
//
//   This function only gets called if the Win32 GetShortPathName failed
//   so we are assuming that the input spec is probably OEM. To get
//   around this we will use a recursive function to call FindFirstFile
//   for each node of the path with the file apis set to OEM. This will
//   give us the equivalent short (8.3) name for each node.
//
////////////////////////////////////////////////////////////////////////
// 02/09/96 JWORDEN Function created.
////////////////////////////////////////////////////////////////////////

STATUS WINAPI NameReturnShortNameHardWay
(
   LPTSTR lpszSpec,
   LPTSTR lpszShortSpec                 // Place to return short version
)
{
    auto WIN32_FIND_DATA *lpFindData = NULL;

    auto LPTSTR lpszLastBackSlash,
                lpszChar;

    auto int iBackSlashCount;

    auto HANDLE hDirectory;

                                        // Dynamically allocate local data
                                        // to avoid stack problems during
                                        // recursion

    auto LPTSTR lpszShortNode = (LPTSTR)MemAllocFixed (SYM_MAX_PATH);

    if (NULL == lpszShortNode)
    {
        return (ERR);
    }

    lpFindData = (WIN32_FIND_DATA *) MemAllocFixed (sizeof (WIN32_FIND_DATA));

    if (NULL == lpFindData)
    {
                                        // If we fail, free our memory.
        MemFreePtr (lpszShortNode);
        return ERR;
    }
                                        // Find the last backslash
                                        // The "do" just gives me a way to
                                        // repeat if I need to. It normally
                                        // should run for only a single
                                        // iteration

    do
    {
        lpszChar = lpszSpec;
        lpszLastBackSlash = NULL;
        iBackSlashCount = 0;

        while (*lpszChar)
        {
            lpszChar = CharNext (lpszChar);
            if (*lpszChar == '\\')
            {
                lpszLastBackSlash = lpszChar;
                iBackSlashCount++;
            }
        }

        if (iBackSlashCount == 0)
            break;

                                        // Check the final node. If it happens
                                        // to be "." chop it off and set the
                                        // pointer to null so we don't try to
                                        // use it again later
        if (!STRCMP (lpszLastBackSlash, "\\."))
        {
            *lpszLastBackSlash = 0;
            lpszLastBackSlash = NULL;
            break;
        }

                                        // If the final node is "\.." then
                                        // we need to backup a node.
        if (!STRCMP (lpszLastBackSlash, "\\.."))
        {
            *lpszLastBackSlash = 0;
            lpszChar = lpszLastBackSlash;
            lpszLastBackSlash = NULL;

            iBackSlashCount--;

            while (lpszChar != lpszSpec)
            {
                lpszChar = CharPrev (lpszSpec, lpszChar);
                if (*lpszChar == '\\')
                {
                    lpszLastBackSlash = lpszChar;
                    break;
                }
            }

            if (lpszLastBackSlash == NULL)
                break;

            break;
        }


                                        // If the final node is "\*.*" then
                                        // we need to get out
        if (!STRCMP (lpszLastBackSlash, "\\*.*"))
        {
            MemFreePtr (lpszShortNode);
            MemFreePtr (lpFindData);
            return (ERR);
        }

        break;

    } while (TRUE);

                                        // If we didn't find a backslash
                                        // we are thru. The return will begin
                                        // the process of unwinding the
                                        // recursion and building the short
                                        // path
    if (iBackSlashCount == 0)
    {
        STRCPY (lpszShortSpec, lpszSpec);
        MemFreePtr (lpszShortNode);
        MemFreePtr (lpFindData);
        return (NOERR);
    }

                                        // Use FindFirstFile to get the
                                        // short (and Long) version of this
                                        // file path
    hDirectory = FindFirstFile (lpszSpec, lpFindData);
    if (hDirectory == INVALID_HANDLE_VALUE)
    {
        MemFreePtr (lpszShortNode);
        MemFreePtr (lpFindData);
        return (ERR);
    }
    FindClose (hDirectory);

    if (STRLEN (lpFindData->cAlternateFileName) > 0)
        STRCPY (lpszShortNode, lpFindData->cAlternateFileName);
    else
        STRCPY (lpszShortNode, lpFindData->cFileName);

                                        // Deep six the last node
    if (lpszLastBackSlash != NULL)
        *lpszLastBackSlash = 0;

                                        // Fetch the next node
    if (ERR == NameReturnShortNameHardWay (lpszSpec, lpszShortSpec))
    {
        MemFreePtr (lpszShortNode);
        MemFreePtr (lpFindData);
        return (ERR);
    }
                                        // When we finally get to here we have
                                        // completely parsed the path and
                                        // have a bazillion short node names
                                        // stored. Now it's just a matter of
                                        // building a new path with the short
                                        // names
    if (*lpszShortSpec)
        STRCAT (lpszShortSpec, "\\");

    STRCAT (lpszShortSpec, lpszShortNode);

    MemFreePtr (lpszShortNode);
    MemFreePtr (lpFindData);

    return (NOERR);

}
#endif                                  // if defined(SYM_WIN32) ..else..

#pragma optimize("", on)
#endif                                  // if !defined(SYM_OS2) && !defined(SYM_NLM)

/* END OF FILE */
/***************/
