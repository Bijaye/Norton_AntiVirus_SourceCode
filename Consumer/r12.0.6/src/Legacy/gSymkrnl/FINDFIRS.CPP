/* copyright 1992-94 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/FINDFIRS.CPv   1.13   16 Mar 1998 18:36:02   bsobel  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *      FileFindFirst                                                   *
 *      FileFindNext                                                    *
 *      FileFindClose                                                   *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/FINDFIRS.CPv  $ *
// 
//    Rev 1.13   16 Mar 1998 18:36:02   bsobel
// Minor cleanup, don't called FindClose with INVALID_HANDLE_VALUE
//
//    Rev 1.12   17 Mar 1997 09:25:06   SPAGLIA
// Replaced FileFindNext with FindNextFile in GetNextMatchingAttr. This
// eliminates the unintended recursion.
//
//    Rev 1.11   12 Mar 1997 09:13:04   SPAGLIA
// Enhanced GetNextMatchingAttr() to handle "must-match" attributes
//
//    Rev 1.10   11 Dec 1996 18:23:12   THOFF
// Added FileFindFirstAttr support for OS/2.
//
//    Rev 1.9   06 Nov 1996 15:00:00   SPASNIK
// Changes to go along with FILE.H
//
//    Rev 1.8   06 Nov 1996 14:36:20   SPASNIK
// More goof fixes
//
//    Rev 1.7   06 Nov 1996 14:34:28   SPASNIK
// Fixed many goofs
//
//    Rev 1.6   06 Nov 1996 14:23:26   SPAGLIA
// Fixed date/time conversions for OS/2
//
//    Rev 1.5   06 Nov 1996 13:51:30   SPAGLIA
// More improvements to OS/2
//
//    Rev 1.4   05 Nov 1996 12:20:44   SPAGLIA
// 2nd pass for OS/2
//
//    Rev 1.3   05 Nov 1996 12:11:08   SPAGLIA
// First pass at FindFirst/Next re-write for OS/2
//
//    Rev 1.2   12 Aug 1996 19:38:00   DMISSIOS
// Reverse error handling logic for OS2 FileFindFirst and Next functions.
//
//    Rev 1.1   19 Mar 1996 17:29:26   THOFF
// Added FileFindFirst / FileFindNext support for OS/2 -- yes, OS/2...
//
//    Rev 1.0   26 Jan 1996 20:22:26   JREARDON
// Initial revision.
//
//    Rev 1.20   14 Dec 1994 15:51:10   BRAD
// Fixed warning
//
//    Rev 1.19   14 Dec 1994 15:21:34   BRAD
// Added VxD support
//
//    Rev 1.18   08 Sep 1994 14:02:48   DAVID
// Replaced HFINDFILE with HANDLE to comply with new SDK.
//
//    Rev 1.17   10 Aug 1994 13:42:40   BRAD
// Change 'XOR AX,AX' to 'MOV AX,0'.  Was messing with flags
//
//    Rev 1.16   13 Jul 1994 17:39:04   BRAD
// If 16-bit WIN under Chicago, FileFindFirst() would always fail, since
// it was using the attribute ~VOLUME (0xFFF7), which apparently will
// choke.  Changed to be known valid attributes.
//
//    Rev 1.15   13 Jul 1994 16:41:50   BRAD
// MS added another parameter to 16-bit FindFirst/FindNext extended functions
//
//    Rev 1.14   23 Jun 1994 17:41:20   HENRI
// Fixed compile error for Win32(chicago)
//
//    Rev 1.13   06 Jun 1994 15:44:22   MARKK
// Don't use extended calls under DOSX
//
//    Rev 1.12   04 May 1994 14:04:50   BRAD
// Call FileGetReturnCode() after calling extended functions
//
//    Rev 1.11   04 May 1994 02:18:04   BRAD
// Forgot to remove the call to ConvertPathname() within DOS specific stuff
//
//    Rev 1.10   03 May 1994 22:57:44   BRAD
// Remove functions NFileOpen() and NFileCreate()
//
//    Rev 1.9   14 Apr 1994 11:34:22   BRAD
// In FileFindClose(), was popping off an extra register in Win16/DOS section
//
//    Rev 1.8   07 Apr 1994 11:50:06   BRUCE
// Fixed definition of DTASet() to match prototype (using LPCVOID)
//
//    Rev 1.7   06 Apr 1994 11:55:52   BRUCE
// Fixed compile warnings - some by renaming to .cpp
//
//    Rev 1.6   05 Apr 1994 16:37:56   BRAD
// Must init szAlternateFileName to EOS, since not cleared, if not used
//
//    Rev 1.5   31 Mar 1994 13:42:04   BRAD
// Added new INT 21h functions for FileTime <-> DosTime
//
//    Rev 1.4   31 Mar 1994 10:19:36   BRAD
// MSVCNT headers don't have HFINDFILE defined.  Use HANDLE
//
//    Rev 1.3   25 Mar 1994 17:38:56   BRAD
// fixed typo
//
//    Rev 1.2   25 Mar 1994 13:37:04   BRAD
// New FindFirst stuff
//
//    Rev 1.1   23 Mar 1994 20:13:04   BRAD
// Cleaned up some compile warnings
//
//    Rev 1.0   23 Mar 1994 16:27:22   BRAD
// Initial revision.
 ************************************************************************/

