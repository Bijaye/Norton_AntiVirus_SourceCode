/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/fop_dir2.c_v   1.2   11 Apr 1996 19:16:28   ACHERNY  $ *
 *                                                                      *
 * Description:                                                         *
 *      Directory Maintenance Functions                                 *
 *                                                                      *
 * NOTE: In Windows, all input MUST be in ANSI.  The routines here      *
 * which directly interface with DOS will upper case and convert to     *
 * OEM before passing to DOS.  All output returned from DOS will be     *
 * converted back to ANSI before being returned to the caller.          *
 *                                                                      *
 * Contains:                                                            *
 *      DirMakePath                                                     *
 *      DirIsRemoveable                                                 *
 *      DirHasChildren                                                  *
 *      DirParentfromPath                                               *
 *      PathRevTok                                                      *
 *      PathTok                                                         *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/fop_dir2.c_v  $ *
// 
//    Rev 1.2   11 Apr 1996 19:16:28   ACHERNY
// Hook up DirMakePath for OS/2
// 
//    Rev 1.1   10 Apr 1996 22:45:54   KSACKIN
// Revised the DirMakePath function so that it would leave you on the same
// drive and path as when you called the function.
//
//    Rev 1.0   26 Jan 1996 20:22:14   JREARDON
// Initial revision.
//
//    Rev 1.31   02 Aug 1995 00:46:14   AWELCH
// Merge changes from Quake 7 branch.
//
//    Rev 1.30.1.2   03 Jul 1995 23:52:12   AWELCH
// Modify INTERNAL_DirMakePath to change directories when
// making multi-level directories on a non-UNC path.  This
// is needed to overcome a limitation under NT whereby you
// can't make directories where the full path to the final directory
// exceeds 64 characters.
//
//    Rev 1.30.1.1   08 Jun 1995 17:45:04   MRIVERA
// In INTERNAL_DirMakePath put in handling of UNC paths to fix defect #37178.
//
//    Rev 1.30.1.0   24 Apr 1995 19:40:40   SCOTTP
// Branch base for version QAK7
//
//    Rev 1.30   05 Apr 1995 22:22:32   MRIVERA
// Commented out SYM_ASSERT on DirMakePath.
//
//    Rev 1.29   20 Mar 1995 19:30:52   BRUCE
// Fixed bug in DirMakePath() - wasn't creating the tail directory
//
//    Rev 1.28   10 Jan 1995 18:39:40   MARKL
// PathTok() now supported in VxD land.
//
//    Rev 1.27   27 Dec 1994 16:26:00   BRUCE
// Changed pathtok and pathrevtok input parm to be an LPCSTR
//
//    Rev 1.26   18 Nov 1994 14:08:14   BRAD
// Made DBCS stuff a little more efficient
//
//    Rev 1.25   02 Nov 1994 16:43:06   BRAD
// Added DBCS support
//
//    Rev 1.24   18 Oct 1994 12:39:42   MARKK
// Fixed bug in makedir
//
//    Rev 1.23   05 May 1994 13:25:48   BRAD
// Added Quake covers for platform differences
//
//    Rev 1.22   25 Mar 1994 13:37:12   BRAD
// New FindFirst stuff
//
//    Rev 1.21   23 Mar 1994 16:28:34   BRAD
// Cleaned up for new FindFirst handle stuff
//
//    Rev 1.20   15 Mar 1994 12:34:02   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.19   25 Feb 1994 15:05:16   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.18   25 Feb 1994 12:22:28   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.17   11 Feb 1994 10:21:04   PGRAVES
// Added call to CloseHandle() in DirIsRemoveable.
//
//    Rev 1.16   08 Feb 1994 19:25:50   PGRAVES
// Win32.
//
//    Rev 1.15   01 Feb 1994 19:55:04   TONY
// Removed MakeDir references and modules from OS/2.  They'll eventually need to be put in, but aren't needed for Anvil and require other source code files.
//
//    Rev 1.14   01 Feb 1994 18:39:52   TONY
// Converted calls to FindFirst and FindNext to OS/2 library functions under OS/2, since the Quake versions of these functions use and pass DOS structures that are completely different from the OS/2 ones and not easily convertable.  FindFirst and FindNext (which remain unconverted) are in the partially-converted FOP_FILE.C.
//
//    Rev 1.13   01 Feb 1994 16:59:06   TONY
// OS/2 conversion - replaced assembly macros with library calls when compiling under OS/2.
//
//    Rev 1.12   30 Sep 1993 06:38:08   DWHITE
// Include DBCS.H for double byte support.
//
//    Rev 1.11   02 Jul 1993 08:51:56   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//
//    Rev 1.10   27 Apr 1993 21:09:38   BRAD
// Changed to use new MAX_* values.
//
//    Rev 1.9   26 Apr 1993 17:16:34   DAVID
// Fixed FindFirst/Next loop in DirHasChildren(); it was ignoring the
// data from the FindFirst call.
//
//    Rev 1.8   21 Feb 1993 18:31:20   BRAD
// No change.
//
//    Rev 1.7   15 Feb 1993 21:03:56   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.6   12 Feb 1993 04:28:30   ENRIQUE
// No change.
//
//    Rev 1.5   22 Jan 1993 13:39:50   DOUGN
// Added DirMakePathNotify and INTERNAL_DirMakePath
//
//
//    Rev 1.4   19 Oct 1992 18:41:32   BILL
// Made DirGetParent truely take a const (was destroying input string)
//
//    Rev 1.3   19 Oct 1992 18:31:38   BILL
// const changes
//
//    Rev 1.2   10 Sep 1992 13:43:54   BRUCE
// Eliminated unnecessary includes
//
//    Rev 1.1   03 Sep 1992 08:14:12   ED
// Added functions to process error codes consistently
//
//    Rev 1.0   03 Sep 1992 08:02:44   ED
// Initial revision.
//
//    Rev 1.0   27 Aug 1992 09:35:08   ED
// Initial revision.
//
//    Rev 1.0   20 Jul 1992 16:59:18   ED
// Initial revision.
 ************************************************************************/

