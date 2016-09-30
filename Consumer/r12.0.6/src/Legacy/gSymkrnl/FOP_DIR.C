/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/fop_dir.c_v   1.3   29 May 1997 02:22:22   RStanev  $ *
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
 *      DirMake                                                         *
 *      DirRemove                                                       *
 *      DirGet                                                          *
 *      DirSet                                                          *
 *      DirParsePath                                                    *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/fop_dir.c_v  $ *
// 
//    Rev 1.3   29 May 1997 02:22:22   RStanev
// Enabled DirMake() for SYM_NTK.
//
//    Rev 1.2   12 Nov 1996 12:39:04   SPAGLIA
// Converted NOemToAnsi and NAnsiToOem to OemToAnsi and AnsiToOEM.
// For LFNs, it does not make sense to fold paths to UPPERCASE. Also,
// because of a bug, using DirSet with a UPPERCASE path will cause
// DirGet to return the same UPPERCASE path, when if fact it may be
// mixed case. In general, do not rely on DirGet to return the correct
// case of a path.
//
//    Rev 1.1   11 Nov 1996 19:21:48   SPAGLIA
// Modifed DirGet() to not fold directory to UPPERCASE
//
//    Rev 1.0   26 Jan 1996 20:21:58   JREARDON
// Initial revision.
//
//    Rev 1.21   09 Nov 1994 20:42:40   BRUCE
// Fixed DirParsePath use of AnsiNext() when adding EOSs to string
//
//    Rev 1.20   09 Nov 1994 16:21:22   BRAD
// DBCS-enabled
//
//    Rev 1.19   01 Jun 1994 13:13:36   MARKK
// Don't make extended calls under DX
//
//    Rev 1.18   18 May 1994 15:52:34   MARKK
// Added disk.h
//
//    Rev 1.17   23 Mar 1994 19:41:40   BRAD
// Added LFN support for DOS and WIN16
//
//    Rev 1.16   15 Mar 1994 12:34:10   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.15   25 Feb 1994 12:22:18   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.14   07 Feb 1994 12:16:08   PGRAVES
// Win32: DirGet.
//
//    Rev 1.13   03 Feb 1994 21:35:12   PGRAVES
// Win32 changes.
//
//    Rev 1.12   03 May 1993 18:55:18   BRAD
// Added DirGetCurrent().
//
//    Rev 1.11   15 Feb 1993 21:03:56   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.10   12 Feb 1993 04:28:16   ENRIQUE
// No change.
//
//    Rev 1.9   19 Oct 1992 18:31:42   BILL
// const changes
//
//    Rev 1.8   21 Sep 1992 21:24:14   BRUCE
//
// Fixed syntax error from last fix (oops!)
//
//    Rev 1.7   21 Sep 1992 21:23:10   BRUCE
// Fixed DirMake to set wRet before returning it
//
//    Rev 1.6   20 Sep 1992 22:58:14   ENRIQUE
// Fixed DirGet.  It was always tring to get the current directory on drive X.
// The drive letter was never being set in the windows path.
//
//    Rev 1.5   15 Sep 1992 09:59:28   ED
// Removed NSTRING.H
//
//    Rev 1.4   10 Sep 1992 13:43:54   BRUCE
// Eliminated unnecessary includes
//
//    Rev 1.3   08 Sep 1992 13:04:04   HENRI
// Fixed use of NAnsiToOem and made sure that driveletter in
// DirGet was uppercased.
//
//    Rev 1.2   08 Sep 1992 12:02:58   HENRI
// Because NAnsiToOem does not copy the source to destination for
// DOS programs, and because we didn't want to add overhead of a
// StringCopy for DOS programs, we had to special case the code for
// DOS so that DirGet, DirSet, DirRemove, and DirChange would work.
//
//    Rev 1.1   03 Sep 1992 08:13:34   ED
// Added functions to process error codes consistently
//
//    Rev 1.0   03 Sep 1992 08:02:44   ED
// Initial revision.
//
//    Rev 1.0   27 Aug 1992 09:35:06   ED
// Initial revision.
 ************************************************************************/

#include "platform.h"