#include "platform.h"
#include "xapi.h"
#include "file.h"

#include "symkrnl.h"

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
#if defined(SYM_OS2)

#define IMCL_DOSFILEMGR
#include <os2.h>

VOID LOCAL PASCAL FileConvertFileTime(LPOS2FILETIME lpFileTime, PFTIME pFTime, PFDATE pFDate)
    {
    lpFileTime->byTwosecs = pFTime->twosecs;
    lpFileTime->byMinute = pFTime->minutes;
    lpFileTime->byHour = pFTime->hours;
    lpFileTime->byDay = pFDate->day;
    lpFileTime->byMonth = pFDate->month;
    lpFileTime->byYear = pFDate->year;
    }

VOID LOCAL PASCAL FileConvertFindData(LPFINDDATA lpFindData, PFILEFINDBUF3 pFileFindBuf)
    {
    FileConvertFileTime(&lpFindData->ftCreation, &pFileFindBuf->ftimeCreation, &pFileFindBuf->fdateCreation);
    FileConvertFileTime(&lpFindData->ftLastAccess, &pFileFindBuf->ftimeLastAccess, &pFileFindBuf->fdateLastAccess);
    FileConvertFileTime(&lpFindData->ftLastWrite, &pFileFindBuf->ftimeLastWrite, &pFileFindBuf->fdateLastWrite);

    lpFindData->dwFileSizeLow = pFileFindBuf->cbFile;
    lpFindData->dwFileAttributes = pFileFindBuf->attrFile;

    STRNCPY(lpFindData->szFileName, pFileFindBuf->achName, pFileFindBuf->cchName);
    lpFindData->szFileName[pFileFindBuf->cchName] = '\0';
    }

HFIND SYM_EXPORT WINAPI FileFindFirst(LPCSTR lpFileName, LPFINDDATA lpFindData)
    {
    HFIND hFind;

    FILEFINDBUF3 FileFindBuf;
    HDIR FileHandle;
    ULONG FindCount;
    APIRET rc;

    hFind = (HFIND)INVALID_HANDLE_VALUE;

    FileHandle = HDIR_CREATE;
    FindCount = 1;
    rc = DosFindFirst(lpFileName, &FileHandle, FILE_READONLY | FILE_HIDDEN | FILE_SYSTEM | FILE_DIRECTORY | FILE_ARCHIVED, &FileFindBuf, sizeof(FileFindBuf), &FindCount, FIL_STANDARD);

    if (rc == 0)
        {
        FileConvertFindData(lpFindData, &FileFindBuf);
        hFind = (HFIND)FileHandle;
        }

    return (hFind);
    }

HFIND SYM_EXPORT WINAPI FileFindFirstAttr(
    LPCSTR lpFileName,
    UINT uAttr,
    LPFINDDATA lpFindData)
    {
    HFIND hFind;

    FILEFINDBUF3 FileFindBuf;
    HDIR FileHandle;
    ULONG FindCount;
    APIRET rc;

    hFind = (HFIND)INVALID_HANDLE_VALUE;

    FileHandle = HDIR_CREATE;
    FindCount = 1;
    rc = DosFindFirst(lpFileName, &FileHandle, uAttr, &FileFindBuf, sizeof(FileFindBuf), &FindCount, FIL_STANDARD);

    if (rc == 0)
        {
        FileConvertFindData(lpFindData, &FileFindBuf);
        hFind = (HFIND)FileHandle;
        }

    return (hFind);
    }