#include <dos.h>
#include "platform.h"
#include "file.h"
#include "xapi.h"
#include "dbcs.h"
#include "disk.h"

#if defined(SYM_OS2)
#   include <dir.h>
#   include <errno.h>
#endif


#if !defined(SYM_VXD)
                                        // Local function definitions
UINT LOCAL PASCAL INTERNAL_DirMakePath (
    LPSTR               lpTopLevel,     // [in]     Top Level Directory
    DIRMAKENOTIFYPROC   fpCallback,     // [in]     Callback function (optional)
    DWORD               dwParam         // [in]     Caller's parameter
);
#endif


#if !defined(SYM_VXD)
/*--------------------------------------------------------------------*/
/* DirMakePath                                                        */
/*    This function is used to make the all non-existing directories  */
/*    in a given path spec.                                           */
/*                                                                    */
/* INPUTS                                                             */
/*    LPSTR     lpTopLevel      pointer to the top level directory.   */
/*                                                                    */
/* RETURNS                                                            */
/*    standard                                                        */
/*--------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI DirMakePath(LPSTR lpTopLevel)
{
    auto        char    szPath[SYM_MAX_PATH+1] = "";
    auto        UINT    uRet;
    auto        BYTE    byDl;
    auto        BOOL    bIsUNC = FALSE;


                                        // Check if we got a UNC
    if ((lpTopLevel[0] == PATH_CHAR) && (lpTopLevel[1] == PATH_CHAR))
        bIsUNC = TRUE;

    if (!bIsUNC)
        {
        byDl = DiskGet();

        DiskSet(*lpTopLevel);

        DirGet(*lpTopLevel,szPath);
        }
                                        // Call internal function for
                                        // DirMakePath without a callback
                                        // function.
    uRet = INTERNAL_DirMakePath(lpTopLevel,NULL,0L);

    if (!bIsUNC)
        {
		DirSet(szPath);

		DiskSet(byDl);
        }

    return (uRet);
}


//------------------------------------------------------------------------
// DirMakePathNotify
//
// This function is used to make all non-existing directories
// in a given path spec and to notify the caller via the callback function
// for each subdirectory created.
//
// NOTE:    Identical to DirMakePath but includes Notify callback function
//          -Doug Neal 1/22/93
//
// Returns: NOERR or error code
//------------------------------------------------------------------------
UINT SYM_EXPORT WINAPI DirMakePathNotify (
    LPSTR               lpTopLevel,     // [in]     Top Level Directory
    DIRMAKENOTIFYPROC   fpCallback,     // [in]     Callback function (optional)
    DWORD               dwParam         // [in]     Caller's parameter
)
{
                                        // Call internal function for
                                        // DirMakePath
    return INTERNAL_DirMakePath (lpTopLevel, fpCallback, dwParam);
}


/*********************************************************************
** DirIsRemoveable()
**
** Description:
**    This function returns TRUE if the directory in szDirName
**    can be removed, FALSE otherwise.
**
** Return values:
**
** See Also:
**
**
** Future enhancements:
**
***********************************************************************
** 01/18/91 BEM Function created.
**********************************************************************/