#if defined(SYM_NTK)
#include "file.h"
#elif !defined(SYM_WIN32)
#include "undoc.h"
#include "file.h"
#include "disk.h"
#endif

#include "xapi.h"


/*---------------------------------------------------------------------------*/
/* DirMake                                                                   */
/*    This routine is used to make a directory.                              */
/*                                                                           */
/* INPUTS                                                                    */
/*    LPSTR     lpPathName                                                   */
/*                                                                           */
/* RETURNS                                                                   */
/*    -1 if error, else 0.                                                   */
/*---------------------------------------------------------------------------*/
#if defined(SYM_NTK)
UINT SYM_EXPORT WINAPI DirMake(LPCTSTR lpPathName)
    {
    auto NTSTATUS          status;
    auto PTSTR             pszNTPathName;
    auto UNICODE_STRING    PathName;
    auto OBJECT_ATTRIBUTES ObjectAttributes;
    auto HANDLE            hPath;
    auto IO_STATUS_BLOCK   IoStatusBlock;

    SYM_ASSERT ( lpPathName );
    SYM_VERIFY_STRING ( lpPathName );

    if ( ! ( pszNTPathName = FileCreateNativeNTName ( lpPathName ) ) )
        {
        return ( (UINT)-1 );
        }

    RtlInitUnicodeString ( &PathName, pszNTPathName );

    InitializeObjectAttributes ( &ObjectAttributes,
                                 &PathName,
                                 OBJ_CASE_INSENSITIVE,
                                 NULL,
                                 NULL );

    status = ZvCreateFile ( &hPath,
                            FILE_LIST_DIRECTORY | SYNCHRONIZE,
                            &ObjectAttributes,
                            &IoStatusBlock,
                            NULL,
                            FILE_ATTRIBUTE_NORMAL,
                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                            FILE_CREATE,
                            FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT,
                            NULL,
                            0 );

    FileDestroyNativeNTName ( pszNTPathName );

    if ( NT_SUCCESS ( status ) )
        {
        ZvClose ( hPath );

        return ( 0 );
        }

    // Set last error.

    return ( (UINT)-1 );
    }

#elif !defined(SYM_WIN32)
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DirMake(LPCSTR lpPathName)
{
    auto        UINT            wRCode;

#ifdef SYM_WIN
    auto        BYTE            szOemPathName[MAX_INTL_STR];
    auto        LPBYTE          lpszOemPathName = szOemPathName;

    AnsiToOem(lpPathName, lpszOemPathName);
#endif

    _asm
        {
        push    ds                      //  Save the DS value

#ifdef SYM_WIN
        lds     dx,lpszOemPathName      //  Load the path name
#else
        lds     dx,lpPathName           //  Load the path name
#endif
#if !defined(SYM_DOSX)
        mov     ax,7139h                // Extended MakeDir
        stc
        DOS
        jnc     doneDirMake
        cmp     ax,7100h                // Supported?
        jne     doneDirMake             // Yes, but really an error
#endif

        mov     ah,39h
        DOS

doneDirMake:
        pop     ds
        call    FileGetReturnCode       //  set AX to to ERR or NOERR
        mov     wRCode, ax
        }

    return (wRCode);
}
#pragma optimize("", on)
#endif                                  // !SYM_WIN32 && !SYM_NTK

/*---------------------------------------------------------------------------*/
/* DirRemove                                                                 */
/*    This routine is used to remove a directory from the disk               */
/*                                                                           */
/* INPUTS                                                                    */
/*    LPSTR     lpPathName      long pointer to the directory to delete      */
/*                                                                           */
/* RETURNS                                                                   */
/*    -1 if error, else 0.                                                 */
/*---------------------------------------------------------------------------*/
#if !defined(SYM_WIN32) && !defined(SYM_NTK)
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DirRemove(LPCSTR lpPathName)
{
    auto        UINT            wRCode;

#ifdef SYM_WIN
    auto        BYTE            szOemPathName[MAX_INTL_STR];
    auto        LPBYTE          lpszOemPathName = szOemPathName;

    AnsiToOem(lpPathName, lpszOemPathName);
#endif

    _asm
        {
        push    ds                      //  save the value of ds

#ifdef SYM_WIN
        lds     dx,lpszOemPathName
#else
        lds     dx,lpPathName
#endif
#if !defined(SYM_DOSX)
        mov     ax,713Ah                // Extended MakeRemove
        stc
        DOS
        jnc     doneDirRemove
        cmp     ax,7100h                // Supported?
        jne     doneDirRemove           // Yes, but really an error
#endif

        mov     ah,3Ah
        DOS

doneDirRemove:
        pop     ds
        call    FileGetReturnCode       //  set AX to to ERR or NOERR
        mov     wRCode,ax
        }

    return (wRCode);
}
#pragma optimize("", on)
#endif                                  // !SYM_WIN32 && !SYM_NTK