BOOL SYM_EXPORT WINAPI FileFindNext(HFIND hFind, LPFINDDATA lpFindData)
    {
    BOOL bRet;

    FILEFINDBUF3 FileFindBuf;
    ULONG FindCount;
    APIRET rc;

    bRet = FALSE;

    FindCount = 1;
    rc = DosFindNext((HDIR)hFind, &FileFindBuf, sizeof(FileFindBuf), &FindCount);

    if (rc == 0)
        {
        FileConvertFindData(lpFindData, &FileFindBuf);
        bRet = TRUE;
        }

    return (bRet);
    }

BOOL SYM_EXPORT WINAPI FileFindClose(HFIND hFind)
    {
    return (DosFindClose((HDIR)hFind) == 0);
    }

#else

#if defined(SYM_WIN) || defined(SYM_DOS) || defined(SYM_VXD)

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
#if !defined(SYM_WIN32)

#if defined(SYM_VXD)
extern "C" WORD SYM_EXPORT WINAPI __FileFindFirstOld(LPCSTR lpFileName, UINT uAttr);
extern "C" BOOL SYM_EXPORT WINAPI __FileFindFirstNew(LPCSTR lpFileName, UINT uAttr, LPFINDDATA lpFindData, HFIND FAR *lphFind);
extern "C" WORD SYM_EXPORT WINAPI __FileFindNextOld(VOID);
extern "C" BOOL SYM_EXPORT WINAPI __FileFindNextNew(HFIND hFind, LPFINDDATA lpFindData);
extern "C" BOOL SYM_EXPORT WINAPI __FileFindCloseNew(HFIND hFind);
#endif

/*----------------------------------------------------------------------*/
/* OLDFINDDATA                                                          */
/*                                                                      */
/*  Used internally to manage the old FindFirst structures.             */
/*----------------------------------------------------------------------*/

typedef struct _OLDFINDDATA {
                                        /* This top portion must be     */
                                        /* exactly 21 BYTEs long        */
    BYTE        reserved1;              /* Drive #                      */
    BYTE        pattern[11];
    BYTE        searchAttribute;
    WORD        indexWithinParentsDir;  /* Index within dir, base-0     */
    WORD        parentsScn;             /* Parent's SCN                 */
    DWORD       reserved2;              /* UNKNOWN                      */

    BYTE        byFileAttributes;
    WORD        wTime;
    WORD        wDate;
    DWORD       dwFileSizeLow;
    char        szFileName[13];

} OLDFINDDATA, *POLDFINDDATA, NEAR *NPOLDFINDDATA, FAR *LPOLDFINDDATA;


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
VOID LOCAL PASCAL CopyOldFindDataToFindData(LPFINDDATA lpFindData,
                                            LPOLDFINDDATA lpOFD)
{
                                        // Don't MEMSET 'findData, since
                                        // the lpOFD is within it!
    lpFindData->dwFileAttributes = lpOFD->byFileAttributes;
    MEMSET(&lpFindData->ftCreationTime,   0, sizeof(lpFindData->ftCreationTime));
    MEMSET(&lpFindData->ftLastAccessTime, 0, sizeof(lpFindData->ftLastAccessTime));
    DosTimeToFileTime(lpOFD->wDate, lpOFD->wTime, &lpFindData->ftLastWriteTime);
    lpFindData->dwFileSizeHigh   = 0;
    lpFindData->dwFileSizeLow    = lpOFD->dwFileSizeLow;
    STRCPY(lpFindData->szFileName, lpOFD->szFileName);
    lpFindData->szAlternateFileName[0] = EOS;
}


/*********************************************************************
** DTASet()
**
** Description:
**    This function sets the disk transfer area.
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

#if !defined(SYM_VXD)
#pragma optimize("", off)
VOID SYM_EXPORT WINAPI DTASet(LPCVOID lpDTA)
{
   _asm
        {
        push    ds
        lds     dx,lpDTA
        MOV     ah,1Ah                  //Set the disk transfer address
        DOS
        pop     ds
        }
} /* DTASet() */
#pragma optimize("", on)
#endif

/*********************************************************************
** DTAGet()
**
** Description:
**    This function sets the disk transfer area.
**
**    Warning of no return value is OK in here.
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

#if !defined(SYM_VXD)
#pragma optimize("", off)
LPVOID SYM_EXPORT WINAPI DTAGet(VOID)
{
    auto        UINT    retOff;
    auto        UINT    retSeg;

    _asm {
        push    ds
        MOV     ah,2Fh                  // Set the disk transfer address
        DOS
        mov     retOff, bx              // return offset in AX
        mov     retSeg, es              // return segment/selector in DX
        pop     ds
        }

    return(MAKELP(retSeg, retOff));
} /* DTAGet() */
#pragma optimize("", on)
#endif