BOOL SYM_EXPORT WINAPI DirIsRemoveable(LPCSTR szDirName)
{
    auto        FINDDATA        findData;
    auto        char            szPathSpec[SYM_MAX_PATH+1];
    auto        BOOL            bRemoveable = TRUE;
#ifdef SYM_OS2
    struct      ffblk           ffBlk;
#endif


   STRCPY(szPathSpec, szDirName);
   NameAppendFile(szPathSpec, "*.*");

   if (NameGetType(szDirName) & NGT_IS_ROOT)
       {
       bRemoveable = FALSE;
       goto LEAVE_DIRREMOVEABLE;
       }

#ifdef SYM_OS2
    if (findfirst(szPathSpec, &ffBlk, FA_DIREC | FA_RDONLY | FA_HIDDEN | FA_SYSTEM ))
        {
       auto UINT wRet = _doserrno;
       bRemoveable = (wRet == NO_MORE_FILES);
       goto LEAVE_DIRREMOVEABLE;
        }
    while (!findnext(&ffBlk))
      if (!NameIsDoubleDot(ffBlk.ff_name) && !NameIsDot(ffBlk.ff_name))
         {
         bRemoveable = FALSE;
         break;
         }

#else
    {
    HFIND      hFind;

    hFind = FileFindFirst(szPathSpec, &findData);
    if (hFind == INVALID_HANDLE_VALUE)
       {
       auto UINT wRet = NDosGetError();

       bRemoveable = (wRet == NO_MORE_FILES);
       goto LEAVE_DIRREMOVEABLE;
       }

    while (FileFindNext(hFind, &findData))
      if (!NameIsDoubleDot(findData.szFileName) &&
          !NameIsDot(findData.szFileName))
         {
         bRemoveable = FALSE;
         break;
         }

    FileFindClose(hFind);
    }
#endif

LEAVE_DIRREMOVEABLE:
    return(bRemoveable);

} /* DosDirIsRemoveable() */


/*---------------------------------------------------------------------*/
/* DirHasChildren                                                      */
/*    This function returns TRUE if the directory in szDirName         */
/*    has subdirectories                                               */
/*---------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DirHasChildren(LPCSTR szDirName)
{
    auto        FINDDATA        findData;
    auto        char            szPathSpec[SYM_MAX_PATH+1];
    auto        BOOL            bHasChildren = FALSE;
    auto        HFIND           hFind;
#ifdef SYM_OS2
    struct      ffblk   ffBlk;
#endif


   STRCPY(szPathSpec, szDirName);
   NameAppendFile(szPathSpec, "*.*");

#ifdef SYM_OS2
    if (findfirst(szPathSpec, &ffBlk, FA_DIREC))
       goto LEAVE_DIRHASCHILDREN;

   do {
      if (ffBlk.ff_attrib & FA_DIREC)
          {
          if (!NameIsDoubleDot(ffBlk.ff_name) && !NameIsDot(ffBlk.ff_name))
              {
              bHasChildren = TRUE;
              break;
              }
          }
      }
      while (!findnext(&ffBlk)) ;

#else   //  not SYM_OS2

   hFind = FileFindFirst(szPathSpec, &findData);
   if (hFind == INVALID_HANDLE_VALUE)
       goto LEAVE_DIRHASCHILDREN;

   do {
      if (findData.dwFileAttributes & FA_DIR)
          {
          if (!NameIsDoubleDot(findData.szFileName) &&
              !NameIsDot(findData.szFileName))
              {
              bHasChildren = TRUE;
              break;
              }
          }
      }
   while (FileFindNext(hFind, &findData));

   FileFindClose(hFind);

#endif  //  #ifdef SYM_OS2 else

LEAVE_DIRHASCHILDREN:
   return(bHasChildren);
} /* DirHasChildren() */