/*----------------------------------------------------------------------*/
/* DirGetCurrent                                                        */
/*      This function is used to get the current directory on the       */
/*      current drive.                                                  */
/*      The directory returned is of the format:                        */
/*              A:\xxxxx\yyyyy                                          */
/*                                                                      */
/* INPUTS                                                               */
/*    LPSTR     lpDirName       returns a pointer to the current dir.   */
/*                                                                      */
/* RETURNS                                                              */
/*    -1 if error, else 0.                                              */
/*----------------------------------------------------------------------*/
#if !defined(SYM_WIN32) && !defined(SYM_NTK)
UINT SYM_EXPORT WINAPI DirGetCurrent(LPSTR lpDirName)
{
    return( DirGet(DiskGet(), lpDirName) );
}
#endif                                  // !SYM_WIN32 && !SYM_NTK

/*----------------------------------------------------------------------*/
/* DirGet                                                               */
/*      This function is used to get the current directory on a drive.  */
/*      The directory returned is of the format:                        */
/*              A:\xxxxx\yyyyy                                          */
/*                                                                      */
/* INPUTS                                                               */
/*    BYTE      cDriveLetter    drive letter                            */
/*    LPSTR     lpDirName       returns a pointer to the current dir.   */
/*                                                                      */
/* RETURNS                                                              */
/*    -1 if error, else 0.                                              */
/*----------------------------------------------------------------------*/
#ifdef SYM_WIN32
UINT SYM_EXPORT WINAPI DirGet(BYTE cDriveLetter, LPSTR lpDirName)
{
    char szCurrentDir[SYM_MAX_PATH];
    BOOL bSuccess;

    GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);
    cDriveLetter = CharToUpper(cDriveLetter);

    /************ removed
    CharUpper(szCurrentDir);
    ************/

    if (CharToUpper(szCurrentDir[0]) == cDriveLetter)
    {
                                        // The current directory happens to
                                        // be on the drive we're interested in.
        STRCPY(lpDirName, szCurrentDir);
        return NOERR;
    }

                                        // Change to the drive in question.
    lpDirName[0] = cDriveLetter;
    lpDirName[1] = ':';
    lpDirName[2] = '\0';
    if (!SetCurrentDirectory(lpDirName))
        return ERR;                     // Invalid drive, maybe.

                                        // Get the current directory on that
                                        // drive.
    bSuccess = GetCurrentDirectory(SYM_MAX_PATH, lpDirName);

                                        // Restore old current directory.
    SetCurrentDirectory(szCurrentDir);

    return (bSuccess ? NOERR : ERR);
}
#elif !defined(SYM_NTK)
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DirGet(BYTE cDriveLetter, LPSTR lpDirName)
{
    auto        UINT            wRetCode = 0;

#ifdef SYM_WIN
    auto        BYTE            szOemDirBuff[MAX_INTL_STR];
    auto        LPBYTE          lpszOemDirBuff = szOemDirBuff;
    auto        BYTE            cOemDriveLetter;
#endif


    FileInitPath (lpDirName);


#ifdef SYM_WIN
    AnsiToOem(lpDirName, lpszOemDirBuff);

                                        // Get upper case oem drive letter
    lpszOemDirBuff[0] = cOemDriveLetter = CharToUpper(cDriveLetter);
    lpszOemDirBuff += 3;                // Point to place for dos to return
                                        // path minus drive letter, colon,
                                        // and leading backslash
#else
    lpDirName[0] = cDriveLetter = (BYTE)CharToUpper(cDriveLetter);
    lpDirName += 3;
#endif

    _asm
        {
        push    ds

#ifdef SYM_WIN
        lds     si,lpszOemDirBuff       //  move the offset into si
        mov     dl,cOemDriveLetter      //  Store the drive letter in dl
#else
        lds     si,lpDirName            //  move the offset into si
        mov     dl,cDriveLetter
#endif
        sub     dl,'A'-1                //  Convert to a drive number

#if !defined(SYM_DOSX)
        mov     ax,7147h                // Extended DirGet
        stc
        DOS
        jnc     doneDirGet
        cmp     ax,7100h                // Supported?
        jne     doneDirGet              // Yes, but really an error
#endif

        mov     ah,47h
        DOS

doneDirGet:
        pop     ds
        call    FileGetReturnCode       //  set AX to to ERR or NOERR
        mov     wRetCode,ax
        }

#ifdef SYM_WIN
    if (!wRetCode)
        OemToAnsi(szOemDirBuff, lpDirName);
#endif

    return (wRetCode);
}
#pragma optimize("", on)
#endif                                  // !SYM_WIN32 && !SYM_NTK