/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
#pragma optimize("", off)
HFIND LOCAL PASCAL _FileFindFirstDOS(LPCSTR lpFileName, UINT uAttr, LPFINDDATA lpFindData)
{
    auto        WORD            wRetCode;
    auto        BOOL            bSupported = TRUE;
    auto        HFIND           hFind;
    auto        LPOLDFINDDATA   lpOFD;
    auto        LPVOID          lpOldDTA;


#if defined(SYM_VXD)

    bSupported = __FileFindFirstNew(lpFileName, uAttr, lpFindData, &hFind);
    if ( bSupported )
        goto doneFindFirst;

#elif !defined(SYM_DOSX)
                                        //-------------------------------
                                        // First try using LFN scheme.
                                        //-------------------------------
    _asm
        {
        push    ds
        push    es
        push    si
        push    di

        mov     ax,714Eh                ; Extended Find First
        mov     cx,uAttr                ; Get attribute
        lds     dx,lpFileName           ; Get the name to search for
        les     di,lpFindData           ; Get the results buffer
        mov     si,0                    ; Return date/time as 64-bit format
        stc                             ; Assume not supported
        DOS                             ; Make the DOS call
        jnc     done
        cmp     ax, 7100h               ; Unsupported?
        mov     ax, -1                  ; Return invalid handle
        jne     done                    ; No, supported, but error
        mov     bSupported, 0           ; Not supported.  Use old method

done:
        mov     hFind, ax
        call    FileGetReturnCode       //  set AX to ERR or NOERR
        pop     di
        pop     si
        pop     es
        pop     ds
        }

    if ( bSupported )
        goto doneFindFirst;
#endif
                                        //-------------------------------
                                        // Extended function not supported.
                                        // Need to use old DTA method.
                                        //-------------------------------
                                        // Set DTA within 'szFileName'
    lpOFD = (LPOLDFINDDATA) (lpFindData->szAlternateFileName - sizeof(OLDFINDDATA) - 10);

    lpOldDTA = DTAGet();                // Preserve current DTA
    DTASet(lpOFD);

#if defined(SYM_VXD)
    wRetCode = __FileFindFirstOld(lpFileName, uAttr);
#else
    _asm
        {
        push    ds

        lds     dx,lpFileName
        mov     cx,uAttr                // Get attribute
        MOV     ah,4Eh                  // Search for first match
        DOS
        pop     ds
        call    FileGetReturnCode       //  set AX to ERR or NOERR
        mov     wRetCode, ax            //  Save return value
        }
#endif

    DTASet(lpOldDTA);                   // Restore DTA

    if (wRetCode != ERR)
        {
        CopyOldFindDataToFindData(lpFindData, lpOFD);
        hFind = (HFIND)1;                      // For now, anything but invalid state
        }
    else
        hFind = (HFIND)INVALID_HANDLE_VALUE;

doneFindFirst:
#if defined(SYM_WIN)
    if ( hFind != INVALID_HANDLE_VALUE )
        {
        NOemToAnsi(lpFindData->szFileName,
                   lpFindData->szFileName);
        NOemToAnsi(lpFindData->szAlternateFileName,
                   lpFindData->szAlternateFileName);
        }
#endif
    return(hFind);
}
#pragma optimize("", on)


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
#pragma optimize("", off)
BOOL LOCAL PASCAL _FileFindNextDOS(HFIND hFind, LPFINDDATA lpFindData)
{
    auto        BOOL            bResult;
    auto        BOOL            bSupported = TRUE;
    auto        LPOLDFINDDATA   lpOFD;
    auto        WORD            wRetCode;
    auto        LPVOID          lpOldDTA;


                                        //-------------------------------
                                        // First try using LFN scheme.
                                        //-------------------------------
#if defined(SYM_VXD)

    bSupported = __FileFindNextNew(hFind, lpFindData);
    if ( bSupported )
        goto doneFindNext;

#elif !defined(SYM_DOSX)
    _asm
        {
        push    es
        push    di
        push    si

        mov     ax,714Fh                ; Extended Find First
        mov     bx,hFind                ; Get Search handle
        les     di,lpFindData           ; Get the results buffer
        mov     si,0                    ; Return date/time as 64-bit format
        stc                             ; Assume not supported
        DOS                             ; Make the DOS call
        jnc     noErr
        cmp     ax, 7100h               ; Unsupported?
        mov     ax, 0                   ; Return FALSE (error)
        jne     done                    ; No, supported, but error
        mov     bSupported, 0           ; Not supported.  Use old method
        jmp     short done
noErr:
        mov     ax, 1                   ; Return TRUE

done:
        mov     bResult, ax
        call    FileGetReturnCode       //  set AX to ERR or NOERR
        pop     si
        pop     di
        pop     es
        }

    if ( bSupported )
        goto doneFindNext;
#endif
                                        //-------------------------------
                                        // Extended function not supported.
                                        //-------------------------------
                                        // Set DTA within 'szFileName'
    lpOFD = (LPOLDFINDDATA) (lpFindData->szAlternateFileName - sizeof(OLDFINDDATA) - 10);

    lpOldDTA = DTAGet();                // Preserve current DTA
    DTASet(lpOFD);

#if defined(SYM_VXD)
    wRetCode = __FileFindNextOld();
#else
    _asm
        {
        MOV     AH,4Fh                  // Find the next match
        DOS
        call    FileGetReturnCode       //  set AX to to ERR or NOERR
        mov     wRetCode, ax            //  Get the return value
        }
#endif

    DTASet(lpOldDTA);                   // Restore DTA

    if (wRetCode != ERR)
        CopyOldFindDataToFindData(lpFindData, lpOFD);

    bResult = (wRetCode != ERR) ? TRUE : FALSE;

doneFindNext:
#if defined(SYM_WIN)
    if (bResult)
        {
        NOemToAnsi(lpFindData->szFileName,
                   lpFindData->szFileName);
        NOemToAnsi(lpFindData->szAlternateFileName,
                   lpFindData->szAlternateFileName);
        }
#endif
    return(bResult);
}
#pragma optimize("", on)