/*---------------------------------------------------------------------*/
/* DirParentfromPath                                                   */
/*    This function copies the parent directory into szParent.         */
/*                                                                     */
/* INPUTS                                                              */
/*    LPSTR     lpPathBuffer   the current path                        */
/*    LPSTR     lpParent       on exit contains the parent directory.  */
/*                                                                     */
/* RETURNS                                                             */
/*    standard                                                         */
/*---------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI DirParentfromPath (LPSTR lpPathBuffer, LPSTR lpParent)
{
    LPSTR       lpszLastPathChar;

                                        /* Make sure we are not at the root */
    if (NameGetType(lpParent) & NGT_IS_ROOT)
        {
        STRCPY(lpParent,lpPathBuffer);
        return (TRUE);
        }

    // Copy characters and remember the last location of a path character.
    // We traverse this way, so this stuff works in DBCS.
    lpszLastPathChar = NULL;
    while (*lpPathBuffer)
        {
        if (*lpPathBuffer == PATH_CHAR)
            {
                                        // If ends in with a path char, ignore
                                        // it.
            if (*(lpPathBuffer+1) == EOS)
                break;

            lpszLastPathChar = lpPathBuffer;
            }
        if (DBCSIsLeadByte(*lpPathBuffer))
            *lpParent++ = *lpPathBuffer++;
        *lpParent++ = *lpPathBuffer++;
        }
                                        // Now truncate where we found the
                                        // last path character
    if (lpszLastPathChar)
        *lpszLastPathChar = EOS;
    else
        *lpParent = EOS;

    return (NOERR);
}

/*--------------------------------------------------------------------------*/
/* PathRevTok                                                               */
/*    This routine parses a path in reverse, returning the last peice of    */
/*    the path first and each subsequent piece in reverse order.            */
/*                                                                          */
/*    When the root directory is all that is left, the parsing is ended     */
/*    and ERR is returned.                                                  */
/*                                                                          */
/* INPUTS                                                                   */
/*    lpTokBlock is a block of variables needed accross calls - just declare*/
/*               and pass the same block each time.                         */
/*    lpszPath   is the path to be parsed                                   */
/*    nSep       is the path separation character                           */
/*    lpTok      is a buffer to return the directory piece into             */
/*    bFirst     is a boolean which should be TRUE the first time you call  */
/*               for a given path, and false for the rest of the times      */
/*                                                                          */
/* RETURNS                                                                  */
/*    NOERR if a piece of the path is being returned                        */
/*    ERR   if no separation character is found (which could be the end of  */
/*          the parsing)                                                    */
/*--------------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI PathRevTok(      // Parse a path in reverse, returning
                                        // a token for each piece of path
    LPTOK_BLOCK lpTokBlock,             // Token data block
    LPCSTR lpszPath,                    // Path to parse for tokens
    int   nSep,                         // separator character
    LPSTR lpTok,                        // place to return token
    BOOL  bFirst)                       // True if starting a new parse pass
{
    auto   LPBYTE lpLastPos;
    auto   BYTE   bySave;               // Saves truncated character
    auto   UINT   wRet=NOERR;
    auto   size_t Len = SYM_MAX_PATH;   // Needed for macro use

    if (bFirst)
        lpTokBlock->bDone = FALSE;

    if (lpTokBlock->bDone)
        {
        lpTokBlock->bDone = FALSE;
        return((UINT)ERR);
        }

    if (!bFirst)
        {
        bySave = *lpTokBlock->lpStringPos;
        *lpTokBlock->lpStringPos = '\0';// Temporarily truncate string
        }

    lpLastPos = lpTokBlock->lpStringPos;
                                        // search for separator character
    lpTokBlock->lpStringPos = STRRCHR(lpszPath, nSep);

                                        // If first time and path ends in
                                        // a separator character, skip that
    if (bFirst && !*(lpTokBlock->lpStringPos+1))
        {
        bySave = *lpTokBlock->lpStringPos;
        lpLastPos = lpTokBlock->lpStringPos;
        *lpTokBlock->lpStringPos = '\0';
        lpTokBlock->lpStringPos = STRRCHR(lpszPath, nSep);
        bFirst = FALSE;
        }

    if (lpTokBlock->lpStringPos)
        {
        LPBYTE lpSrc;                   // Needed for STRNCPY() macro

        lpSrc = lpTokBlock->lpStringPos+1;

                                        // copy path token to token buffer
        STRCPY(lpTok, lpTokBlock->lpStringPos+1);
        }
    else
        {
        STRCPY(lpTok, lpszPath);
        lpTokBlock->bDone = TRUE;
        }

    if (!bFirst)
        *lpLastPos = bySave;            // restore truncated character

    return(wRet);
} /* PathRevTok() */