/*-------------------------------------------------------------------------*/
/* DirSet                                                                  */
/*    This function is used to get change the current directory to the     */
/*    specified path.                                                      */
/*                                                                         */
/* INPUTS                                                                  */
/*    LPSTR     lpDirName       returns a pointer to the directory to      */
/*                              change to.                                 */
/*                                                                         */
/* RETURNS                                                                 */
/*    -1 if error, else 0.                                                 */
/*-------------------------------------------------------------------------*/

#if !defined(SYM_WIN32) && !defined(SYM_NTK)
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DirSet(LPCSTR lpDirName)
{
    auto        UINT            wRCode = 0;

#ifdef SYM_WIN
    auto        BYTE            szOemPathName[MAX_INTL_STR];
    auto        LPBYTE          lpszOemPathName = szOemPathName;

    AnsiToOem(lpDirName, lpszOemPathName);
#endif

    SetDirChangedFlag();

    _asm
        {
        push    ds

#ifdef SYM_WIN
        lds     dx,lpszOemPathName      //  move the offset into si
#else
        lds     dx,lpDirName            //  move the offset into si
#endif
#if !defined(SYM_DOSX)
        mov     ax,713Bh                // Extended DirSet
        stc
        DOS
        jnc     doneDirSet
        cmp     ax,7100h                // Supported?
        jne     doneDirSet              // Yes, but really an error
#endif

        mov     ah,3Bh
        DOS

doneDirSet:
        pop     ds
        call    FileGetReturnCode       //  set AX to to ERR or NOERR
        mov     wRCode,ax
        }

    return (wRCode);
}
#pragma optimize("", on)
#endif                                  // !SYM_WIN32 && !SYM_NTK

/*----------------------------------------------------------------------*
 * This procedure converts the path into a list of ASCIIZ strings and   *
 * returns a count of the number of paths found.                        *
 *                                                                      *
 * When this procedure counts paths, it can also include null strings   *
 * such as when you have ;; in the path.  In this case we have a path   *
 * that consists of and end-of-string mark '\0'.                        *
 *----------------------------------------------------------------------*/
#if !defined(SYM_NTK)
UINT SYM_EXPORT WINAPI DirParsePath(LPSTR path)
{
    register    UINT    i = 0;          /* Count the number of paths    */
    auto        BOOL    start = TRUE;   /* When we first start a path   */


    while (*path)                       /* Look through entire path     */
        {
        if (start)                      /* Have we just started a path? */
            {                           /* Yes, then count this path    */
            i++;                        /* Count this path              */
            start = FALSE;              /* We're no longer at the start */
            }
        if (*path == ';')               /* Is this end of a path?       */
            {
            *path++ = '\0';             /* Yes, write end of string     */
            start = TRUE;               /* About to start another path  */
            }
        else
            path = AnsiNext(path);      /* Move to next character       */
        }

    return(i);                          /* Return number of paths found */
}
#endif // #if !defined(SYM_NTK)