#endif  // !defined(SYM_WIN32)

/*----------------------------------------------------------------------*/
/* This finds the first file that matchs the name in the ASCIIZ string. */
/*      FileFindFirst(name, &info);                                     */
/* where:       name    ASCIIZ name of the file, with wild cards        */
/*              info    FINDDATA                                        */
/*                                                                      */
/* Errors:      Search Handle                                           */
/*              INVALID_HANDLE_VALUE (-1), if error                     */
/*----------------------------------------------------------------------*/

HFIND SYM_EXPORT WINAPI FileFindFirst(LPCSTR lpFileName, LPFINDDATA lpFindData)
{
    auto        char    szConvertedFile[SYM_MAX_PATH];

                                        // Not set, if not used
    lpFindData->szAlternateFileName[0] = EOS;

    ExpandAndConvertFile(lpFileName, szConvertedFile);

#if defined(SYM_WIN32)

                                        // Mark hidden field as NOT searching
                                        // on attributes
    lpFindData->bSearchOnAttributes = FALSE;
    return ((HFIND) FindFirstFile(szConvertedFile, (LPWIN32_FIND_DATA) lpFindData));

#else

    return (_FileFindFirstDOS(szConvertedFile, (WORD) (FA_READ_ONLY | FA_HIDDEN | FA_SYSTEM | FA_DIR | FA_ARCHIVE), lpFindData));

#endif

}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
#if defined(SYM_WIN32)
BOOL LOCAL PASCAL GetNextMatchingAttr(HFIND hFind, UINT uAttr, LPFINDDATA lpFindData)
{
    auto        BOOL    bOk = TRUE;
    auto        BOOL    bFound = FALSE;
    auto        DWORD   dwAttribMay;
    auto        DWORD   dwAttribMust;
    auto        UINT    uAttribMay;
    auto        UINT    uAttribMust;


    if ( hFind == INVALID_HANDLE_VALUE )
        bOk = FALSE;
                                        // See if this file matches the attr
                                        // requirement.  If not, continue
                                        // looking.
    while (bOk)
        {
        dwAttribMay = lpFindData->dwFileAttributes & (FA_DIR | FA_SYST | FA_HDDN);
        uAttribMay = uAttr & (FA_DIR | FA_SYST | FA_HDDN);
        dwAttribMust = lpFindData->dwFileAttributes & (FA_DIR | FA_SYST | FA_HDDN | FA_ARCH | FA_READ);
        uAttribMust = (uAttr >> 8) & (FA_DIR | FA_SYST | FA_HDDN | FA_ARCH | FA_READ);

        if ((dwAttribMay | uAttribMay) != uAttribMay || (dwAttribMust & uAttribMust) != uAttribMust)
            bOk = FindNextFile((HANDLE) hFind, (LPWIN32_FIND_DATA) lpFindData);

        else
            {
            bFound = TRUE;
            break;
            }
        }

    return(bFound);
}