#endif // !defined(SYM_VXD)

/*--------------------------------------------------------------------------*/
/* PathTok                                                                  */
/*    This routine parses a path, returning the successive pieces of        */
/*    the path with each call.                                              */
/*                                                                          */
/*                                                                          */
/* INPUTS                                                                   */
/*    lpTokBlock is a block of variables needed accross calls - just declare*/
/*               and pass the same block each time.                         */
/*    lpszPath   is the path to be parsed                                   */
/*    nSep       is the path separation character                           */
/*    lpTok      is a buffer to return the directory piece into             */
/*    bFirst     is a boolean which should be TRUE the first time you call  */
/*               for a given path, and false for the rest of the times      */
/*                                                                          */
/* RETURNS                                                                  */
/*    NOERR if a piece of the path is being returned                        */
/*    ERR   if no separation character is found (which could be the end of  */
/*          the parsing)                                                    */
/*--------------------------------------------------------------------------*/

#pragma optimize("", off)
UINT SYM_EXPORT WINAPI PathTok(         // Parse a path returning
                                        // a token for each piece of path
    LPTOK_BLOCK lpTokBlock,             // Token data block
    LPCSTR lpszPath,                    // Path to parse for tokens
    int   nSep,                         // separator character
    LPSTR lpTok,                        // place to return token
    BOOL  bFirst)                       // True if starting a new parse pass
{
    auto   LPCSTR lpLastPos;
    auto   UINT   wRet=NOERR;
    auto   size_t CpyLen;

    if (bFirst)
        {
        lpLastPos = lpszPath;
        lpTokBlock->bDone = FALSE;
        }

    if (lpTokBlock->bDone)
        {
        lpTokBlock->bDone = FALSE;
        return((UINT)ERR);
        }

    if (!bFirst)
        lpLastPos = lpTokBlock->lpStringPos+1;

                                    // search for separator character
    {
    LPSTR FAR *lplpResult;          // Needed for macro _FSTRCHR()
    BYTE       bySep;               // Also needed for macro

    lplpResult = &lpTokBlock->lpStringPos;
    bySep = (BYTE)nSep;

    *lplpResult = STRCHR(lpLastPos, bySep);
    }

    if (lpTokBlock->lpStringPos)
        {

        CpyLen = (size_t)(lpTokBlock->lpStringPos - lpLastPos);

                                        // copy path token to token buffer
        STRNCPY(lpTok, lpLastPos, CpyLen);

        lpTok[CpyLen] = '\0';           // NULL terminate token
                                        // Ignore final backslash
        if (!*(lpTokBlock->lpStringPos+1))
            lpTokBlock->bDone = TRUE;
        }
    else
        {
        STRCPY(lpTok, lpLastPos);

        lpTokBlock->bDone = TRUE;
        }

    return(wRet);
} /* PathTok() */
#pragma optimize("", on)


//************************************************************************
//************************************************************************
//********** BELOW THIS POINT ARE FUNCTIONS LOCAL TO THIS MODULE *********
//************************************************************************
//************************************************************************