#endif

/*----------------------------------------------------------------------*/
/* This finds the first file that matchs the name in the ASCIIZ string. */
/*      FileFindFirstAttr(name, wAttr, &info);                          */
/* where:       name    ASCIIZ name of the file, with wild cards        */
/*              info    FINDDATA                                        */
/*                                                                      */
/* Errors:      Search Handle                                           */
/*              INVALID_HANDLE_VALUE (-1), if error                     */
/*----------------------------------------------------------------------*/

HFIND SYM_EXPORT WINAPI FileFindFirstAttr(
    LPCSTR lpFileName,
    UINT uAttr,
    LPFINDDATA lpFindData)
{
                                        // Not set, if not used
    auto        char    szConvertedFile[SYM_MAX_PATH];


                                        // Not set, if not used
    lpFindData->szAlternateFileName[0] = EOS;

    ExpandAndConvertFile(lpFileName, szConvertedFile);

#if defined(SYM_WIN32)
    HFIND       hFind;
    BOOL        bFound = FALSE;

    hFind = FileFindFirst(szConvertedFile, lpFindData);
    bFound = GetNextMatchingAttr(hFind, uAttr, lpFindData);

    if (!bFound)
        {
        if(hFind != (HFIND)INVALID_HANDLE_VALUE)
            {
            FileFindClose(hFind);
            hFind = (HFIND)INVALID_HANDLE_VALUE;
            }
        }
    else
        {
                                        // Mark hidden field as being a search
                                        // on attributes
        lpFindData->bSearchOnAttributes = TRUE;
        lpFindData->dwSearchAttributes  = uAttr;
        }

    return(hFind);

#else

    return (_FileFindFirstDOS(szConvertedFile, uAttr, lpFindData));

#endif
}


/*----------------------------------------------------------------------*/
/* FileFindNext                                                         */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI FileFindNext(HFIND hFind, LPFINDDATA lpFindData)
{
                                        // Not set, if not used
    lpFindData->szAlternateFileName[0] = EOS;

#if defined(SYM_WIN32)

    BOOL        bOk;

    bOk = FindNextFile((HANDLE) hFind, (LPWIN32_FIND_DATA) lpFindData);
    if (bOk && lpFindData->bSearchOnAttributes)
        bOk = GetNextMatchingAttr(hFind, lpFindData->dwSearchAttributes, lpFindData);
    return (bOk);

#else

    return (_FileFindNextDOS(hFind, lpFindData));

#endif
}
#pragma optimize("", on)


/*----------------------------------------------------------------------*/
/* FileFindClose                                                        */
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI FileFindClose(HFIND hFind)
{
#if defined(SYM_WIN32)

    return ( FindClose((HANDLE)hFind) );

#else

    auto        BOOL            bSupported = TRUE;

#if defined(SYM_VXD)

    bSupported = __FileFindCloseNew(hFind);
    if ( bSupported )
        return (TRUE);

#elif !defined(SYM_DOSX)

    auto        BOOL            bResult;

                                        //-------------------------------
                                        // First try using LFN scheme.
                                        //-------------------------------
    _asm
        {
        mov     ax,71A1h                ; Extended Find Close
        mov     bx,hFind                ; Get Search handle
        stc                             ; Assume not supported
        DOS                             ; Make the DOS call
        jnc     noErr
        cmp     ax, 7100h               ; Unsupported?
        mov     ax, 0                   ; Return FALSE (error)
        jne     done                    ; No, supported, but error
        mov     bSupported, 0           ; Not supported.  Use old method
        jmp     short done
noErr:
        mov     ax, 1                   ; Return TRUE

done:
        mov     bResult, ax
        call    FileGetReturnCode       //  set AX to ERR or NOERR
        }

    if ( bSupported )
        return (bResult);
#endif
                                        //-------------------------------
                                        // Extended function not supported.
                                        // Just return TRUE.
                                        //-------------------------------
    return(TRUE);
#endif
}


#endif  // defined(SYM_WIN) || defined(SYM_DOS) || defined(SYM_VXD)

#endif