#if !defined(SYM_VXD)
//------------------------------------------------------------------------
// INTERNAL_DirMakePath
//
// This is the internal function used for actually carrying out the
// work of the functions "DirMakePath" and "DirMakePathNotify"
//
// NOTE:    Added callback function to be used to notify the parent
//          of the directories that are actually created within this
//          function. - Doug Neal 1/22/93
//
// Returns: standard
//------------------------------------------------------------------------
UINT LOCAL PASCAL INTERNAL_DirMakePath (
    LPSTR               lpTopLevel,     // [in]     Top Level Directory
    DIRMAKENOTIFYPROC   fpCallback,     // [in]     Callback function (optional)
    DWORD               dwParam         // [in]     Caller's parameter
)
{
    auto        LPSTR           lpStart;
    auto        LPSTR           lpTemp;
    auto        BOOL            bPathCharStripped = FALSE;
    auto        UINT            wRet = NOERR;
    auto        BOOL            bIsUNC = FALSE;

                                        // Check if we got a UNC
    if ((lpTopLevel[0] == PATH_CHAR) && (lpTopLevel[1] == PATH_CHAR))
        bIsUNC = TRUE;

    lpStart = lpTopLevel;

                                        // When making a directory remove
                                        // the final slash
    if (StringGetLastChar(lpStart) == PATH_CHAR)
        {
        StringStripChar(lpStart);       // Replace the \ with an End of
                                        // string.
        bPathCharStripped = TRUE;
        }

                                        // If dir already exists, skip
    if (NameGetType(lpStart) & NGT_IS_DIR)
        {
        if (bPathCharStripped)          // Restore to original
            StringAppendChar(lpStart, PATH_CHAR);
        return(wRet);
        }

                                        // Make the necessary directories.
    if (DirMake(lpStart) == ERR)
        {
                                        // If we are unable to make the dir
                                        // we need to assume the failure
                                        // occurred because one or more
                                        // of the directories above it
                                        // did not exist.
                                        // We begin at the root and try to
                                        // make directory at each level.
        lpTemp = lpStart;

                                        // Make path end in backslash
                                        // (we know it doesn't end in '\'
                                        //  right now because we made sure
                                        //  at the start of this function)
        StringAppendChar(lpStart, PATH_CHAR);

                                        // For UNC path, move on to the next
                                        // segment of the path past the server/
                                        // volume part of the path.
        if ((lpStart[0] == PATH_CHAR) && (lpStart[1] == PATH_CHAR))
            {
            lpTemp += 2;
            while (*lpTemp && (*lpTemp != PATH_CHAR))
                lpTemp++;

            if (*lpTemp == EOS)
                lpTemp = lpStart;
            else
                lpTemp++;
            }

        while (*lpTemp)
            {
            if (*lpTemp == PATH_CHAR)
                {

                *lpTemp = EOS;          // Null the string.                 

                                        // Skip the root.                   
                if (!(NameGetType(lpStart) & NGT_IS_ROOT))
                    {
                    if ((wRet = DirMake(lpStart)) == -1)
                        {
                                        // Clean up 
                        //SYM_ASSERT(FALSE);          
                        }
                    else
                        {

                        if (!bIsUNC)
                            DirSet (lpStart);

                        if (fpCallback)
                            {
                                        // Notify caller of directory being
                                        // created 
                            (*fpCallback) (lpStart,dwParam,0L);
                            }
                        }
                    }

                *lpTemp = PATH_CHAR;    // Restore the character we nulled.  
                }

            lpTemp = AnsiNext(lpTemp);
            }

        if (!bPathCharStripped)         // If the original string didn't       
                                        // have a backslash, remove the
                                        // one we added.
            {
            StringStripChar(lpStart);
            }
        }
    else                                // Directory already exists
        {        
        if (fpCallback)
            {
                                        // Notify caller of directory being
                                        // created 
            (*fpCallback) (lpStart,dwParam,0L);
            }
        if (bPathCharStripped)          // Restore the backslash we nulled     
            StringAppendChar(lpStart, PATH_CHAR);
        }        

    return (wRet);
}
#endif // #ifndef SYM_OS2

           

